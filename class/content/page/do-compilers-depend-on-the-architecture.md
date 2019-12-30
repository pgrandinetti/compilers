---
title: "Do Compilers Depend on the Computer Architecture?"
date: 2019-10-13
draft: false
---

In all previous articles in this course, I tried to emphasize equally all steps in the compilation:

1. Lexical Analysis
2. Parsing
3. Semantic Analysis
4. Optimization
5. Code Generation

For some reason, though, I noticed that the first three steps receive a lot of attention, usually much more than the last two.

Moreover, the first three steps form what’s usually called front-end compilation, as opposed to the back-end compilation that if formed by the last two.

The goal of the front-end is to make sure the source code is well-written, as well as to produce some data structure that will be useful for the back-end.

The key point I want to make is that front-end is indeed an abstract process. It’s about algorithms, formal grammars, regular expressions and finite automata. In other words, **front-end is independent from the computer architecture**.

However, front-end is just one part of the Compiler. The back-end strongly depends on the computer architecture.

Therefore, **it’s correct to say that Compilers do depend on computers architecture**.

Let’s get now deeper into the why.

## Procedures and Activations

Let’s start with a fair approximation: **a Program is made of Procedures**.

We are used to thinking about Procedures as pieces of reusable code (that is, functions). That’s true at all levels of coding, including machine-level such as Assembly code.

In Assembly code, for example, we can think of procedure like pieces of code delimited by the labels and the jump instructions. And if there isn’t any such block, then we can think of the whole program as a procedure.

In either case, the activation of a procedure needs to be handled with care by the executing environment.

Let’s think about the trickiest points with procedures:

- Input data and variables names must be confined to that procedure’s execution (this happens in all languages of practical relevance).
- There needs to be a link to the caller, that’s the piece of code that invoked the procedure.
- Storing the return value.
- The return address, that’s the line of code, in the whole program, that must be executed after the procedure has finished.

To complicate things further, consider that even though the various blocks are logically separated, the entire storage is really just one gigantic array in memory. And the fact that elements in the array are contiguous plays an important role.

All considered, we should imagine the activation record of a procedure like a block of memory that stores the information needed to correctly execute that procedure within the program.

## Enters Compiler

Why and how does a Compiler gets to play with the activation record?

The answer is relatively easy: **the activation record is designed at compile time**.

It couldn’t be in a different way, because it’s absolutely necessary to know each word of memory within the record, in order to correctly execute the program at a later stage.

In other words, the compiler must be able to determine the layout of the activation records, and generate code that can access any sub-block in any of such records. This means, **Activation Record layout and Code Generation must be designed together**.

Think about in this way. If the first word of memory in the activation record is reserved for the return value of the procedure, then the Compiler must know it, because it’s a key information for executing the program.

And what’s the only way to know that with absolute certainty? Well, the Compiler must be the designer of the activation record.

## Summary

To summarize the content of this article, remember: **a well-designed Compiler knows, depends on and takes advantage of the underlying computer’s architecture**.

This is evident in particular in the stage called Runtime Organization.

There, the Compiler literally design how to organize the memory (and sometimes the storage too) of the machine, in order to build the best execution environment for the program.

Activation records for procedures, also called activation frames, are the key elements that allow the correct management of procedures within the program. They are meant to store information such as the return address and value, arguments to the procedures itself and a link to the caller. Furthermore, **the Compiler is in charge to design the layout of the activation frames**.

Finally, it’s worth mentioning that these are fairly advanced subjects. Strong Compilers out there definitely take them into account and do an excellent job with it. But if you want to experiment and build a smaller Compiler, then you can take these concepts a bit less strictly.

## References

My article [How does the Computer Run Code?]({{< ref "/page/how-computer-runs-code.md" >}}).
