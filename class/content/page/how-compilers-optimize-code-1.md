---
title: "How Compilers Optimize the Source Code (Part I)"
date: 2019-10-13
draft: false
---

A previous aricle of this course was entirely devoted to an overview of Optimization in Compilers. Reading that article is a clear prerequisite for this one (see the references at the end).

We did study the following concepts:

- **What** Compilers optimize.
- **When** Compilers optimize, that is, at which point in the compilation process.
- **Where** they optimize, that is, what parts of the code.

In this article I want to study **How** Compilers Optimize source code. Let me start with an overview and the whole concept, and then go into many practical examples.

## A Bag of Tricks

The first thing to know about Optimization in Compilers is that the whole module is like **a bag full of tricks**.

What does that mean? Basically, the Compilers will take repeated looks at the code and do the following:

1. Search in the bag any trick that can be applied.
2. If applied at least one trick, start over from step 1.
3. Otherwise exit.

The other point worth remembering is that the code the Compilers is looking at while doing Optimization is very likely an **Intermediate Code** as you know from my past articles. To the best of my knowledge, there's no Compiler that does that directly on the original source code.

On the one hand, I am going to illustrate and discuss with you many examples drawn from this bag of tricks.

On the other hand, it's not possible to explore all tricks in the bag, because there's no limit to them. In fact, **Optimization is a very active research field**, and new advanced techniques are discovered every now and again.

Now let's begin exploring some Optimization technique, starting from Local Optimization.

## Local Optimization

Techniques that belong to the Local Optimization "bag" focus on optimizing one single basic block. Citing from my previous article:

_A basic block of code is a sequence of instructions without any label, except possibly for the first line, and without any jump, except possibly for the last line._

_The idea is that a basic block is a piece of code whose execution is completely predictable. There's no entry point for other parts of the code (except for the first line, maybe), and there's no exit point (except maybe for a final jump). Therefore, we know what's going to happen there, and we can optimize its behavior._

Let's start with some simple cases.

### Remove statements

Sometime we can delete an entire statement. For instance in the following code

```
x := x + 0
x := x * 1
```

the two statements above can simply be removed. Removing one statement is one of the best optimization you can do, in terms of performances.

If you think this is a very unlikely case, or an odd example, you are wrong!

The reason is that, even though nobody would explicitly write `x + 0`, sometimes you have an expression such as `x + y` and the Compiler is able to infer that `y` will always be zero at that point in the program.

Consider that `y` may actually be a complex nested expression, and the Compiler is smarter than the programmer in figuring out this sort of things.

### Simplify statement

A basic example is the following line 

```
x := x*0
```

that can be simplified into

```
x := 0
```

which is obviously more efficient. Another example:

```
y := y^2
```

can be simplified into

```
y := y*y
```

This is always a good optimization to do, because in 99% of the machines, the power operator is not built-in the CPU capabilities, so its implementation would go down to a series of complex operation, maybe with an external library.

Another case is

```
z := z * 8
```

Which can be simplified into

```
z := z << 3 (bitwise shift)
```

This last example is likely to be more efficient, even though in modern machines the multiplication is as fast as the other operations, bitwise shift included.

## Constant Folding

This is a very important technique. It's absolutely worth knowing it.

The main point is that one of the most powerful optimizations we can perform is to compute the result of some operations at compile time. This is often possible when the operands are constant. For example, if we have

```
x := y op z
```

And `y`, `z` are both constant (immediate values), then we can compute the result at compile time and replace the instruction with an assignment to constant:

```
x := 2 + 3 ⇒ x := 5
```

The same concept applies to the predicate of a conditional statement. When we have

```
if (x < 2) jump L
```

and we are actually able to say that `x` will always evaluate to some constant, then the entire if block can be:

- Deleted, if the condition always evaluates to false.
- Replaced with an unconditional jump, if the predicate is always true.

### Danger!

There's one uncommon case when constant folding is dangerous. Even though it's unlikely, it's very instructive.

Suppose we have two machines, X and Y, and we want to compile some code on machine X and then run the generated code in machine Y.

This scenario could happen when Y is a weak machine, for example some embedded system, and therefore is not suitable to develop code.

Now let's assume that we have the operation `a := 1.5 + 3.2` in the code and we want to constant fold it down to `a := 4.7`.

When we do that at compile time, and we generate a code that contains the folded constant, we are actually taking a big risk, because the two architecture X and Y can be very different and can implement different ways to represent floating point numbers.

`4.7` in the first machine can be `4.6999` in the second, and this approximation can propagate down to other operation and lead to inconsistent behaviors.

Cross-Compilers (Compilers designed to work in similar scenarios) take these situations very seriously and use instead the string representation of the number, and then perform all "constant folding" that are possible directly on that string. Eventually, they will let the specific architecture to translate that string into a number.

### Eliminate Unreachable Basic Blocks

If there's a basic block that can never be reached by the control flow then we can just delete it from the program.

This will not make the code faster (because that code will never be executed!), but it will make the program smaller, which can in turn make it faster because of some cache effect in the machine.

Even though this can seem like a weird case, it's in fact quite common. Consider the snippet of C code

```
#define DEBUG 1
...
if (DEBUG) {
...
}
```

This sort of blocks is useful to have during the development of the program. Then, when we compile it for a release, we would change the definition to

```
#define DEBUG 0
```

The C-compiler would then automatically delete all if-blocks like that one, making the program smaller, and hence very likely more efficient.

### Single-Assignment Form

Many optimizations strategies are easier to apply if the code is in single-assignment form. This means that each register is assigned only once. For example, the following code is not in single-assignment form

```
x := z+y
a := x
x := 2*x
``

because the register `x` is assigned twice. However, the code can be written in the following, equivalen, form

```
b := z+y
a := b
x := b*2
```

That's in single-assignment form.

In particular, we can make two more assumptions that can potentially lead to many more optimizations. Given a basic block, if

1. It's in single assignment form, and
2. A definition `x := ...` is the first use of `x` in the block,

then when two assignments have the same RHS (right-hand side), they must compute the same value.

Looking at an example will show the real power of these assumptions. Consider the code

```
x := y + z
...
w := y + z
```

In the "..." part of the code, the register x cannot be assigned, because the block is in single-assignment form (assumption 1). Furthermore, the registers `y`, `z` cannot change either, because they are used to define `x` and that means they must have been defined before (assumption 2).

Therefore, we can rewrite the code as it follows:

```
x := y + z
...
w := x
```

This optimization is called **common sub-expression elimination**.

It's very important to understand the implication of the two assumptions we made about single-assignment form.

Consider that's very unlikely, if not impossible at all, that the original source code be in such a form. So, what's the point of assuming we have a code in that form?

The answer is, again, Intermediate Language. The Intermediate Language must be generated so that the two assumptions are fulfilled. This is always possible since we assume that the registers are infinite in the IL.

Another use of the single-assignment form is that everytime we see in a block the assignment `w := x`, then we can replace all subsequent usages of the register w with the register `x`. For example

```
b := z+y
a := b
x := a*2
```

Can be written as

```
b := z+y
a := b
x := b*2
```

This optimization technique is called **copy propagation**.

## The power of Local Optimization

Until now we have studied a few local optimization techniques that don't seem very powerful.

Indeed, each of these tricks will not change dramatically the efficiency of a program, when is considered standalone.

The real power of Local Optimization lies in the combination of all these techniques. As explained in a previous section, the main idea of the whole Optimization module is to apply tricks in an iterative fashion, until no more tricks can be applied.

The key point of such a strategy, is that **the successful application of one optimization tricks unblocks more tricks that couldn't be applied before**.

Thus, Local Optimization is like a waterfall that grows while it descends. At the beginning, a very few optimization techniques can be applied. But their application unblocks many more optimization opportunities that eventually lead to great performance improvement.

## Summary

In this article we started studying more in depth how Compilers optimize the source code.

More specifically, we went into the nuts and bolts of Local Optimization, that is a set of optimization techniques meant to improve performance of a basic block of code.

Even though most local optimization techniques are simple tricks, their real power lies in their iterative application, that can unlock a great deal of improvement in the overall code. Every time even a single trick is applied, it can unblock many others that weren't possible to apply before. This waterfall-effect produces great results.

At this point, is time to switch over to the next article, where we will be studying Optimization Part II.

## References

My article [Source Code Optimization in Compilers]({{< ref "/page/source-code-optimization-in-compilers.md" >}}).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).
