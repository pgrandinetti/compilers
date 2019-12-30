---
title: "How Does the Computer Run Code?"
date: 2019-10-08
draft: false
---

This article is positioned between the front-end compilation (to decide whether a source code is correct or not) and the back-end compilation (to generate machine-runnable code and optimization). Before taking the leap, I want to understand the machine's internal organization, in order to truly grasp what to run code means.

Here's what I am going to discuss:

- Memory organization.
- Word Alignment.
- Activation and lifetime of procedures.

## Understanding Static vs Dynamic

A **key distinction**, that one should really understand deeply, exists between what happens during compilation time, and what happens at runtime. It's common to call the former static, and the latter dynamic.

Generally speaking, when the compiler makes a completely informed decision about something, we say that's a static operation. It basically means that the compiler has all the information it needs, can see the full picture and ensure that the operation is correct.

Let's look at one example. Let's consider statically typed programming languages that are not OOP and therefore do not support inheritance. In this case, the compiler is in charge to check that every operation is correctly typed, that is, the objects used in that operation are consistent in terms of their types. The compiler is fully capable of doing this, because it knows the type of each object currently defined in the program, and also knows the rules that govern how types can be used each next to another. This is what happens, for example, in the C language. It's also how the programming language I designed works (see the references at the end).

What's dynamic then? The easiest example I can think of is with I/O operations. Let's take a look at the following Java snippet.

```{java}
import java.util.Scanner;

class MainClass {
    public static void main (String args[]) {
        Scanner sc = new Scanner(System.in);
         int x = sc.nextInt();  // ask the user to enter from keyboard
        // do something smart with x...
    }
}

```

This code would be compiled correctly by the Java Compiler, and it's indeed well written. However, the compiler has no way to know if the user will enter a sequence of digits or not. There's nothing that forbids her to type, for instance:

```
>> 123R31
```

This is not a valid integer, therefore the method nextInt() of the class Scanner will fail. In Java- terms, it will raise an exception.

The compiler cannot possibly know in advance what the dynamic type resulting from the method nextInt() is. Or, to better says it, nextInt() will always expect an integer, or fail brutally. This happens at runtime.

I want to show you another example of dynamic operation, a bit more sophisticated. Let's consider again a Java snippet.

```{java}
class A {
    ...
}

class B extends A {
    ...
}

class MainClass {
    public static void main (String args[]) {
        A obj1; // static type of identifier "obj1" is A
        obj1 = new A();
        B obj2; // static type of identifier "obj2" is B
        obj2 = new B();
        obj1 = obj2; // valid assignment thanks to inheritance
        // now the dynamic type of obj1 will be B
        ...
    }
}
```

If you know a bit of Java (or pretty much any other OOP language), the example should explain itself. When you create a variable of type A, that will be the static type for that variable for the rest of the program. However, if A has any subclass, then you can assign to the same variable objects of any of such subclasses. If you do that, then **the runtime type (dynamic type) of the identifier will be different from its static type**.

## Program execution, Memory and Operating System

The other important concepts we should master are about how the Operating System (OS) interacts with the program we compiled and now want to execute.

What happens when someone starts using a computer and wants to run a software?

That software can be pretty much anything. It can be your internet browser, a video player, a simple program you just wrote and want to test out. How does the machine deal with it? In happens in a few steps.

First, the OS allocates space in memory for this program. Depending on what OS you are using, and what type of environment your program is run in (like a virtual environment, or directly the machine registers), the allocated space can be protected or not.

For example, it's well known that Java programs are run into the Java Virtual Machine, and therefore offers some more protection with respect to C programs, for instance, from a security point of view.

Secondly, the code for the program is loaded into the allocated memory. Again this is just the principle and the execution of such principle may happen in various forms.

For example, a program that you compiled down to the machine level (for example a C or Assembly program), might be loaded directly into the computer's memory. On the other hand, a program that was compiled in some form of bytecode, like Java programs are, will be loaded into the Virtual Machine that only later interacts with the computer's memory and overall system.

The third and final step (final as far are this discussion is concerned) is to jumpstart at the entry point of the program. You can imagine the OS having a pointer to the first operation that must be executed and moving it right to where the program asks.

For example, in C programs that would be the function main. In Java programs, that would be the main method inside the class you set as executable. In Python programs, the entry point is the first line of the file that you give as argument to the interpreter.

Once the entry point is decided, code is run sequentially line after line. That is, of course, unless some branching or jump instruction is met. In that case, well, I cannot really predict what will happen!

### Memory Visualization

There's a final detail I consider worth keeping in mind. And it's about how the machine memory is _physically_ organized.

In truth, this is all about a convention that has traditionally been applied all over textbooks and papers: memory is depicted from the low addresses to the high ones.

<img src="/images/memory1.png" alt="Computer memory view" width="280px">

Clearly, in a real machine it's not even guaranteed that these spaces of memory are contiguously allocated. This is why we use logical names for variables, instead of hexadecimal addresses!

From the figure you can also see that memory is split in two parts. The first part is where the code is loaded and it's said to be loaded starting from the lowest address (again, this is a convention). Also, this is a fixed-size part, unless you're on a very atypical architecture.

Then, there's the part that will contain data of the program. For every meaningful program that's run, this part of the memory changes very frequently during the execution. You allocate new variables, modify them, create complex structures or objects, and much more.

Next, we are going to see that the Data part of the memory can be described more in detail.

## Activations and Lifetime

Now that we know what happens in the OS when the user request a program to be run, we can start looking at how it is run.

The first thing to do, for the sake of this discussion, is to make two assumptions:

1. The code is run sequentially. Don't be mistaken by this definition: it totally allows jumps and branches, and it simply means there's a well defined order with which statements are executed.
2. When a procedure (aka function, method) is completed the control immediately returns to the point where that procedure was called.

These two seem very intuitive and "permissive" assumptions. And indeed are valid for a wide range of programs. However, they also cut off some. In particular, **all multithreading programs do not fulfill these constraints!**

It's also worth introducing two definitions, when talking about procedures.

1. We use the word **activation of P** to simply describe the activation mean of a procedure P. It involves all we said earlier, like memory and code allocation, entry point.
2. The **lifetime of an activation** of P is made of
  - All steps required to execute P,
  - All steps required by sub-procedures invoked by P.

At times you can also hear or read about the **lifetime of a variable**, say x. In such case, this term refers to all the steps where x is correctly defined in the code. Or, if you will, all the steps included from the moment x is initialized till the moment it's destroyed (deallocated).

## Activation Tree and Stack

The very important observation we have to draw from the previous concepts is the following: **if a procedure P calls a procedure Q, then Q must terminate before P does.**

On a more general level, this observation implies that all activations in a program can be represented by a tree data structure. Why? Because they are correctly nested, and therefore it's always possible to find a parent-to-child relation.

Things can get arbitrarily complex, of course. For example the same procedure P can be child of multiple parent procedure. But this would happen in different parts of the tree, so it'd be all good.

Or, one more example, procedure P can be a child of procedure Q, which can then be child of P, recursively. That would also be fine with a tree structure.

Thus, we use the term **Activation Tree** to refer to the nested levels of activations that happen in the whole program.

Just knowing the overall distribution of activation, with a tree structure, might not be enough. Generally speaking, it's also very useful to keep track of what are the activations that are currently active.

For example, if the main program calls a procedure P and that, before terminating, calls a procedure Q, then P and Q are currently active. The fact that the Activation Tree also contains a procedure R that might be activated in some future it's quite irrelevant.

This is why it's good to keep track of the currently active procedures. Thanks to the property I announced earlier (that when a procedure has been called then it must return before the caller returns), it's enough to keep a stack data structure. We call it **Activations Stack**.

As you can imagine, a procedure is pushed onto this stack every time is activated, and is popped off the stack when it returns. This mechanism guarantees correctness of the nested calls. By this point in the class you should be fluent enough with stack to see that!

Finally, the Activations Stack **is stored in the Data part of the program's memory**. Hold this concept in your mind for a minute, as we'll get back to it very soon.

## Activation Frames

After all this talk about activations, we've finally arrived to the most natural question. **What information do activations carry with them?**

First of all, let's give the concept a name. We use the word **Activation Frame**, or sometimes **Activation Record**, to describe the set of information that activations bring on the table.

These frames need to carry with them quite a lot of information. For example, if a procedure F calls a procedure G, then the record for G will have to contain a mix of information about F and G. Why? Well, that's for a number of reasons. Consider, for instance, that F is suspended in the moment it calls G. Therefore, information about how to reactivate F must be contained in G's activation frame.

There's no universal recipe on how to design the Activation Frame. In practice, there are many small differences and nuances that change in each compiler.

Generally speaking though, Activation Frames contain the following four blocks:

1. The return value (initially empty, of course).
2. The arguments.
3. The link to the caller.
4. The address to the return (basically, the line of code that must be executed after the procedure returns).

A few more concepts worth sticking into our minds:

* Even though the four blocks are logically independent, they really are just one big block of memory inside the machine. The fact that they are contiguously allocated plays an important role, because their content can be accessed much faster this way.
* After a procedure returns its activation frame can be kept in memory for quite some time. And it can be useful for a number of purposes. For instance, the return value of this sub-procedure will be needed. In this case, it's very important to know that the result value is (e.g.) the first block of its activation record, so it can be accessed by the calling procedure very efficiently.
* Strong compilers try to save Activation Records as much as possible in the registers of the CPU, especially method result and method arguments. No need to say it, this is done for the sake of efficiency. Registers are the fastest locations to access, in pretty much all architectures' memory.

Last, but definitely not least, there's a very important concept I want to underline.

It's crucial to understand that **all we discussed so far happens at compile time**. So, the compiler must be able to determine the layout of the activation records, and generate code that can access any sub-block in any of such records.

In conclusion, and the message that you should really remember, **the Activation Frame layout and the Code Generation module must be designed together**.

We haven't seen an inch of what Code Generation means yet, but that's on the roadmap for future articles!

## Globals and Heap

Globals and Heap are two things you and I are used to see, and to use when reading or writing programs.

But just in case:

- Globals are variables that are visible throughout the program, in all files. For example, a Java's object private attributes are not globals. You don't have their definitions available outside of the class. On the other hand, a Matlab variable declared as "global" is… global!
- Heap is the name used to describe the portion of the memory where objects and data are dynamically allocated while the program runs, on-demand. Keep in mind that the same term is also used to indicate a specific type of tree data structure, but that doesn't concern us, for the time being.

I want to discuss with you how Globals and Heap are handled by the compiler, in terms of where and how they are allocated into memory.

The best way, in my opinion, is to think about this logically and draw our own conclusions. You'll be surprised (or maybe not!) to discover how well our intuition will work!

As we said, globals must be accessible everywhere in the program. This means that they memory address should not change during execution. It could change, in theory, but that would make memory management awfully complicated.

Thus, **the compiler creates a statically allocated memory** that is devoted to store globals. By convention, this is said to be at the top of the Data part of the memory. Remember, though, that "top" means low-addresses. And below it there is the Activation Stack, with all Activation Frames for the currently active procedures. Here is how you can look at it.

<img src="/images/memory2.png" alt="Computer memory view" width="280px">

In reality though, there is a bunch of other things that make sense to store in a "global" place, not just global variables. This is why compiler's people more frequently talk about **static data** part of the memory. What's in there vary from compiler to compiler, but the idea is that such part of the memory is right below the Code part, and contains all statically allocated data.

In addition to static memory, each program needs a number of dynamically allocated memory slots. In some cases these slots live with the procedure: it means they can be deallocated when the procedure returns. In such case it may make sense to just use the activation frame.

In more cases though, objects allocated by a procedure must survive the procedure itself. In other words, **that data must be available even after the procedure terminates**. An example? Just about the return object of any function.

This is where the Heap enters the game.

The Heap is like a big bucket where objects can be dynamically created, allocated, destroyed and deallocated. A little secret: it's called "heap" because the data structure compilers use most commonly for it is really a Heap.

The Heap is represented by convention below the Activation Stack. So, to summarize, here's a list of what we find in the memory from the low to the high addresses:

1. The code area. Contains the code, and for many languages it's a fixed size, read-only area.
2. Static area. Contains data (not code) with fixed address, like global variables. It's fixed size, and can be writable.
3. Stack area. Contains the stack of activation frames for the currently active procedures.
4. Heap area. Contains everything else really, including all allocated area. In C this can be managed via `malloc` and `free`, in Java via `new` (and garbage collection).

There's a final, very interesting point, that I believe is worth discussing.

From all we discussed so far, it's clear that both the Activation Stack and the Heap grow and shrink during the program execution. But what happens then if they step on each other's feet?

As you can imagine, that would be quite bad. Indeed, it's something to avoid at all costs.

Sometimes the simplest and most elegant solution is also the best one. In this case, the idea is to initialize Stack and Heap at the opposite sides of the memory's boundaries, and **let them grow one towards the other.**

This is the final picture you should remember.

<img src="/images/memory3.png" alt="Computer memory view" width="280px">

At any moment during the program's execution, if the pointer to the next space available for the Stack becomes equal to the pointer to the next space available for the Heap, then it really means **the program has run out of memory**. This is when you'd see some Error or Exception such as Out-Of-Memory-Error, and the program would be aborted.

## Alignment

Alignment is the last subject I want to discuss in this article, even though just briefly. It's a concept that lives at the very lowest level, from the point of view of machines' architecture, but it's nonetheless quite important to understand, in my opinion.

Let's start from some very basic concepts:

- Modern machines are built either with a 32- or a 64-bits architecture.
- A **word of memory** is made of 4 bytes (that is, 32 bits) in a 32-bits architecture, and is made of 8 bytes in a 64-bits architecture.
- **Any machine's memory is accessible either per byte or per word**. Not really per bit, in general.

With that said, alignment is really just about a single definition.

_We say that some data is word aligned if it begins at a word boundary._

For example, if you want to allocate an object that needs 32 bits in memory, and your machine is on a 64-bit architecture, then your data would be aligned if:

- It starts at the bit 0 (in which case it'd take till bit 31), or
- It starts at the bit 64 (in which case it'd take till bit 95), or
- It starts at the bit 128,
- Etc...

Basically, the first bit where data "starts" must coincide with the first bit of a new word, in memory.

What happens when data is scattered around in memory and not well word-aligned? Two nasty consequences may happen:

- The program will not execute at all, or not correctly. This is a rare event though.
- The program will run much more slowly than you'd expect. The drop in performance can be very large. This is the most common case.

The question, as always, is **why is that?** The reasons are related with deep hardware considerations. The way CPUs are built, and logically connected with registers and memory inside the architecture. And I am really talking about electrical connections! Sadly for me, I have a very poor knowledge about this subject, so that's all I can tell you!

The takeaway message is that word alignment is so important that often compilers leave a lot of empty (unused) space in memory, to make sure most of data is word aligned.

Thus, you also start understanding that writing the back-end part of a Compiler, the part that deals with Code Generation and Optimization, **is intimately related to the architecture that the program is supposed to be run on**. This is why you have many different C compilers, and many different JDKs. These are subjects for the next article!

## Take-home messages

This article may seem like a collection of randomly chosen subjects. Believe me, that's not the case. I illustrated many different concepts, true, but they will all be useful in the upcoming articles. We spoke about very low level concepts, related to how Compilers organize the memory that has to be dedicated to the program's execution.

The most important concepts in my opinion are the following ones:

- Everything that the Compiler is able to decide with certainty is static. It's a decision taken at compilation time, and the Compiler if fully capable of understanding its consequences. If not, then there's some dynamic variability that can cause issues, at runtime.
- Compilers allocate memory for the program's execution. The top of such space contains the Code for the program, and it's statically allocated.
- The key data structures for the correct execution and memory management are Activation Frames. They must be designed together with the Code Generation module, because the two are very strictly connected.
- The dynamic allocated memory is called Heap and contains Activation Stack and the actual dynamic Memory (heap, or bucket). They grow one towards the other.

## What's next

Now that we know quite well what has to happen in the computer's memory during the program execution, we are ready to jump to the Code Generation.

This is going to be a fascinating subject. Basically, the goal is to take the original source code, and whatever data structure was produced by the Parser and by the Semantic Analysis, and literally translate that source code in something that the machine can understand, and run!

See you there!

## References

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

My article [Why There Are So Many Programming Languages]({{< ref "/page/why-so-many-programming-languages" >}}) (Talks about front-end, back-end compilation).
