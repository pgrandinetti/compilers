---
title: "Source Code Optimization in Compilers"
date: 2019-10-12
draft: false
---

The strongest Compilers out there do not limit their work to direct compilation of the source code.

**They also modify the original source code in order to optimize performances.**

The first concepts we must study are:

- **What** they optimize.
- **When** they optimize, that is, at which point in the compilation process.
- **Where** they optimize, that is, what parts of the code.

## What do Compilers Optimize?

Software applications vary over a very broad range. Web, Desktop, Database, Embedded Systems are just a few examples.

Thus, it's no surprise that **there exist Compilers to optimize different metrics**.

Here's a few common metrics a Compiler may want to optimize:

- **Runtime**. This is probably the most common case, when a Compiler wants to minimize the number of execution steps of a program.
- **Disk access**. When a Compiler wants to minimize the number of accesses to disk made by a program.
- **Network messages**. When the number and size of packets sent through the network must be minimized.
- **Power consumption**. When the battery of the machine must be preserved as long as possible.
- **Size of the compiled program**. When the size (in kilobytes or megabytes) of the final program is important, and must be minimized.

The most intuitive metric is surely the first one. And indeed is also the one commonly implemented in the Compilers that are the most used out there.

## When do Compilers Optimize?

We have studied the different steps of a Compilers in great depth before. They are, from the first to the last:

1. Lexical Analysis.
2. Parsing.
3. Semantic Analysis.
4. Optimization.
5. Code Generation.

Thus, the Optimization module can work on the result of any of the other steps.

**The first option is to optimize the parse-tree**, that is the result of the Parsing step.

This approach has two consequences:

- Advantage. The parse-tree is an algorithmic abstraction. Therefore it's a very general approach, not tied to any specific machine architecture.
- Disadvantage. For the same reason, the parse-tree is sometimes too generic. There are not many optimization tricks that can be applied to it.

**The second option is to optimize the Assembly code**, that is the result of the Code Generation step. No need to say, this is the deepest optimization level.

- Advantage. At such a low level, you can potentially optimize every single bit of code!
- Disadvantage. Assembly code strongly depends on the machine architecture, so we'd have to reimplement the optimization module for each architecture.

I will speak in a moment about the trade-offs between complexity and gainings in Source Code Optimization, but for the moment it suffices to say that the disadvantage of performing optimization on Assembly code is such that nobody really does it.

**The third option is to optimize an Intermediate Code**. Let's make a small digression to see what Intermediate Code is.

### Intermediate Code

The concept of **intermediate code**, also known as **intermediate language**, simply refers to a language that is in between the source code and the target (final) language.

For example, the source language can be C and the target can be some mixed Assembly (Intel, MIPS, etc.).

The real question is - **why would one need an Intermediate Language?**

The reason is that the Intermediate Language (IL) creates yet one more level of abstraction, and practice has shown that it turns out to be a good thing!

In particular, the IL has:

- Many more details than the original source code.
- Some detail less than machine (Assembly) code.

For example, if the source is in Python then we have no way to express something about the machine's register, and their allocation. But before going down to Assembly we might want to express this specific feature of the program in some intermediate language.

To get a more precise idea of what an Intermediate Language is, let's create one.

Our Intermediate Language will have the following features:

- It can allocate registers, but assumes there's an infinite number of them.
- It has control structures like Assembly.
- It has opcodes (`add`, `mul`, etc.) like Assembly, plus some more higher level, such as push that translates into more than one Assembly instruction.

Furthermore, in this IL, every operation must have one of two forms:

```
x := y op z, or
x := op y
```

where `y`, `z` must be either constants or register names.

This Intermediate Language is called **three-address code**, because every instruction has at most 3 operands. In fact, it's a well known form of IL, and widely used too.

As an example, consider the operation

```
x + y * z
```

How would that be translated in the IL?

```
t1 := y * z
t2 := x + t1
```

Remember that we can create as many temporary registers as we like (`t1`, `t2`, …). This is the most evident approximation respect to Assembly.

The important thing to know is that to generate intermediate code is not conceptually different from generating runnable code the way we already studied.

Thus, if you decide to use IL in your Compiler the sequence of steps would become:

1. Lexical Analysis.
2. Parsing.
3. Semantic Analysis.
4. Intermediate Code Generation (from the Parse-Tree).
5. Intermediate Code optimization.
6. Runnable code generation (from the Intermediate Code).

Thus, it's easy to understand that an additional, nice to have feature of an Intermediate Language is that it's easy to translate in Assembly.

Now, let's get back to Optimization.

## Where do Compilers Optimize?

Since Source Code Optimization is literally about modifying some parts of the source code, a natural question arises - can we do that no matter where in the original code?

One possibility is **to optimize basic blocks of code**.

A basic block of code is a sequence of instructions without any label, except possibly for the first line, and without any jump, except possibly for the last line.

The idea is that a basic block is a **piece of code whose execution is completely predictable**. There's no entry point for other parts of the code (except for the first line, maybe), and there's no exit point, except maybe for a final jump. Therefore, we know what's going to happen there, and we can optimize its behavior.

Another option is **to optimize control-flow graphs**.

A control-flow graph is a directed graph that represents the execution of a single function. In such a graph:

- Nodes are basic blocks.
- There's an edge between two nodes (basic blocks) A and B, if the execution can pass from the last execution of A to the first execution of B.

Another key point here is that the body of a method (or a procedure) can always be represented as a control-flow graph, where the “return” nodes are terminal. That's why talking about control-flow graphs optimization makes sense.

The third possibility is **to optimize procedures execution**, taking into account that the execution can jump back and forth into each other. Clearly this is the most complex case.

In summary, there are three alternatives:

1. **Local Optimization**. Applied to each basic block in isolation.
2. **Global Optimization**. Applied to a control-flow graph (a whole function/method) in isolation.
3. **Inter-procedural Optimization**. Applied across function boundaries.

It's worth noting that most compilers do (1). Many do (2). But just a few do (3). The reason is not only that (1) is the least difficult to implement, but also because it's where the gain is the highest.

This means that there's often a conscious choice of not implementing fancy optimization techniques that are known in literature. Why?

- Some optimizations are complex to implement.
- Some are costly in compilation time.
- Some have low payoff.
- The fanciest techniques have all three problems!

This points out what is the real goal of optimization: **Maximum benefit for minimum cost**. This is the tread-off between benefit and costs.

## What's next

In this short article I simply wanted to give an overview of what optimization means in the context of Compilers. It's actually the most involved step of all, in today's Compilers, and also the most active from research point of view.

In one upcoming article in this course I will discuss more in detail a few techniques to perform optimization in the three cases we saw above.

## References

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).
