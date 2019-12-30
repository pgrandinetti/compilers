---
title: "How Compilers Optimize the Source Code (Part II)"
date: 2019-10-13
draft: false
---

In Part I of this class we learned a great deal about how Compilers do Local Optimization.

Local Optimization is the process of improving the performance of each basic block of code, isolated from the rest of the code. A basic block is a subset of the program that doesn't contain label, except possibly in the first instruction, and doesn't contain jump, except possibly in the last instruction.

In Part II of the class on How Compilers Optimize the Source code, we will be studying Global Optimization and Data-Flow Analysis. These subjects will complete the picture about Optimization.

## What is Global Optimization?

Let me start with a clarification. Despite its name, Global Optimization refers to optimization techniques applied to a control-flow graph.

A control-flow graph is a formal description of a whole function, or method, in the program. More about this in one minute.

Thus, despite the adjective "global", this type of optimization doesn't deal with optimization techniques for the program as a whole. Those are called instead Inter-Procedural Optimization techniques, and are quite rare even in strong Compilers.

Why are they rare? Because Compilers tend to perform Optimization following a **tradeoff between complexity (cost) and benefit**.

For instance, Local Optimization gives the best ratio cost/benefit, because most of the techniques are quite simple to apply and, when applied iteratively, they give great improvement. We saw all such concepts in the last article.

On the contrary, Inter-Procedural Optimization is extremely complex, and it's not quite clear in advance how much benefit it can give. This is why the fanciest optimization techniques that are published in research papers are rarely put into practice.

Global Optimization stands somewhere in the middle. It's definitely more costly than Local Optimization, but it can give more benefits too. And it's surely less costly than Inter-Procedural techniques. This is why **Global Optimization is implemented by many strong Compilers**.

## Data-Flow Analysis

Formally speaking, **Data-Flow analysis is the study of how a given property propagates through the Control-Flow graph**.

An example will help clarify the concept.

Let's consider a procedure made of four basic blocks.

The first block is always executed, and is made of two instructions.

```
x := 3
b > 0
```

The second block is executed on the conditional jump `b > 0` (on register `b`), and has two instructions as well:

```
y := z+w
x := 4
```

The third block is executed on a jump that is mutually exclusive with the condition related to the second block, and it has only one instruction:

```
y := 0
```

The fourth and final block has one instruction and it's always executed.

```
a := 2*x
```

The key detail that the optimizer should note is that if the second block didn't contain the assignment to register `x`, then we could propagate the value `3` assigned to `x` in the first block straight down to the fourth and last block. This would give a nice performance improvement.

However, the assignment `x := 4` is in the second block, so we can't ignore it.

Nonetheless, there's a second case when that assignment would be irrelevant. It happens when the second block is never entered, due to the conditional jump. In this case again, we could propagate the value `3` for the register `x`.

Thus, the key question is - **How to be sure that is OK to propagate a constant value for a register**?

This kind of reasoning, which is also an optimization technique, is called global constant propagation.

Constant Propagation is an example of a "property" that we want to know whether it propagates through the entire control-flow graph, that means through the entire procedure. And, as I said, this is what Data-Flow Analysis studies.

To clarify even further, the "property" in the example is the equality `x := 3`. We would like to prove that this property holds true everywhere in the control-flow graph, so that we can optimize the code in various places.

Of course, constant propagation is not the only goal of Global Optimization. However, most Global Optimization techniques share some common traits:

- The optimization is based on knowing that some property X holds true at a particular point in the program's execution.
- Proving the property X holds true everywhere else down in the procedure requires to know the entire program, or at least the whole Control-Flow Graph for that procedure.
- For many "questions" (that is, properties to prove of disprove), it is OK to be conservative. It's better to say "don't know" and to give up with that optimization path, rather than to make a mistake.

With that clarified, let's continue studying global constant propagation, that is not only a very common technique, but also a good learning tool.

## Global Constant Propagation

Let's start with a picture of the Control-Flow Graph for the previous example.

```
       X := 3
       B > 0
      /     \
     /       \
 Y := Z+W    Y := 0
 X := 4       /
    \        /
     \      /
     A := 2*X
```

As earlier said, the question we want to answer is - **How to determine if a given property holds true for a single variable X, at all points in the program?**

In the example we considered the equality `x := 3` as the "property" to prove (or disprove).

Let's start with a formal, as well as easy to understand, statement.

_To replace a use of `x` by a constant `k`, it must hold true that on every path to the use of `x` the last assignment to `x` is `x := k`._

The above sentence can be read in the following way:

1. Look at all points in the program where x is used.
2. If we can prove that `x := k` was the last assignment to `x` in every sequence of instructions that lead to those points,
3. Then the property `x := k` holds true in the control-flow graph.

Unfortunately, step 2 is quite difficult to prove. We need a **conceptual framework around the problem**, in order to clarify it.

Let's start with some definitions. Define three values that the variable X can have at any point in the program.

- ┴ (read: "bottom"), when the statement at that point in the program will never execute.
- `C` (read: "constant"), when we believe or we have proven that `x` is a constant.
- T (read: "top"), when we don't know whether `x` is a constant or not. This is the safe condition.

Now let's look again at the control-flow graph, and assign these values **in-between each pair of statements**.

1. At the beginning, we don't know what values `x` has, so we say `x` is top.
2. After the assignment `x := 3` we know for sure that `C` is the value to assign.
3. After the branch we definitely know that `x` is still `C`.
4. In the left branch, after the assignment to `y` we know that `x` is still `C`.
5. In the right branch, after the assignment to `y` we know `x` is still `3`, so it's `C`.
6. After the assignment `x := 4`, `x` is `C` but with value `4`.
7. Before the final block, `x` can be either 3 or 4, depending on which block was executed. So we must say that `x` is Top.
8. After the assignment `a := 2*x`, we are still unsure about `x` so we must say that `x` is Top.

It's really important to notice that the assignments to Top, C or Bottom happen **in between instructions**. We must determine the value for `x` before and after each instructions, including the first and the last ones.

Once we have the global constant information, it's very easy to perform the optimization. We just have to inspect the value (top, bottom, C) associated with a statement that uses `x`. If `x` is a constant, then we can replace it with that value.

The crucial question is **how to compute the property Top/C/Bottom in a systematic way**? In the example above we did it manually, but of course this is not a sensible approach, especially for large procedures.

Here's where Data-Flow Analysis enters the game.

The most effective way, for me, to understand Data-Flow Analysis algorithms is to try to come up with a solution before looking at the actual solution.

Let's put what we already know in writings:

1. We want to know whether a given condition (or property) about the value of a register holds true at a given point in the program. For instance, at line 5, is `x` equal to 3?
2. Given that we don't have any oracle, the only pieces of information we can use are the instructions themselves. Each instruction changes something in the state of the program. For example, the assignment `x := 3` means that before and after every subsequent instruction `x` is equal to 3, until a different assignment to `x` is found.
3. We need a systematic way to propagate information down through the control-flow graph. Each piece of information must also be "merged" with others, according to the current state of the program.

The third point above is the key, conceptually, that unlock the path towards the general strategy. **We must devise rules that dictate how pieces of information are merged to produce new knowledge**.

A concrete case, to help understanding. When we find the statement `x := 3` we start propagating this information down. If then we find the statement `x := 0`, we must merge the two information and create a new one. In this (simple) case we will start propagating the new information that `x` is equal to zero.

If we manage to build a set of rules that handle every possible case, then we will have a systematic (automatic) way to propagate information in the control-flow graph. As a side objective, we would like to have rules that are quite simple.

Luckily for us, someone has already devised such a set of rules. Let's study them!

First, a couple of definitions.

For each statement `s`, we compute information about the variable `x` before and after `s` executes, with the following function:

- `C(x, s, in)` ⇒ value of `x` before `s`
- `C(x, s, out)` ⇒ value of `x` after `s`

Also, let's agree on calling `p1, ..., pn` all predecessors of a statement `s`. There can be more than one if `s` comes after a branch in the graph.

Now we are ready to take at look at the systematic rules. There are 8 of them.

### Rule No 1

_If Exists i such that C(x, pi, out) = Top, for i=1,...,n, then C(x, s, in) = Top_

This rule just says that if we don't know the value of `x` in at least one of the paths that lead to `s`, then we don't know the value of `x` before `s` executes.

Remember that "Top" is the safety value, for when we aren't sure about the actual value.

### Rule No 2

_If exist i, j such that C(x, pi, out) = c1, C(x, pj, out) = c2, and c1 != c2, then C(x, s, in) = Top_

Basically, this rule says that if two branches that lead to the same statement `s` assign to `x` different values, then we cannot be sure of the value `x` has before executing `s`, so we must say it's Top.

### Rule No 3

_If C(x, pi, out) = c or C(x, pi, out) = bottom, for every i=1,...,n, then C(x, s, in) = c_

This rule says that if all branches that lead to `s` assign to `x` the same value, then that exact value has to be `x`'s value before `s` executes. In case one or more of them it's bottom, this means that branch is never reached and so it doesn't change the evaluation.

### Rule No 4

_If C(x, pi, out) = bottom, for every i=1,...,n, then C(x, s, in) = Bottom_

Rule 4 says that if all branches leading to statement `s` carry over the value Bottom, then also `s` will continue carrying over the same value.

The first 4 rules relates the output of a statement to the input of the next statement. We also need rules to relate the input of a statement to the output of the same statement. This is what the next 4 rules are about.

### Rule No 5

_If C(x, s, in) = bottom, then C(x, s, out) = Bottom_

Rule 5 is quite clear: if statement `s` is never reached, and hence the value before its execution is Bottom, then after its execution we should consider the same value.

### Rule No 6

The next rule refers to a statement of the form `x := c`, that is, an assignment.

_C(x := c, s, out) = c, if c is a constant_

It's worth noting that Rule 5 has higher priority than Rule 6. So if the "in" of a statement is Bottom, we will say that the "out" is bottom too, applying Rule 5, regardless of whether Rule 6 could be applied or not.

### Rule No 7

Rule 7 complements Rule 6, by defining the behavior for every assignment that is not in the simple form `x := c`, but a more complex `x := f(...)`.

_C(x := f(...), s, out) = Top_

### Rule No 8

Finally, a rule that deals with assignment to a different variable altogether.

_If x != y, then C(y=..., s, in) = C(y=..., s, out)_

This rule basically says - if we don't touch `x` at all in the statement, then propagate whichever value we had before.

### Global Constant Propagation Algorithm

Now that we know the 8 rules, we are ready to design the systematic way to propagate information in the control-flow graph: an algorithm.

```
1. For every entry statement to the program s, set C(x, s, in) = Top.
2. Set C(x, s, in) = C(x, s, out) = Bottom, everywhere else.
3. Repeat:
  i. Find statement s that does not satisfy at least one among Rules 1-8.
  ii. If didn't find any such statement, STOP.
  iii. Apply one of the Rules 1-8 to s.
```

After we are done executing this algorithm, we'll end up with correct values (Top, c, or Bottom) before and after each statement of the program, and then we can make the correct reasoning about it.

## Loop Analysis

This topic is probably the most interesting within Data-Flow Analysis. Let's first get a sense of what the problem is with an informal description.

First of all, we are still working towards the same objective: **we want to decide what's the value of `x` before a statement**.

To do so, as we saw, we need to know the value of `x` after all the predecessors of such a statement. Let's assume that one of such predecessors is actually deeper down in the control-flow graph, because there's a loop in the program. This is a fairly common case, if you think about it.

In this situation, it's not obvious how to determine the value of `x` after that one predecessor, because to compute it we would need to know its predecessors too, but at least one of them depends on the same statement that we are considering, due to the loop in the code!

Now, let's look at some real code:

```
x := 3
if b > 0 jump R

L:
y := z + w
jump end_if

R:
y := 0
jump end_if

end_if:
a = 2*x
if a > b jump R
// else exit
```

The control-flow graph for this procedure has `a` loop on the right side, from statement `a > b` back to `y := 0`.

Concretely, in order to know whether `x` is constant before the statement `y := 0`, we need to know the value of `x` at his two predecessors:

- `x := 3`
- `a := 2*x`

But the information for the variable depends on the predecessors of that statement, including `y := 0` itself! The situation is such that, to answer the question, we must "unwire" this odd **recursive enigma**.

There is **an elegant solution** to this problem. In fact, it's a standard technique applied in many areas of mathematics.

It consists of **assigning an initial value (a guess) to the variables**. It's fine if such value is not expected to be the final and correct value. This is what the initial assignment as Bottom is for, in the algorithm given in the previous section.

After that initial assignment, the iterative algorithm will make all loops "converge" to the real solution, when there are no more updates that can be done.

The concept of "convergence", in this case, means that at some point during the iterative procedure all the variables will stand on the same value: they will not change anymore. And this stable configuration will also be the final solution, as well as the final state of the algorithm.

Before continuing, it's worth making one point clear. **The constant propagation algorithm is already designed to take into account loops, thanks to the initialization to Bottom of all unknown values**.

On the other hand, the soundness of the algorithm is guaranteed by the convergence of the same.

I understand that the "convergence" concept seems a bit like a magic black box. I want to study it more in detail, and that's the subject of the next (and conclusive) section.

## Orderings

The values we've talked so far, Top, Constants, Bottom, are "abstract" values. They are abstract because each of them can be one of many "concrete" values that are actually in the machine's memory.

We can dictate an ordering over there three values, as it follows:

$$
\text{Bottom} \ge \text{Constants} \ge \text{Top}
$$

It's important to note that there's no ordering between two constants. Basically, comparing 1 and 2 in this ordering gives an indefinite result, and we must fallback on Top as the "safety value".

Quite clearly, this is a choice that we make. It could have been a different ordering, but we are about to see why this one was purposefully chosen.

It's worth adding one more definition. Let's talk about the **Least Upper Bound** (lub) in this ordering, as the "biggest" among two argument.

For example:

- $lub(Bottom, 1) = 1$, because every constant is "bigger" than Bottom in the ordering.
- $lub(Bottom, Top) = Top$.
- $lub(1, 2) = Top$, as said above.

The great thing about this definition is that we can rewrite Rules 1 to 4 using this new definition. For example Rule 3 is equivalent to the following one:

$$
C(x, s, in) = lub\\{C(x, p, out),\, \text{such that p is a predecessor of s}\\}
$$

And the good things are not over! In fact, the existence of **this ordering also proves the convergence of the algorithm**! Let's see why.

If we look carefully at all rules, then we see that the function $C(x, s, **)$ can only "increase" the propagated value: it starts at the value Bottom and can increase to Constant, and then can increase to Top.

This means that each piece of information can change at most twice. And therefore the algorithm will always terminate.

And by continuing the same reasoning, we can also see that **the complexity of the constant propagation algorithm is linear with the number of values that we want to compute**.

This is another important (good) property to keep in mind, about constant propagation.

## Other Topics

Global Optimization is a very active research field. Scientists and engineers from all over the world work hard to find new optimization techniques, while trying to make the most out of the tradeoff between cost and benefit.

On the one hand, the subjects we have studied (Local Optimization, in the previous article, and Data-Flow Analysis, Global Constant Propagation, Loop Analysis and Orderings, in this article) are only the surface of everything there is to learn about Optimization in Compilers.

On the other hand, in my opinion, it would be pointless to go through a lot of optimization techniques here. The student who's really interested in taking this subject to the next level should definitely go ahead and start reading research papers.

For those among us that are really interested in other topics, I want to suggest a sensible starting point.

Let's think about the consequences of applying Global Constant Propagation. Once we've done propagating the constant, it's quite possible that some statement in the program become useless. For example,

```
x := y + z
```

Can be removed if constant propagation proves that `y := x` and `z := 0` at that point in the program. Removing a whole statement is one of the most beneficial optimization steps you can do, and therefore is very important.

Thus, the question is - **how can we assess whether a statement can be removed or not?**

This topic is called **Liveness Analysis**, and it's also addressed with a Data-Flow Analysis, not too different from the one we studied for Constant Propagation. It's definitely a good starting point for you next step!

## Summary and Outlook

Optimization in Compilers design is a fascinating subject!

It's also very complex, so much that the most advanced optimization techniques are too expensive in terms of implementation and do not give benefits proportionally to their complexity.

From a higher perspective, Optimization is the second to last step in the implementation of a Compiler. It should come right before Code Generation, although, as we've seen, most optimization techniques are applied on an Intermediate Language.

So, the most likely sequence is that after Semantic Analysis (the last front-end step), there's a first Code Generation step, that generates code in the Intermediate Language, then Optimization kicks in. And after that, finally, the target code (in machine language) is generated in a further Code Generation step.

## References

My article [Source Code Optimization in Compilers]({{< ref "/page/source-code-optimization-in-compilers.md" >}}).

My article [How do Compilers Optimize the Source Code (Part I)]({{< ref "/page/how-compilers-optimize-code-1.md" >}}).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).
