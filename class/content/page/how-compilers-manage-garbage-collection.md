---
title: "How do Compilers Manage Garbage Collection?"
date: 2019-10-12
draft: false
---

A Garbage Collector is the module of a programming language that takes care of cleaning the memory during the program's execution.

Even though we are used to it, Garbage Collection (also called **Automatic Memory Management**) is a relatively recent innovation in programming languages.

In the good old days, programming languages such as C and C++ asked the developer to take care of every bit of memory. For example, in C the developer should:

- Request the allocation of new space in memory, specifying how many bytes are needed. This is done with the function `malloc(...)`.
- Delete from memory those objects that aren't needed anymore, specifying a pointer to that location. This is achieved with the function `free(...)`.

Obviously, this way to manage memory has really nothing of automatic. It's fully manual.

Decades of experience have proven that **manual memory management leads to bugs** in every program, no matter how good are the developers. It's just too much to have to deal with (among many others):

- Memory leaks: when you forget to delete something from memory.
- Dangling pointers: should be dereferenced, but you must remember it!
- Override part of an object: maybe you thought it wasn't there anymore!

The truth is that this type of storage bugs are extremely difficult to find, and many productions systems nowadays still have some memory leaks that's still hidden. This is not a safe and robust way of building systems.

Automatic Memory Management, aka Garbage Collector, gained mainstream popularity with the rise of Java. Java was indeed the first enterprise language to become very popular and to have garbage collection.

**The principles of Garbage Collection** are the ones simple intuition would suggest:

1. When a new object is created, some unused space in memory is reserved for it.
2. Doing this over and over means the program will run out of free memory.
3. However, some objects that were created will never be used again and if we find them then we can free the memory.

There is one very important concept I would like to start our conversation from. **Garbage Collection is not a perfect system**.

Let's see what that means.

## No Garbage Collector is Perfect

The key concept that all automatic management systems use is that **a program can only use objects that it can find**.

In other words, if there is an object in memory that's not accessible by any part of the program, and therefore can't be used in any sort of operation (assignment, computation, etc.) then it can be deleted and the memory reused.

Let's take a look at one basic example.

```java
x = new A();
y = new B();
x = y;
```

With the first statement, some words of memory are reserved and a new `A` object is created there, then this object is referenced by the pointer attached to the variable `x`.

However, with the third statement the pointer attached to `x` is moved to a different memory location (one with a `"B"` object). Therefore, **the object created in memory with the first instruction becomes unreachable**. Thus, that space can be freed.

Let's now throw in two simple definitions.

An object `X` is _reachable_ if either:

1. One register has a pointer to X.
2. Another reachable object Y contains a pointer to X.

An _unreachable_ object is one that is not reachable. These objects are garbage.

Unfortunately, this approach, as sound as it is, leaves some "dark zone" in memory that are not considered unreachable, and therefore not reused, even though they should really be.

Let's take a look as one example where this can happen.

```java
x = new A();
y = new B();
x = y;
// assume y is dead here
// Garbage Collection starts here too, collecting y
if (true)
    x = new A();
else
    x.foo()
```

When Garbage Collector starts, it will collect the variable/pointer `y`, as we're assuming is dead, and it will also collect the object of type `A` created in memory on the first line.

It will not collect the object B created on the second line, even if `y` is collected, because that object is referenced by `x`.

However, that object will never be used again in the code, because the condition of the `if` is always true, and therefore the statement `x.foo()` in the else branch will never be executed. This means that the object `B` should also have been collected in the previous run of the Garbage Collector.

This is just one example, among infinitely many, that shows that **reachability is an approximation**.

With this piece of knowledge in mind, let's move on to analyze three of the most well known **algorithms for Garbage Collection**.

## Mark and Sweep Algorithm

As its name says, this algorithm works in two phases:

1. The Mark Phase, when all reachable objects are traced.
2. The Sweep Phase, when all unreachable objects are deleted.

Practically speaking, **this algorithm requires every object to have one more bit**, the mark bit.

Let's now look into the details of the two phases.

### Mark Algorithm
The idea of the first phase is to start from "roots" objects. These are the objects that are always reachable. For each of them, the algorithm finds all objects that can be reached, mark them as reachable (with the mark bit), and consider it as a root too.

```
Let T = {all roots}
While T is not empty:
    Select v from T
    T ← T - {v}
    if mark(v) == 0 then
        mark(v) ← 1
        Let v1,...,vn all pointers from v
        T ← T U {v1,...,vn}
```

## Sweep Algorithm

In the second phase, the Heap memory is scanned in order to find all objects that have the mark bit unset (that is, equal to zero). These are the garbage objects.

It's worth understanding that the "deletion" operation happens at the logical level only. Not on the physical level. Let me elaborate this point better.

When an object with mark bit unset is found, we know that we want to be able to reuse the memory currently taken by that object.

Actually clearing that memory is not something meaningful to do. In fact, it's not actually possible! How would "clear" a slot of memory? Would you set it to zero bits? But that could be a real object with zero value!

Thus, **the action of "deleting" the object simply consists in knowing that those memory slots can be reused**, and in saving this precious information somewhere.

To implement this logic, let's assume we have:

- A function `sizeof(p)`, that gives the size (in block of memory) the object pointed by p takes in memory.
- A list, called the _free-list_, where we can store the blocks of memory that can be reused.

Let's now look at the actual algorithm:

```
Let p ← bottom of the heap
    while p greater than top of the heap
        if mark(p) == 1, then mark(p) = 0
        else add the block [p,..., p + sizeof(p) - 1] to the free list
        p ← p + sizeof(p)
```

### Comments

Overall, Mark and Sweep is an easy to understand procedure. That said, it has many tricky details that's worth knowing about.

For starters, let's consider that the Mark algorithm, as I stated it, needs to create a list of objects that will contain first the root object, and then potentially many more objects.

But **Garbage Collector is run when the program is running out of memory**! It should not need to use a lot of memory to run! And what makes it worse is that the size of the T list is not known in advance.

The solution to circumvent this issue is **to use pointer reversal**. The algorithm avoids to create a new list, and instead follows the same pointers used to determine reachability, reversing them twice in order to follow all paths.

Let's look at one example to clarify how this works. Let's say we have five objects in memory, with only one root, the object A.

```
A → B → C
  \
   \
    D → E
```

So, we start at A and take the path towards B. When we arrive at B, we mark B as reachable (mark(B) = 1) and then we reverse the previous pointer, so there will now be a pointer from B to A.

```
A ← B → C
  \
   \
    D → E
```

We now continue, and we arrive at C. So we set mark(C) = 1, and we also reverse that pointer.

```
A ← B ← C
  \
   \
    D → E
```

At this point we notice that C does not have any other pointer. We can then go backwards thanks to the reversed pointers.

We walk back from C to B, and reverse again the pointer.

```
A ← B → C
  \
   \
    D → E
```

Object B does not have more pointers either, so we walk back one more step, and reverse that pointer too.

```
A → B → C
  \
   \
    D → E
```

Now we see that A has one more pointer that we didn't explore, so we go towards D following the same procedure.

This seems to work nicely, but there's still one obscure point. Let's consider again the action of reversing the pointer from B to A, during the initial step.

This operation cannot be done without care. First of all, B might not have space for a pointer at all! Secondly, if it does have space (like in the example), then this space is probably already used for a pointer to a different object!

Referring to the example, B has a pointer to the object C. If we changed that pointer and link it to A, then we would not be able to reach C!

In fact, this is a common problem that arises in algorithms that are based on work-lists (also called, to-do lists). It is often called **off-by-1 problem**.

The solution is to reverse the pointer of the second to last visited object. For example, when we arrive at C, we change the pointer B→C into A←B, and also use a temporary pointer to remember that we are onto B, because we will later need to add a pointer B←C.

Finally, it's worth knowing that the same trick is used to manage the free-list, in the sweep phase.

What is **the main advantage of Mark and Sweep**? One of its best features is that **it does not change the pointers in memory**.

In different words, objects are not actually moved around, within the memory. Some are marked as deleted, by adding those locations in the free-list, but those who are not will still be in the same memory slots, pointed by the same pointers are before.

This is a nice-to-have feature in languages that have explicit pointers. For example, in C pointers are explicit and exposed to the developer. Therefore, one could build a Garbage Collector for C using the Mark and Sweep algorithm. This has indeed been done.

Instead, if you wanted to use a different algorithm for Garbage Collector in C, one that changes pointers and object locations, you would really give a hard time to everybody who uses your Garbage Collector! When it starts, it would change the pointers that the programmer has used in his code!

What about **the disadvantages of Mark and Sweep**? The main problem it has is **memory segmentation**.

The whole idea of using the free-list is such that the computer's memory becomes very fragmented. Thus, at times it can be difficult to find a contiguous part of the free memory where to allocate some larger object.

## Stop and Copy Algorithm

This is another strategy for Automatic Memory Management. Its main idea is to split memory in two parts, both logically and physically.

- The _old space_, that's used to allocate objects.
- The _new space_, reserved to Garbage Collector operations (explained in a moment).

In addition, this system keeps track of a special pointer, called **heap pointer**. This points to the first free block of memory in the heap.

Basically, every block of memory below the heap pointer is already used. Everything above it, instead, is available space. Clearly, every time a new object is stored in memory, the heap pointer is shifted (up), according to the size of the object.

The Garbage Collector is activated when the old space gets full. Then, the algorithm:

1. Finds all reachable objects in the old space.
2. Copies all of them into the new space.
3. Switches the roles: new space becomes old space and vice versa.
4. Resumes the program.

This logically very easy steps are such that all garbage objects are left behind in the old space, and they will be overwritten soon. On the other hand, all reachable objects are copied into the new space (that will then become the old space), and can still be used.

### Comments

Even though Stop and Copy seems a very easy algorithm, there are a lot of tricky details worth knowing about, as for Mark and Sweep.

The peculiar feature of Stop and Copy, is that, in order to copy all objects in the new space, we need to move them and **find and fix all pointers in/out**. It's not obvious how to do that, because when we find an object, we can't see all pointers that points to it (whereas it's easy to see the ones that goes out).

To solve this problem, the trick is to use a **forwarding pointer**: when we copy an object, we mark it as "copied" and also add a pointer from the original object to its copy in the new space. This way, if another object had a pointer to the object we moved, it will be able to find the new copy using the forwarding pointer.

Another important point is: **how to implement the traversal without using extra space**?

The trick here is to **partition the new space into three contiguous regions**:

- The _empty region_, where we allocate new objects. There's a pointer `alloc` to the first block in this region.
- The _scan region_, where we have objects that have been copied but not scanned for their pointers. There's a pointer `scan`, for the first block of this region.
- The _start region_, where there are objects that have been copied and scanned.

To make a parallelism with mark and sweep, we can think of **the scan region as the work list**.

### Algorithm

Let's take a look at the pseudo-code for the Stop and Copy Algorithm.

```
While scan != alloc
  1. Let O be the object at the scan pointer
  2. For each pointer p contained in O
    a. Find O' that p points to
    b. If O' does not have a forwarding pointer:
       1. Copy O' to new space
       2. Update alloc pointer
       3. Set 1st word of old O' to point to the new copy of O'
       4. Mark old O' as copied
       5. Change p to point to the new copy of O'
    c. Else set p in O equal to the forwarding pointer
  3. Increment scan pointer to the next object
```

There are two other important points to note:

- We must be able to tell how large an object is (like with a function _sizeof_), and also in which words of the memory reserved to each object there are pointers. However, we can safely assume that **the Compiler knows all of this**.
- When performing Garbage Collection, we must check if any of those objects is referred by a pointer in the stack. Therefore, we must scan the entire stack and update all pointers. This can be an expensive operation.

Finally, it's good to know that **Stop and Copy is considered the most efficient Garbage Collection Algorithm**:

- Allocation is very cheap (just increment the pointer).
- Collection is relatively cheap, and is very cheap if there's a lot of garbage, because the algorithm only touches the reachable objects.

On the other hand, Stop and Copy can only be used in languages that do not expose pointers to the programmers. For example, it cannot be used in C because during Garbage Collection pointers are moved all around in memory, and would mess up the rest of the program.

## Reference Counting Algorithm

This is yet another Automatic Memory Management technique, and also the last one we will study in this article.

The basic idea is that instead of waiting for the memory to be almost full and only then start the Garbage Collector, the system tries to keep track of how many references each object has, and as soon as the counting is zero for an object - which means that object has become unreachable - it frees the space it occupied in memory.

To do this, obviously there must be an additional field in every object that counts the number of pointers to it.

What this implies, is that every assignment operation in the code must also take care of updating this field for all objects. Clearly, this causes some overhead in the program's execution.

Let's look at one example of how Reference Counting works.

Let `rc(x)` be the reference count field in object `x`. Assume we have two pointers `x`, `y` that point to two objects in memory `O`, `P`.

Now, it's important to notice that during Code Generation we must do a lot of additions in the code. Every assignment `x←y` in the original code is transformed to the following sequence of 4 instructions.

```
rc(p) ← rc(p) + 1
rc(o) ← rc(o) - 1
if (rc(o) == 0) then free(o)
x ← y
```

As it's easy to see, these 4 instructions are all is needed to perform Garbage Collection based on Reference Counting.

On the bright side, Reference Counting has two pros:

- Very easy to implement, for a basic implementation.
- Collects garbage incrementally, without waiting for memory to get full.

On the other side, there are some cons too:

- Cannot collect circular structures (because they will always have `rc=1`, even if they are unreachable from the stack).
- The manipulation of `rc(x)` makes the code much slower.
- For an optimized version of the algorithm, the implementation becomes really complex.

To solve the problem with circular structures, either the programmer remembers that each time one such structure becomes unreachable, they should change one pointer inside the structure to NULL, so then in a few steps the entire structure will be collected. Or, we should back this technique with another Garbage Collector technique that is run only once in a while.

## Summary and Outlook

In this article we learnt what Automatic Memory Management is, and how's often called: Garbage Collector.

We also studies three of the most well known Garbage Collector algorithms:

- Mark and Sweep
- Stop and Copy
- Reference Counting

There are a few points worth reiterating, in my opinion:

- There's **no doubt that Garbage Collector helps preventing storage bugs and memory leaks** in complex programs.
- Nonetheless, **it does reduce the control that the programmer has** over what happens in memory.
- Furthermore, the program execution is paused during Garbage Collection. This can cause some trouble, especially in real-time applications.
- Finally, one must remember that Garbage Collection Algorithms aren't perfect. **There's no guarantee that memory bugs are prevented**, no matter what system is in use. Reduced, yes. Avoided completely, forget about it.

As I briefly mentioned in a paragraph in this article, Automatic Memory Management is still an active research topic, and many other techniques have been proposed. For instance, here are some:

- Concurrent: Garbage Collector works in background in a separate thread.
- Generational: It does not analyze "too often" objects that are referred by a lot of places.
- Real-Time: There are limits on the time that the system can be paused during Garbage Collection.
- Parallel: The system uses (and coordinates) many Garbage Collectors at the same time.

## References

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

