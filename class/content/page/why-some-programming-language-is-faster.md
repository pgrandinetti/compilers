---
title: "Why Are Some Programming Languages Faster?"
date: 2019-10-12
draft: false
---

Thanks to "Compilers" being the main keyword for this course, we also learned a great deal about Programming Language.

In a previous article I described what features can differentiate a programming language from the others. In particular:

- General Purpose vs Domain Specific Language.
- High vs Low Level Language.
- Interpreted vs Compiled Language.
- Procedural vs Functional vs Object Oriented Language.
- Typed vs Untyped Language.
- Statically vs Dynamically Typed Language.
- Static vs Dynamic Scope.
- Automatically Managed Memory (Garbage Collector) vs Manual Memory Management.

These are core features design of a language. The person (or the team) that works on that language development, and its compiler, has to make a choice for each of the above points, and even more.

As a result, there are a lot of Programming Languages out there. Each Language has its unique set of features, although, in all honesty, some languages are really similar.

A common question is **what's the fastest programming language**?

First of all, the question asked in this form makes little sense. For a number of reasons:

- The quality of your code is more important than the efficiency of the language you use, for 99.9% of applications.
- Actual speed of execution strongly depends on the hardware, and not just on the programming language.
- Even on the same hardware, the current CPUs and RAM are such that in a tiny percentage of cases you will experience a performance difference due to the programming language.
- There are many things that you can't know about how the Compiler modifies your code, to optimize the execution. I talked about them in a previous article.

In different words, much before thinking about the performance of the language you chose, and of its compiler, you should focus on different things, much more important:

1. Write the highest-quality code you can.
3. Use standard, well-known software pattern design. They improve readability and overall maintenance of the code, hence performance.
4. Choose well tested, robust, external libraries and packages if you need any.

That said, the main question still stands, and it's an interesting one from a theoretical point of view.

**If we could implement the same algorithm, on the same hardware, with all other conditions being the same, in different Programming Languages, which would be the fastest?**

To answer that, and in view of all other important points I mentioned, I want to discuss **the top-3 features that impact the efficiency**.

## Assembled vs Compiled vs Interpreted

In fact, there's little doubt. _Assembled code is the fastest_.

Speaking from a theoretical point of view, if you could always write your programs in Assembly code, and you could also always write the best Assembly code that's possible, then yes, your Assembly code would always be the fastest.

That's because Assembly code doesn't need to pass through a Compiler. It's directly assembled into CPU instructions. Blazing fast.

Needless to say, that's quite a lot of hypotheses.

First of all, most modern programs would be painful to write in Assembly.

Secondly, very few programmers are trained to write efficient Assembly code. This is a direct consequence of the previous point. Therefore, it's highly possible that a good code, say in C, compiled with a good Compiler becomes more efficient than a poorly written Assembly code.

In other words, if you are not an Assembly developer, then a good Compiler can written better Assembly code than you, even if it's just a software!

If you don't want to write Assembly code, then again, one can make an educated statement: **Compiled code is the second fastest** (among the three types we are discussing).

Briefly explained, the reason is that Compilers produce so-called "target code", that's basically machine code. On the other hand, Interpreters add one more layer between the execution environment for your program, and the Operating System, and that slows things down a bit.

## Abstraction

Why is C that fast?

As a programming language, C doesn't offer you anything special.

- There are no class and inheritance.
- There is no dynamic typing.
- Arrays boundaries are not checked (you don't know the length of an array a-priori).
- There's nothing cleaning up memory after you (you must do it manually).

In other words, C code is just a little bit more abstract than Assembly code. This allows a very good C developer to write highly efficient code.

On the other hand, C code is not really safe. Every time you perform an operation on the computer memory, for example using memcpy, it's not even checked what was before in that memory locations, and if there's any overlap. Trust the programmer is C's spirit.

Other languages provide more abstraction and more security.

To start with, all OOP Languages allow us to define complex objects and new types, along with the operation that one can use on them.

Then, things like the Java Virtual Machine (or Python's interpreter) make the program execution overall much safer. But, they do spend some time to perform security checks, during the execution itself.

It must be said that Just-In-Time compilers exist for many languages, and they give great performance improvements. But that's an entirely different topic.

## Tie with the Application

If you work on an application and could write for it a C program that takes 1 week to write and runs in 0.1 seconds, or a Python program that takes 1 day to write, and runs in 5 seconds, which option would you choose?

I guess it really depends on the actual application.

If the program is meant to run once a day, then maybe it doesn't matter how fast it is.

But if you must run it 100.000+ times a day, on several machines, then the CPU-time on these machines would be very different if you sue C or if you use Python. And so would be the total power consumption of your computers, which means money. In this case, spending one week writing good C code can save you lots of money.

The point I am making here is that execution speed is often a relative concept. Other factors play an important role, such as how much speed do you really need, what's the benefit of some speed improvement, as well as the trade-off between execution speed and development speed.

## But so, Which Programming Language is the Fastest?!

I purposefully avoided to give a firm answer on this question in the previous part of this article, because I think this is not the correct design question to ask when building something.

That said, if you really can't resist, just know that C is usually taken as reference point.

That means that performances of new languages are evaluated by comparing execution of standard algorithms with the runtime of the same algorithms implemented in C.

According to many sources (that you can google yourself) C, Rust, C++ and Fortran are always at the top of the many ratings for the fastest programming language.

As additional study, I warmly recommend to check you the link in the references to the page Bad Code Can Be Written In Any Language.

To conclude. _Which programming language is fastest? Should we care? How could we know?_ (from [The Benchmarks Game](https://benchmarksgame-team.pages.debian.net/benchmarksgame/which-programs-are-fastest.html)).

## References

My article [What are the differences between programming languages?]({{< ref "/page/what-are-the-differences-in-programming-languages.md" >}}).
