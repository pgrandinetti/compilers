---
title: "How To Generate Machine-Runnable Code?"
date: 2019-10-08
draft: false
---

Software developers are usually interested in writing a few thousands lines of code, and then run that code. They rarely ask deeper questions.

In this series of articles, I did ask many deep questions. My objective was to learn about Compilers, Programming Languages and also to create a new programming language, to get hands-on practice.

To summarize, we’ve learnt about:

- Formal Grammars.
- Software modules that make a Compiler.
- Lexical Analysis, Parsing and Semantic Analysis.
- Memory allocation and management.

Thus, one fundamental question is still left unanswered: **how is the source code (that you wrote) run into the computer eventually?**

## You’ve got a computer and some code, so what?

In the previous article in this series about Compilers, Formal Grammars and Languages, we learned many important concepts about computer architecture, and in particular about the system’s runtime organization:

- Memory allocation for the Code of the program.
- Activations and Lifetime of procedures.
- Activation Stack and Frames.
- Heap memory.
- Static data and globals.
- Alignment.

I can’t help but recommending to check that article for deeper insights (see the references at the end of this one).

In summary though, the previous long article explains how the machine’s memory is internally organized by the Compiler that wants to prepare some code for the execution. Overall, you understood that **a strong compiler needs to have a deep knowledge of the architecture the code will be run into.**

This is why there exists a lot of different C compilers. C is a language designed to be extremely efficient, therefore C compilers are made ad-hoc for each architecture.

On the other hand, there are much less Java Virtual Machines around. That’s because the Java language is designed to be run into the JVM, not directly on the system’s architecture. It may seem a subtle difference, but it makes a world of difference.

The important message to remember is that **the Code Generation process of a Compiler depends heavily on what architecture the Compiler is made for**. You can’t make a compiler for all kinds of computers, if you want it to be a good compiler.

So, how can we approach this subject when studying Compilers? We can’t study all possible architectures, that’s for sure!

I decided to follow the advice of Stanford’s Prof. Aiken. In his class about Compilers, he uses **the stack machine as a model and as a learning tool**.

## Stack Machine Overview

A stack machine is a type of computer architecture that uses a stack as only storage. That means that the operands for every operation are always at the top of such stack, and that the result of every operation is also pushed on the same stack (after popping the operands, of course).

This is, conceptually as well as practically, extremely different from the more famous registers machines, where the location of each register (and hence, of the data) is explicit.

The hybrid that comes from mixing registers machine and stack machine is called **N register stack machine**. A long name that simply means the top N locations of the stack are actually stored in registers.

We are going to continue this article by using the **1 register stack machine** (that is, N=1). This is a stack machine where the top of the stack is stored in a register, that is conventionally called **accumulator**.

I believe that a simple example will make a lot of details clear. Let’s consider an operator `op` that takes `n` operands `e1,..., en`. The operator can be really anything, even a simple sum. The operands may be complex subexpressions.

Thus, we have an expression to evaluate: `op(e1,..., en)`. How do we do that in a stack machine?

1. For each expression `ei, i=1, ..., n-1` (not the last one).
  - Compute `ei` (this saves the result in the accumulator).
  - Push the result onto the stack.
2. For `en`, simply compute the result (which saves it in the accumulator).
3. Pop `n-1` values from stack, and compute `op` (because the result of `en` is already in the accumulator).

The key point to remember is the standard sequence **pop-compute-push**:

1. Pop something from the stack.
2. Use it to compute something else.
3. Then push the result onto the stack.

## Architecture details and Instructions

Let’s continue the discussion about Code Generation by taking the stack machine as an architecture model. Other than this, we also need a **set of instructions**.

What are instructions? I use the word instruction to refer to a line of code that it’s executed directly in the architecture. Let me clarify with two examples.

In an assembly code, the line

```
MV reg1 reg2
```

which copies the content of register reg1 into register reg2 is what I’d call instruction.

In a high-level programming language, like Java, the line

```
int a = 5;
```

could also be called “instruction”. However, for high-level constructs I prefer different terminology, such as assignment, operation, expression, etc. So, let’s agree that an instruction is something you run at the lowest level possible.

The set of instructions is decided by the manufacturer of the architecture. For example, Intel CPUs have their own set of instructions, which is a bit different from the instructions available for AMD CPUs, and so on.

What does that mean? It means that, in order to continue our studies, we need to settle on a choice of architecture type, and then use its instruction set. I want to follow Prof. Aiken again here, so let’s take the MIPS architecture and use it as if it was our machine.

The MIPS architecture is a **highly realistic example**, which is why I like this choice. A few details about it, along with important choices:

- The register `$a0` of the MIPS architecture is a standard choice for the accumulator of a stack machine. Any register would work of course, but let’s stick with `$a0`.
- Let’s assume we keep the entire stack of the stack machine into the MIPS memory. As only exception, remember that `$a0` is a separate register but it’s also the top of the stack machine.
- In the MIPS processor the stack grows towards the low addresses. I discussed this in detail in the previous article.
- MIPS is a 32-bit architecture, so each word is 4 bytes.
- The stack pointer register `$sp` points to the first word of unused memory. This means that, since the stack grows towards the low addresses, **the top of the stack is at $sp + 4**.
- MIPS has 32 general purpose registers. However, we will only use `$a0` (accumulator), `$sp` (stack pointer), and `$t1` (a temporary register).

Let’s now see a few instructions that MIPS supports.

_Load word_

The syntax is `lw reg1 offset(reg2)`, where

- `reg1` is a memory address.
- `reg2` is also a memory address.
- `offset` is a number (hard-coded).

This instruction takes content of register `reg2`, sums up the hard-coded `offset` and interpret the result as a memory location. It copies the content of such location into register `reg1`.

_Add_

The syntax is `add reg1 reg2 reg3`, and it simply does `reg1 ← reg2 + reg3`

_Sub_

Similar to the add instruction, we can write `sub reg1 reg2 reg3`. Guess what it does.

_Store word_

Syntax is `sw reg1 offset(reg2)`. It stores the 32-bits word currently in `reg1` in the address located at `offset + "content-of-reg2"`.

_Add immediate unsigned_

It’s used as `addiu reg1 reg2 imm`, where `imm` is a hard-coded number. The operations saves in `reg1` the value `"content-of-reg2" + imm`. The sum is unsigned, that means the overflow bit is discarded.

_Load immediate_

Syntax: `li reg1 imm`. It simply loads into `reg1` the hard-coded value `imm`.

_Branch if equal_

Obviously there’s a branch instruction too. It’s used as `beq reg1 reg2 label`, and it makes the execution flow jump to `label`, if and only if the content of `reg1` is equal to the content of `reg2`.

_Unconditional jump_

Jump to some label without any check, with the line `b label`.

Alright, with as few as eight instructions we are ready to generate machine code for some real complex high-level source code!

## Code Generation - Basic Example

The easiest way to understand the overall concept, for me, was to start with a very basic example. I want to show you how to generate code for a stack machine first, and for a MIPS architecture second. And the code will be implementing the extremely simple expression:

```
print (5+7)
```

Before starting, it’s worth underlining one point. While we are going to generate code “manually”, the compiler is supposed to do it automatically! We’ll see how to create **templates for automatic code generation later**. For now, let’s stick with the basic example.

Generating the result of the operation `5+7` is very easy in a stack machine. We have to:

1. Save the first operand (5) into the accumulator.
2. Push the accumulator on the stack.
3. Save the second operand (7) into the accumulator.
4. Sum the top of the stack and the accumulator.
5. Pop the accumulator, assuming that “pop” would also “return” or “print” the result.

So, overall, it’s just a few lines:

```
acc ← 5
push acc
acc ← 7
acc ← acc + top_of_stack
pop
```

Now let’s see how to do the same in MIPS. First, we have to load the number `5` in the accumulator, and that’s exactly what the Load-Immediate instruction does.

```
LI $a0 5
```

Then, we have to transfer the value we just stored to the top of the stack, because we need the accumulator for other purposes. Remember, the top of the stack is identified by the content of the register `$sp` (stack-pointer). Also, we have the Store-Word instruction. So, putting all together:

```
SW $a0 0($sp)
```

Notice that we are applying `offset=0` to the content of `$sp`.

Now there’s a tricky thing to think about. We just pushed something onto the stack. Suppose we pushed 1 word, that’s 4 bytes in MIPS. If you want to see some numbers for the sake of clarity, you can imagine that initially `$sp = 100` (first unused memory location) and after the `SW` instruction the first unused memory location is not `100`. It’s `96` (4 bytes less, since the stack grows towards the low addresses!).

This means that the current value of `$sp` is not the top of the stack! We have to reset it, and we can do so with the Add-Immediate-Unsigned instruction.

```
ADDIU $sp $sp -4
```

Now let’s load the second operand into `$a0`

```
LI $a0 7
```

At this point, we would be ready to compute the sum. However, MIPS instruction (Add included) only works with registry address, so we first have to load the value at the top of the stack into the temporary register `$t1`.

```
LW $t1 4($sp)
```

Notice that this time we are using an `offset = +4`. The reason should be clear. We did reset the stack pointer to `$sp=96` (following the example above), but the value we stored was 4 bytes above.

Now, finally, everything is ready for the Add instruction.

```
ADD $a0 $a0 $t1
```

which, as we know, will do `$a0 ← $a0 + $t1`.

We aren’t done yet! The final step is to make a “pop” operation, because we already used the second operand but it’s still onto the stack.

We can simply shift the `$sp` up by 1 word, so that the word that’s currently occupied by the second operand will be overwritten soon.

```
ADDIU $sp $sp 4
```

Putting all together, the code generated for MIPS is the following.

```
li $a0 5
sw $a0 0($sp)
addiu $sp $sp -4
li $a0 7
lw $t1 4($sp)
add $a0 $a0 $t1
addiu $sp $sp 4
```

We are ready to switch gear and take on some more complex scenario.

## Template for High-Level Code Generation

As we are approaching the end of this series of articles on Compilers, it should be quite clear that the central blocks of a programming language are the expressions.

Sure, there are a lot of keywords, parentheses, commas, and other stuff. But the “computability power” of the language is given by the expression. This is why Context-Free Grammars are more powerful, in terms of expressivity, than Regular Grammars. I dedicated an entire article to this subject, so I recommend checking it out (see the references at the end).

From Code Generation point of view, generating code for keywords does not sound too difficult. And indeed it is not.

If you want to “translate” your code in another high-level language, there’s probably a one-to-one relation between keywords in the two languages.

If you are going as deep as machine language, then keywords simply do not exist. You must translate the logic. And where is the logic of the program contained? In the expressions!

If you want to object, for example because you think the logic of a program is given by the branching operations (if-then-else), well that’s right. But based on what do you branch? I’ll tell you: based on the result of an expression!

For example, in my programming language (yes, I am building a new programming language as a learning tool for these studies), the branching operation is defined in the grammar as it follows.

```{}
ifLine      :   'if' ifCond ifBody
ifCond      :   '(' expr ')'
ifBody      :   (line ENDLINE)+ (optElse)?
optElse     :   'else' (line ENDLINE)+
```

And you can see very well that the ifCond Token is nothing more than two parentheses and an expression. **You must evaluate expressions to understand what the code does**.

Let’s then approach the problem in a scientific way and find a solution, as general as possible, to the problem of **code generation for arbitrarily complex expressions**.

### How does Code Generation work in practice?

This is a subtle point that I want to clarify as soon as possible. What does it mean to “generate code”, practically speaking?

Let’s think again about the sequence of steps in a Compiler.

First, the front-end. The Lexical Analysis generates a sequence of Tokens. Then, the Parser generates a parse tree. Finally, Semantic Analysis performs a number of additional checks, for instance about typing, and uses Symbols Table and Scopes Stack.

Then, the back-end. The Optimization modules takes care of finding improvement in the source code (I haven’t studied that yet with you). And the Code Generator produces a different code, one that can be run.

Thus, the key sentence is **the Code Generator produces a different code**. And this is really all you need to understand. The final product of the compilation is a source code too, very different from the source code that was given as input at the beginning.

You can **imagine Code Generation as a function that takes a bunch of stuff (original source code, parse-tree, Symbol Table, etc.) and produces a string**. Yes, just a string! What is this string? It’s a source code in a different language, **one that can be run**. It can be a string of MIPS code, Python code, C code, and so on. As long as you can run it, you have generated code!

## Generate Code for Expressions

Back to expressions. Now that we know Code Generation is like a function, we can confidently talk about the function

```
cgen(e)
```

where `e` is an expression of some sort. We know that `e` is valid in the original language, because the original source code has already been checked by the front-end modules.

What is the output of this function? It’s a string, as I said one minute ago. Just checking if you are following along.

There are two extremely important things that `cgen` must do:

- **Save the result** of the expression in `$a0`.
- **Preserve the status** of `$sp` and of the whole stack.

The second point is really crucial. It means that the content of `$sp` and the content of the stack before `cgen(e)` is run must be the same after it’s finished. Keep this in mind, seriously.

The actual output of `cgen(e)` will depend on what `e` does, of course. The most basic expression is an integer number, in pretty much all programming languages. So, what does cgen produces in this case?

Let’s assume the expression is the integer `i`. The meaning of this expression is to create this integer number into memory and to keep it there for more useful purposes, hopefully.

So `cgen` for this expression should generate a string of code that does exactly that. If we are to generate MIPS code then we can use the Load-Immediate instruction.

```
cgen(i) = "LI $s0 i"
```

What would you do with the output string? You would save it into a file and then load it in memory to run. Or you would use it to fill some blank inside a template string. Obviously, the latter is the most common case.

And so we understand what cgen really does. It produces strings of code that must be plugged-in into a bigger file (or template, if you will). Imagine such a template like a form with blank spaces to fill for all expressions.

Just to remind ourselves about two critical concepts, note that:

1. After `cgen(i)` has run, the stack is preserved. In fact, it hasn’t been touched at all.
2. Same holds true for `$sp`.

There’s one more key point that I would like to discuss now. We must understand that **the function cgen(i) is executed at compile time**. In other words, it’s run by the compiler. **The code it produces, instead, will be executed at runtime**. In other words, it’s run by the Operating System.

When I said that expressions are the central blocks of a language, I meant literally. They are “blocks” because they can be put together, plugged into more complex expressions, to generate very meaningful “sentences”.

For example, let’s look at the case where the expression to generate code for is not an integer, is a sum. And not just any sum, it’s the sum of two subexpressions!

What are these subexpressions? They can be really anything, potentially. We don’t care too much because we are going to use the usual, simple, recursive scheme that you know by heart at this point.

So, we have `e = e1 + e2`, and we want to compute `cgen(e)`. Here’s what has to be done.

1. Compute the value of `e1` (this will put the result in `$a0`).
2. Put it on the stack.
3. Reset the stack pointer, like a push.
4. Compute `e2`.
5. Load the result of `e1` from the top of the stack into a register `$t1`.
6. Add `$a0` (that contains the result of e2) and `$t1`.
7. Reset the stack pointer, like a pop.

Here’s how it should look like. I have written down explicitly what’s run at compile time and what at runtime.

```
cgen(e1+e2) = 
    cgen(e1) // compute e1, put results in $a0 and preserve the stack - COMPILE TIME
    SW $a0 0($sp)  // push the result onto the stack  - RUN TIME
    ADDIU $sp $sp -4  // reset the stack - RUN TIME
    cgen(e2) // COMPILE TIME
    LW $t1 4($sp) // load the result of e1, computed at the beginning. The offset=4 works
                  // because cgen(e2) is assumed to preserve the stack. - RUN TIME
    ADD $a0 $t1 $a0  // $a0 ← e1 + e2  - RUN TIME
    ADDIU $sp $sp 4 // reset the stack pointer (like a pop operation) - RUN TIME
```

I think there are two takeaway messages a this point:

* The code generator for the “sum” expression works like a template. It has some fixed part (the ones executed at runtime) and then some placeholder for the recursive calls `cgen(e1)` and `cgen(e2)` that will be injected.
* Stack machine code is highly recursive. This means that, at least for expressions, it can be written as a recursive descent in the parse tree.

### Generate Code for Other Grammar Construct

Even though expressions are the central blocks of (practically) any language, a Grammar is made by many other pieces. Runnable code must be generated for them too, of course.

Here’s where the idea of templates becomes really helpful.

You have read my previous article on Formal Grammars and therefore you know that the most part of the grammar is made of rules that recursively use simpler rules. For example

```{}
ifLine      :   'if' ifCond ifBody
```

is a high-level rule that uses two other, simpler rules, `ifCond` and `ifBody`. In pretty much the same way, they also “call” other rules.

```{}
ifCond      :   '(' expr ')'
ifBody      :   (line ENDLINE)+ (optElse)?
```

After all, this is exactly the concept of Recursive Descent, which you know very well since we used it to implement the Lexical Analysis, the Parser and also some part of the Semantic Analysis!

This logic lets us understand that we can create a few custom code-generator rules for the “final” pieces of the Grammar. These pieces correspond to the leaves in the parse tree. Once we have these rules, we can write templates for the higher level parts of the Grammar, and then plug into them the former ones.

For example, one rule in the Grammar I designed is

```{}
output      :   'writeOut' obj
```

We could have, in my language, the following, valid line of code:

```
writeOut 5;
```

How would we generate code for a “output” Token then?

First, we generate the code for the keyword `writeOut`. If we were translating into some really low level language, then this corresponds to some system call to display objects at screen. If we are translating into a high-level language, say Python, we would simply output the string

```
“print (<fill-template>)”
```

To fill the template we look ahead where we find the `obj` token. What does the Grammar say about this type of Token?

```
obj         :   var | num | str | bool | null | list | listElem
```

So we look even deeper in the parse tree and we discover that this `obj` is actually an integer (the number `5`). And now, the magic, we **delegate the task** to a function that generate code for integers.

Overall, you can imagine a function `cgen` for the output token that is defined in the following way.

```
cgen("writeOut obj") = 
        "print ("
        + cgen(obj)
        + ")"
```

The output of this function is a string in Python language.

## Code Generation - Tougher Example!

I want to go through a more involved example now, to see and touch for real the concepts we discussed in the previous section.

I want to generate code for the MIPS architecture that realizes the following higher level code.

```
if (e1 == e2) then e3 else e4
```

Here are the logical steps we have to implement.

1. Generate code that will compute e1 and will put the result in the accumulator `$a0`.
2. Push the result onto the stack.
3. Generate code that will compute `e2` and will put the result in the accumulator `$a0`.
4. Compare the result of `e2` (currently `$a0`) with the result of `e1` (currently at the top of the stack).
5. If they are equal jump to some label, where we’ll write the right code.
6. If not, jump to some different label.
7. Clean stuff and exit.

Also, it’s worth recalling the key concept that **every cgen function is supposed to preserve the status of $sp and of the stack**.

So, the above steps can be written as it follows.

```
cgen(if e1 = e2 then e3 else e4) = 
        cgen(e1) // template code that saves the result in $a0
                  // the next two instructions implement the “push” operation
        SW $a0 0($sp)
        ADDIU $sp $sp -4
        cgen(e2) // now result of e1 is at the top of the stack and result of e2 is in $a0
                 // to compare the two results we need to load the former in $t1
        LW $t1 4($sp)
        ADDIU $sp $sp 4 // pop the stack
        BEQ $a0 $t1 true_branch  // conditional jump
        B false_branch // if arrived here then jump in any case
```

We must now implement code that goes under the two labels `true_` and `false_branch`. They are extremely simple.

```
false_branch:
      cgen(e4)
      B end_if  // unconditional branch

true_branch:
      cgen(e3)
      B end_if
```

How can we be sure that’s all we need? Once again, we are sure because each cgen function preserves the status of the stack.

We now just miss the `end_if` part.

```
end_if:
    // there’s actually no cleaning to do here, because both cgen(e3) and cgen(e4)
    // preserve the stack
```

It’s worth noting that the bigger function `cgen(if... else...)` preserves the stack too!

## A mistake to avoid

The whole concept of using a stack, push and pop data to and from it continuously was a bit tricky to understand for me, at first. Why are we doing that? It really seems complicated and unnecessary.

For instance, many architectures, MIPS included, support the `move` instruction:

```
MV reg1 reg2
```

that simply copies the content currently in register `reg2` and put it in register `reg1`.

Couldn’t we use this instruction to implement the `sum` operation in a much cleaner way? For example:

```
cgen(e1+e2) = 
    cgen(e1) // put results in $a0 and preserve the stack - COMPILE TIME
    MV $t1 $a0  // immediately save it in $t1, without passing by the stack - RUN TIME
    cgen(e2) // COMPILE TIME
    ADD $a0 $t1 $a0  // $a0 ← e1 + e2  - RUN TIME
```

At this first sight this seems to work, and we don’t need to do all that clumsy resetting of the stack. However, **this code is wrong**.

Why’s that wrong? Consider the nested expression

```
1 + (2 + 3)
```

In this case we would first call the function `cgen(e1+e2)` with arguments `e1 = 1` and `e2 = (2+3)`. That function would first evaluate the expression `1`, and move its result onto `$t1`. Then it would need to evaluate `e2`, which means it would recursively call itself, this time with arguments `e1 = 2` and `e3 = 3`. Unfortunately, such recursive call would also use the temporary register `$t1`, and therefore the value that was moved to `$t1` in the previous call would now be overwritten, and lost.

If we use the stack instead, the sequence of push/pop operations guarantees that each recursive call only grabs its own data.

Remember this concept because it’s **fundamental to understand the whole Code Generation module**.

## Summary and Conclusions

In this lengthy article we learnt about **the final part of the compilation** of a source code: Code Generation.

Code Generation is all about creating a string that is a valid program in a different language, one that can be run by the machine. For example, if you are writing a C compiler then your Code Generation module could output a string in Intel Assembly code, that can be run on your computer.

Specifically, we learnt about stack machines and MIPS architecture, a very handy learning tool. If you want to work further on it, keep in mind that **there are many MIPS simulators available online** (see the references at the bottom).

All kinds of combinations are possible. You can have a source code in C and want to generate Assembly code, or MIPS code. Or you can have a Java source code and want to generate JVM bytecode. Or you may have a Python source code and want to generate C code, because you already have a C compiler available. The last case is very common.

Or you can do like me: I designed my own programming language because I decided that Do-It-Yourself was the best way to learn about Compilers.

1. I designed a LL(1) Context-Free Grammar, then
2. I implemented Lexical Analysis, and
3. A Parser, and 
4. A Semantic Analysis module.

Which made my project ready for Code Generation.

## What’s next

I want to be able to run code that it’s written in my Grammar. Hence, I must generate runnable code for it.

Practically speaking, generating MIPS code would not be very useful. The issue, in my opinion, is about **reproducibility**.

The first and foremost purpose of my programming language is learning about Compilers, and to build one. However, if I implemented a Code Generator module that generates MIPS or Assembly code it would probably never even looked at by someone else, and likely be forgotten forever even by myself! Too much hassle for me or anybody else to download my project, let’s say from github, and then having to install a MIPS simulator.

Don’t get me wrong though. I believe going through the effort of building a Code Generator for MIPS is an incredibly valuable learning experience, and it is in my roadmap. But I am going to prioritize a different approach.

I want **to learn very well the concept of Code Generator Templates**. In fact, this is independent of what is the “output” language. It can be MIPS or anything else, the templates’ concept is the same.

Thus, my next step is going to be Code Generation for my own language, in a different high-level language. I am yet to decide what will be the output language; so far the strongest candidates are C and Python.

## References

My article [How Does The Computer Run Code?]({{< ref "/page/how-computer-runs-code" >}}).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

My article [What is a Programming Language Grammar?]({{< ref "/page/what-is-a-programming-language-grammar" >}}).

MIPS simulators. [Option 1](http://spimsimulator.sourceforge.net/) (also used by Stanford’s Prof. Aiken), [Option 2](http://courses.missouristate.edu/KenVollmar/mars/), among many. Google it!
