---
title: "What Are the Differences Between Programming Languages"
date: 2019-09-29
draft: false
---

Whether you want to design a new programming language, or you want to study Compilers theory, or you just want to learn a new language, the first and perhaps most important thing to do is to **understand the whole picture**.

Programming Languages are very complex systems, where **many features interact** and provide a (hopefully) consistent result. This why we use many words to describe a single language. For example, Java is a general-purpose, high-level, statically typed, object-oriented, compiled to bytecode, programming language.

Each of those definitions specify a particular feature of Java's. These are theoretical concepts, well defined in books about languages theory. Then, every language is basically a different implementation of several among these concepts, to provide an original result.

To understand the whole picture means to understand what are the main features of a language. What does it mean that Java is object-oriented? What about the fact that is general-purpose?

Here's the list of the concepts we will master:

1. General-purpose vs Domain-Specific.
2. High vs Low level.
3. Interpreted vs Compiled.
4. Procedural vs Functional vs Object-Oriented.
5. Typed vs Untyped.
6. Statically vs Dynamically typed.
7. Automatic Memory management vs Manual management.
8. Static vs Dynamic Scope.

## General-purpose vs Domain-Specific Languages

This distinction is quite easy to understand.

Some programming languages are designed to build software that can solve any problem and perform any task. This is why they are called general-purpose.

Other languages are tailored to some specific application domain. They are therefore called domain-specific.

An example of a general-purpose language is C++. You can build pretty much everything in C++, but in some cases it might cost you a lot of effort, even for simple problems.

An example of a domain-specific language is the code you would write in a configuration file, where on each line you write `variable = value`. You can't build anything different from what you need for this application, with such a language. On the other hand, it does solves this specific problem in a very easy way, whereas it may require you to put in more effort using, say, C++.

## High vs Low Level Languages

This distinction is related to the way the code you write interacts with the underlying computer architecture.

The first thing you need to know is that, eventually, every program you write is somehow executed by the computer in its architecture: data are saved in the available memory, computing instructions are executed in the CPU, etc. Thus, at the end of the day, **executable code is always low-level**.

Now, the point is - who interacts closely with the architecture? It is you, with your code? Or is the Compiler that makes something magic for you?

In the first case, it means you are writing code in a low-level language. For example, you might be using Assembly language, where you specify what to do with each CPU's register and each slot of memory.

This is why Assembly programs, as well as programs written in other low-level languages, are usually very long, in number of lines. You have to explicitly state every micro-operation you want to perform. For example:

```
load 1 r1;  // save the integer number 1 into register r1
load 2 r2; // save the integer number 2 into register r2
add r1 r2 r3; // add the content of registers r1 and r2 and put it into register r3.
```

Keep in mind that, even though Assembly is considered low level, you can even go at a lower level. That's when you set **each bit of memory** into the machine.

In the second case, instead, it means you are writing code in a high-level language. For instance, C is one among such languages.

What happens when we write code in a high-level language? There's some part of the language, usually called either Compiler or Interpreter, that takes care of "translating" our high-level instructions into low-level code.

A single line of C code, such as 

```
int x = 1;
```

is translated into many low-level instructions by the C Compiler. This is why they are called high-level instructions. Then, the low-level instructions are executed into the computer's architecture.

## Interpreted vs Compiled Languages

First, an important disclaimer. The distinction between interpreted and compiled languages was very clear years ago. Today, **these two features are mixed in a lot of languages**.

Second, an important clarification. Using the terms interpreted and compiled next to a language name is actually a misuse of them. Python, the language, is not interpreted. It's not, because it's just a language. Same for Java. Languages are just definitions of words and symbols, along with their semantic.

The correct way would be to say "this Java's implementation compiles the code, whereas that one interprets it". This is just a nuance, but I think it's **an important concept to remember**.

Programs in almost every language can be either compiled or interpreted, it's your choice. Furthermore, many languages do a combination of interpreting and compiling the source code.

That said, let's continue the discussion by using the terms interpreted and compiled directly on languages, and we should refer in such cases to "the most common implementation of that language".

Let's see what the word _interpreter_ and _compiler_ means.

First, and foremost:

  * Given a source code and some input data, **an interpreter produces a result**.
  * Given a source code (no data), **a compiler produces another source code**, one that is executable.

If you give your source code and some initial data to an interpreter for that language, it will go through your code line by line, executing each line.

If you give your source code to a compiler for that language, it will go through many steps that will eventually produce an executable code. This is a file that no human is able to read, because it contains instructions at machine-level (the lowest level). You will then have to run this executable, optionally providing some input data, to get the result.

Some say that interpreted languages have advantages:

 * It's faster to prototype using them. But I disagree, or, to better say it, capability to prototype faster is not about interpreting or compiling, in my opinion.
 * They are portable. I disagree again: for compiling you need to have the compiler in the machine; to interpret you need to have the interpreter.
 * They allow dynamic scoping. I disagree! I mean, yes they do, but dynamic scoping is not something to be proud of! More on this later.
 * They allow dynamic typing. Yes, that one I will concede.

Obviously, others say compiled languages have advantages:

 * They are more robust, because they perform static-type checking (we will study it in future articles).
 * They are faster in execution.
 * Source code is protected (because the executable is not really readable).

At any rate, you don't want to get involved in a discussion about which type of language is better. Furthermore, as I said, *many modern languages are both interpreted and compiled*.

For example, let's talk about Java. Java code really gets a long way before it's eventually executed.

1. **First, the source code is compiled** by a program called javac (Java Compiler), and another source code is produced. The latter is in a language called bytecode.
2. Then, another program called java is started. This creates a virtual environment, called Java Virtual Machine.
3. Now, depending on the specific JVM, **the bytecode can be either interpreted or compiled**:

    - If interpreted, then the Java Interpreter takes the bytecode and runs it.
    - If compiled, then a Just-In-Time Java Compiler takes the bytecode and produces native machine-level code.

The compilation down to bytecode is a very common step for a lot of languages that are said to be "interpreted". For example, Python, that is known to most people as an interpreted language, is actually first compiled to bytecode. Only then, the bytecode is interpreted.

**Why is compilation to bytecode done?** The reason is that interpreting directly the high-level language and executing it into the machine would be really, really inefficient. Bytecode is an intermediate language, much lower level, that therefore can provide improvements in terms of efficiency.

## Procedural vs Functional vs Object-Oriented Languages

First of all, what are such names? They are **programming paradigms**.

A programming paradigm is a concept, and idea that shapes the way you write a code, and even the way an entire programming language is designed.

For example, **Python is a Object-Oriented language**. This means that every thing, inside a Python program, is an "object".

So, what is an object? The standard way to identify objects is as **instances of a Class**, where the word "class" is a construct of the Grammar of a given language.

Let's make all I said more concrete.

In Python, the reserved keyword class is used to create (or refer to) a Class, in the semantic sense. When you write

```
class A:
    # fields and methods here…

a = A() # new object of type A

```

the semantic of the code you wrote is to _logically define a type of objects, said A, and then to create a single instance of such type_.

I will discuss in a minute what implication all that has on actual code writing. Let me first briefly introduce the other two paradigms.

Procedural programming, like the name says, it's when the **code is organized in procedures**. Procedures is the standard block of code that we often (improperly) call function. Let's stick with the word procedures, to avoid confusion with the functional paradigm.

The classical example of procedural language is C, where you organize all your code in procedures, routines, structures, etc.

Finally, functional programming, to put it simply, it's **the counterpart of procedural programming**. The name comes from the mathematical world, where a function is a computable object that will always return the same value, if the same input is provided. 

Furthermore, **functions have no side effects**. No database read/write, no file systems write, no changes to the global state.

You may wonder - how can I possibly write a meaningful program with such constraints. Well, you need to accept some compromise. Very good examples of functional languages are Scala and Haskell.

It's my firm opinion that **none of these paradigms is superior to the others**. Good software can be written with each paradigm, and it's more about your knowledge of the language and you skills as a software engineer.

That said, I also think that depending on the problem you want to solve, and even more on your personal style (yes, each programmer has their own style!) one paradigm can work better than the others, in the given situation.

For example, if you want a complete separation between data and algorithms in your code, then adopting a procedural style might be the right approach. By their own nature, procedures isolate the logic from the data.

If instead you like to think about your software as a group of logical entities, each with its own set of data and internal logic, then OOP comes immediately to mind.

Finally, if you prefer to think about your program in a mathematical way, like a flow of mathematical functions that compute partial results and share them each to another, then functional programming is likely the answer for you.

## Typed vs Untyped Languages

To illustrate the difference between typed and untyped languages, there's a distinction I want to emphasize first:

  - An _identifier_, often called "variable, is the logical name that we use to refer to some entity inside the code.
  - The _data_ that an identifier refers to, is the actual memory content that is referred by that identifier.

A simple example will make everything clear. In Java, the line of code

```
int x = 1;
```

defines:

  - An identifier, `x`.
  - The data referred by `x`, that's the integer number `1`.

Alright. Now, here's another simple distinction:

  - In typed languages, at least one between identifiers or their data have types.
  - In untyped languages, neither identifiers nor data have types.

Let me make this concept clearer, with an example.

Python is a typed language. Indeed, if you write

```
x = 5
y = None
print(x + y)
```

you will get an error, because Python's interpreter knows that the type of `x` (numeric) and the type of `y` (None) are such that the two data cannot be added. In other words, Python knows the `add` operation (`+`) cannot work in this case.

Assembly, on the other hand, is a untyped language. It known nothing about types. It only knows about bits! Thus, you can add the bit-representation of `5`, which in 8 bits is `00000101`, to the bit-representation of None (let's assume that's just a sequence of `0` bits). This operation will be executed in a mathematically correct way. The problem is that the result is not what you wanted, very likely.

Let's consider Java again. Java is a typed language, because each identifier must be declared with a type. You can already see the difference with Python: both are typed, but Java is more "explicit". I am going to discuss this point in a minute.

Untyped languages are very rare today, especially among high-level languages. Assembly is an untyped language, because you don't deal with integers, strings, etc., you just deal with bits and words of memory.

## Statically typed vs Dynamically typed Languages

This is a key distinction, but also a very easy to understand after the previous one.

  - In statically typed languages, **identifiers have types**.
  - In dynamically typed languages, **data have types** (but not identifiers).

Thus, the distinction is easily understandable, but the implications are enormous! In my opinion, this is one of the most important things to understand in modern languages.

In fact, **what I said it above is imprecise**. Let me tell you the whole story, in view of the importance of this concept.

**A language is statically typed if the type of each identifier is known at compile time.**

This is the precise definition. The compiler can get this information in two ways:

  1. The programmer is required to write the type next to each identifier. Languages such as Java, C, C++ work like this, with keywords such as `int`, `double`, etc..
  2. The compiler infers the type of each identifier, during compiler. Most often, in a compilation step known as Semantic Analysis, that we will study in a future article (not too far away!). Scala is a language that works this way.

On the contrary, **a language is dynamically typed if types are known at runtime only, and not associated with identifiers but only with data**.

Languages such as Python, Perl, Php, are dynamically typed. For instance, in Python, you can write

```
x = 1;
x = "abc";
```

and there won't be any type associated with the identifier `x`. At runtime, the Python interpreter will know that, while the first instruction is executed, the identifier `x` is associated with an `integer` data, and then with a `string` data, when the second instruction is executed.

## Automatic Memory Management vs Manual Management

Every program you ever wrote, and each program you will ever write, interacts with the memory of the computer.

Declaring an identifier means (almost in all languages) to allocate some memory in the stack. Initializing an object in Java, means allocating some space in the heap part of the memory.

Even though we haven't discussed yet stack and heap, I am sure you understand how important is memory management in software design.

For every bit of memory that we allocate, there's a natural question: when is that bit going to be freed? And who will take care of doing it?

Quite simply:

  - In languages with manually managed memory, **the programmer is responsible to free memory for future usage**.
  - In languages where the memory is automatically managed, there's one module of the language itself that takes care of freeing memory slots every now and again. This module is usually called **Garbage Collector**.

The C programming language is a good example of manual memory management. To reserve some space in memory (the heap part), you need to use the instruction `alloc`. To free some part of memory, you must explicitly invoke the function `free`, and you also have to specify the address of the location you want to free (which, in C, means a pointer).

While this is a fine approach, and truly gives control of everything to the programmer, it's undeniable that **it creates space for errors and bugs**. In fact, it's realistic to say that almost all programs with manual memory management have some hidden memory leak!

Java was the language that brought automatic memory management, and Garbage collector, to the mainstream.

The idea, in Java as well as in most languages that uses such an approach, is that the execution environment will, at times, **stop to execute the main program in order to run the Garbage Collector**. This is a software module that, thanks to some algorithms, understands what pieces of memory can be freed, and does so. We will study these algorithms too, of course!

## Static vs Dynamic Scope

Although this is a good piece of knowledge to have in our bag, let me start with a disclaimer here.

_Static scope is much better and easier to understand for both humans and automated tools. Therefore, practically all important languages use static scope._

That said, let's see what's the difference between the two.

**With static (lexical) scope, the data referred by each identifier is determined by the source code.**

Despite the complex sentence, this is exactly as you would expect. When you write

```
programA {
    x = 1;

    function f() {
        print(x);
    }

    function g() {
        x = 2;
        f();
    }

    main() {
        g();
    }
}
```

the `main` function of the program invokes the function `f` and as you would expect that prints out `1` because it sees the global value of `x` defined at the beginning of the program. That's probably the only way you are used to, and the best way.

But there's another way.

If you are using a language with dynamic scoping, then the function `f` will be able to access the scope of the function `g`, because it was `g` to invoke `f`. The calling chain is

main --> g --> f

and in dynamic scoping that means `f` "can see" the scope of `g` as first thing. The scope of `g` contains the statement `x=2`, so `f` will resolve the identifier `x` inside the print command to `2`.

So, how does that work?

- In lexical scoping, **the "meaning" of each identifier depends on the source code**. That's why it's known as static scope.
- In dynamic scoping, **the "meaning" of each identifier depends on the current execution** and runtime status.

To be more precise, "meaning" of an identifier is better said as **binding**. Basically, what value has to be associated with a given identifier, in a particular point of the program.

In static scope, the binding depends on the source code only. And, most generally, **each function can see the scope of the parent functions** (even after they are terminated).

Another simple example will clarify.

```
Main {
    x = 1;

    function f1() {

        function foo() {
            x = 3;
            print(x);
            f2();
        }

        foo();
    }

    function f2() {
        print(x);
    }

    f1();
}
```

In languages with static scoping:

- Function `f1` will see the scope of `Main`, so it will bind `x` (if needed) to the value `1`.
- Function `f2` will do like `f1`.
- Function `foo` can also see the scope of `Main`, but if it has to bind the identifier `x` it will use its own scope, where `x` is set to the value `3`.
- Function `f1` can't see the scope of `f2`. Nor can `f2` see the scope of `foo`. As well, `f1` can't see the scope of `foo`.
- When the `Main` function invokes `f1`, this will first set `x=1` in its scope and then invoke `foo`. Then, `foo` will first bind `x` to the value in its internal scope, which is `3`, then print it and then invoke `f2`.
- When `f2` is invoked, no matter by what function is invoked, it can only see its inner scope and the outer scope of `Main`, but not the scope of `f1` nor `foo`. Therefore, it will bind `x=1` and will print "1".

This is the standard way, you should get used to it, if you aren't already.

But, if the language you're using it's based on dynamic scoping, then:

- Whether `f1`, `f2`, `foo` can see each other's scope depends on the invocation chain, that is known only at runtime.
- So, when the `Main` function calls `f1`, this will set `x=1`, call `foo`, set `x=3`, print `3` and then call `f2`. But now, `f2` can see all the scopes in the invocation chain (that is, `foo`, `f1` and `Main`) and therefore it will bind `x=3`, because it finds it in `foo`'s scope.

As you can easily imagine, **dynamic scoping can be very confusing**. That's why languages with dynamic scope lost the "historical war" against languages with static scope, and the former have practically disappeared.

## Conclusions

In this article we learned a number of concepts that are extremely important to wrap our heads around each programming language's features.

When you get in touch with a new language, asking (and answering) questions about types, programming paradigms, scope and garbage collector, will help you understand the language better.

And if you were not very familiar with these concepts, then be reassured that now you will understand better even languages you already knew, and **write better code**.

## References

Rather than a long list of references, I recommend to google each team you are interested in, such as "static scope", "typed programming language", etc.
