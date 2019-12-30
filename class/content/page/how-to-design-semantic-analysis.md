---
title: "How to Design Semantic Analysis"
date: 2019-10-11
draft: false
---

In a previous article in this series, we learnt what is the Semantic Analysis module of a Compiler.

The key points to remember are:

- Semantic Analysis is the third step in front-end compilation (after Lexical Analysis and Parsing).
- It's meant to catch all errors that weren't caught by the first two steps.
- It's the last step in front-end compilation, so it's also the last point where a source code can be rejected.

In the first article about Semantic Analysis (see the references at the end) we saw what types of errors can still be out there after Parsing. These are the ones that Semantic Analysis must catch.

Now it's time to go deeper into the details! We need to learn how to prototype and design the Semantic Analysis module within a Compiler. In order to do so, we must understand:

- What data structures and algorithms are commonly used for these objectives.
- How symbols are handled within Semantic Analysis.
- What's typing, and why is important.
- How to handle complex language constructs, such as the ones in OOP languages.

## Data Structures in Semantic Analysis Algorithms

In my opinion, an accurate design of data structures counts for the most part of any algorithm. In different words, **your strategy may be brilliant, but if your data storage is bad the overall result will be bad too**.

Luckily, choosing a data structure is not too difficult in most cases. A few principles are worth remembering:

- Your algorithm needs to do a lot of searches? Think about hash tables.
- Your algorithm is recursive? You may need a stack, somewhere.
- Your data is naturally ordered? Consider a balanced binary tree (maybe a red-black tree).

In my experience, if you truly master Arrays, Lists, Hash Maps, Trees (of any form) and Stacks, you are well ahead of the game. If you also know a few famous algorithms on Graphs then you're definitely good to go.

Now, back to Semantic Analysis.

The first data structure we'll need is something to store information about symbols. Please be aware that I will use the words "identifier" and "variable" interchangeably. Both are examples of "symbols". To be clear, in the following line

```
int xyz = 0;
```

the string `int` is a type, the string `xyz` is the variable name, or identifier. It's also a symbol in this language. In fact, both `int` and `xyz` are symbols.

What do we need to know about symbols, when performing Semantic Analysis? Mainly, we need to know that a symbol was defined, before being used. For example, the line

```
int k = i + j;
```

is not valid unless both symbols `j` and `i` were defined earlier.

Thus, all we need to start is a data structure that allows us to check if a symbol was already defined. Historically, this is called **Symbol Table**.

Let's think about the operation that we will make most commonly on a Symbol Table:

- Each time we encounter a symbol, we **add** it to the table. Optionally, we may want to check whether that symbol was already defined. This can be a useful information to debug, such as "Warning: Variable Already Defined!".
- Every time we see an expression (that is a computation), or an assignment, or pretty much any type of statement, we must **verify that all symbols within it are well defined**.

Therefore, we understand that **insertion and search are the two most common operations** we'll make on the Symbol Table.

There are many valid solutions to the problem of **how to implement a Symbol Table**. As I said earlier, when lots of searches have to be done, a hash table is the most obvious solution (as it gives constant search time, on average).

However, we will also have to do a lot of insertion operations. Hence, an **alphabetically ordered Linked List** also comes to mind, so that we can use binary search (that's logarithmic search time) followed by insertion (that's also loogatithmic time operation, in a ordered Linked List). Clearly, if you don't care about performance at this time, then a standard Linked List would also work.

By now you have hopefully spot a problem. In modern languages, symbols are repeated all the time! What about these two methods:

```C
int foo1 (int x) {
    ...
}

void foo2 (int x) {
    ...
}
```

The symbol `x` is used here twice, and in a totally correct way. Semantic Analysis should NOT return a compilation error because of that.

What is going on here? You've probably heard the word _scope_, especially if you read my previous article on the differences between programming languages.

A scope is a subsection of the source code that has some local information. These are pieces of information that exist only in that scope. For instance, **each function/method defines its own scope**.

Furthermore, **variables declaration and symbols definition do not generate conflicts between scopes**. That is, the same symbol can be used for two totally different meanings in two distinct functions. This is how pretty much all programming languages work.

The most basic programming languages, such as experimental, configuration or DS (Domain Specific) languages lack the possibility to define multiple scopes. They have only one global scope, therefore a single Symbol Table is enough to compile them. But what about more complex languages, where multiple scopes are very commonly used?

In such cases, a single Symbol Table would not work. Why? Because the same symbol would be overwritten multiple times even if it's used in different scopes (for example, in different functions), and that's definitely not what we want.

Let's think about this issue more deeply. Actually, let's put ourselves in the Semantic Analysis' shoes, while it's scanning the source code top-down. Let's imagine we meet a function definition, or some other statement that defines a new scope. What has to be done at this point?

One thing we know for sure. This new scope will have to be terminated before the outer scope (the one that contains the new scope) is closed. For example, a class in Java defines a new scope that is inside the scope of the file (let's call it global scope, for simplicity). On the other hand, any method inside that class defines a new scope, that is inside the class scope.

Thus, a method's scope must be terminated before the class scope ends. Similarly, the class scope must be terminated before the global scope ends. However, two methods scopes are independent. More exactly, a method's scope cannot be started before the previous method scope ends (this depends on the language though; for example, Python accepts functions inside functions).

This type of reasoning should ring a bell in our head: Stack! A stack data structure works with the **Last-In First-Out** policy. It's exactly like a stack, where everything you put on the top for last, is also the first thing to be taken from the top of the stack. The operation on a Stack are called `push` (insert on top), and `pop` (extract from top).

Let's see how this would work in Semantic Analysis, with a realistic step by step example, for Python.

1. Start of the file, push on the stack the global scope.
2. Start of a class, push on the stack a new scope for this class.
3. Start of a method, push on the stack a new scope.
4. End of a method, pop from the stack. As you can see, the top of the stack is the scope that was created at step 3, so we are correctly terminating the scope for this method.
5. Start of another method, push on the stack a new scope.
6. Start of a new (nested) method, push yet again a new scope.
7. End of a method. This has got to be the method defined at step 6, so we are good simply popping from the top of the stack.
8. End of a method. This must be the method started at step 5, which now is also what's on the top of the stack, because step 7 removed the nested one. So, pop again.
9. End of the class. Guess what, this class' scope is exactly found at the top of stack. So, pop.
10. End of the file. Pop. Nicely done.

At this point, the only question left is **what are we actually pushing on the top of the stack?** What kind of data do we need there?

For sure we need a Symbol Table, because each scope must have its own. There may be need for more information, and these will depend on the language specification. Therefore, the best thing to do is to define a new class, or some type of container, and use that to save information for a scope. It will have at least a Symbol Table, but maybe more.

In other words, **we use a stack of scopes as data structure**, where each scope is a container (struct-like) with a Symbol Table, and possibly more. This data structure should support the following five operations:

- `enter_scope()`, initialize a new scope on the top of the stack.
- `exit_scope()`, terminates the current scope (pop).
- `find_symbol(x)`, searches the symbol `x` in any scope, and returns the first found.
- `add_symbol(x)`, adds the new symbol to the current top-scope.
- `check_scope(x)`, returns `True` is the symbol `x` is defined in the current top-scope.

## Before continuing...

Before going further, and getting into the intricacies of typing, I want to clarify one point.

I have been saying so far that the Semantic Analysis reads the source code from the top to the bottom. This is not true, and I said it just to ease the reading. Now it's time to clarify this point.

_Semantic Analysis analyzes the output of the Parsing stage._

The three front-end steps are like steps down a staircase. The first, Lexical Analysis, gets the output from the external word, that is the source code. It produces a sequence of Tokens. After that, the original source code is almost forgotten.

The second step, the Parser, takes the output of the first step and produces a tree-like data structure, called Parse Tree.

Thus, the third step (Semantic Analysis) gets as input the output of the Parser, precisely the Parse Tree so hardly built. **All Semantic Analysis work is done on the Parse Tree, not on the source code.**

Now we are ready to continue!

## Handling Symbols in Advanced Programming Languages (OOP)

The general strategy I described in the previous section can be summarized in the following points:

1. At the beginning of the Semantic Analysis, create a stack of scopes, and push onto it the global scope. In such scope, put all global symbols definitions, such as for types (`int`, `float`, etc.).
2. Every time a new scope starts in the Parse Tree (for example when a function starts), use the method `enter_scope()` on the stack of scopes.
3. Every time a new symbol is defined (for example in a variable declaration), use the method `add_symbol(...)` on the stack of scopes. The symbol will be added in the current scope (at the top of the stack).
4. Every time we require to check whether an expression is well-defined, use `check_scope(...)` for every symbol used in the expression.
5. Everytime a scope ends (for example, when the current node in the Parse Tree is the end of a function) use `exit_scope()` on the stack of scopes.

This is in fact a sound strategy. It's also the basic version of strategies implemented in many real compilers. Why basic? Quite simply, many adjustments have to be made to handle the specification of each particular language. That said, **these are the core principles of all Semantic Analysis algorithms**.

One of the main adjustments is about Object Oriented Programming Languages. In many (if not all) of them, class names can be used before they are defined. This clashes against the simple fact that symbols must be defined before being used.

In fact, this reasoning leads us to a very important and generic question. **How to deal with symbols that can be used before being fully defined?**

The solutions to this problem is very instructive, and that's why I am focusing on it. The trick is to **perform multiple passes over the Parse Tree**.

For example, during the first pass, Semantic Analysis would gather all classes definition, without spending time checking much, not even if it's correct. It would simply gather all class names and add those symbols to the global scope (or the appropriate scope).

The take-home message here is that **multiple passes over the Parse Tree, or over the source code, are the recommended way to handle complicated dependencies**.

Once again, the focus is on code modularity, easiness to debug and extend. To split a complex task, such as understanding class definitions, into multiple passes over the Parse Tree is a good solution.

Another common problem to solve in Semantic Analysis is how to analyze the "dot notation". This exists both in old languages and in more modern OOP languages. For example, in C the dot notation is used to access a struct elements. In Java, dot notation is used to access class members, as well as to invoke methods on objects.

In fact, this is less complicated than it may seem. When Semantic Analysis gets the first part of the expression, the one before the dot, it will already know in what context the second part has to be evaluated. What this really means is that **we must add additional information in the Symbol Table, and in the stack of Scopes**. There isn't a unique recipe for all cases, it does depend on the language specification.

Let's try to understand this better with an example. Let's use Java.

```java
class Person {
  String name;
  String lastName;
  int age;
  
  public Person(String name, String lastName, int age) {
    ... // assign objects fields
  }

  public int getAge() {
    return this.age;
  }
};

// in the main
Person p1 = new Person("Jane"', "Doe", 30);
int a = p1.age();
```

How does the Compiler know that the final line (the assignment) is well-typed?

This is an overview of what will happen during compilation (and Semantic Analysis, in this case), for that line:

1. The Compiler sees the declaration of the identifier `a`. It checks the Symbol Table for the current scope to see if this symbol was already defined. As it's not, it add it into the table.
2. The Compiler also sees that the identifier is declared to be `int`. Therefore, it adds this information in the Symbol Table.
3. It then sees that the remainder of the line is an expression. Therefore it starts evaluating it.
4. It will see that is an expression with a `.`. This is the first thing that it will see because the dot is the root of the subtree. Imagine a subtree where the dot is the root, then on the left branch we have `p1` and on the left we have `age()`.
5. It will analyze the left subtree and understand the context is about the symbol `p1`. It will search in the Symbol Table within the current scope and find it (if not found, then an error must be reported). It will also get access to all additional information that this symbol has in the Table. For example, it will know that it's an Object of type Person.
6. Then it will go search the symbol `Person`, and it will find it in some outer scope (because class names are defined in higher-level scopes).
7. It will get access to all additional information for the symbol `Person`. For example, it will know that another symbol `age` is defined for all `Person` objects, and that is a method without argument that has return type `int`.
8. It will conclude that the code is correctly typed!

## How Semantic Analysis Checks Types

The other big task of Semantic Analysis is about ensuring types were used correctly by whoever wrote the source code. Used, and understood correctly, I dare to say. In this respect, modern and "easy-to-learn" languages such as Python, Javascript, R really do no help. Let me tell you more about this point, starting with clarifying what such languages have different from the more robust ones.

### Static vs Dynamic Typing

You've certainly heard about scripting languages. What are they? What does it mean to write a script? In fact, there's no exact definition of it, but in most cases a script is a software program written to be executed in a special run-time environment.

Some examples: Javascript programs are written to be run in the web browser (well, before NodeJS was created); Python programs are written to be run in the Python interpreter; Bash programs are written to be run in the Unix shell environment.

Pretty much always, scripting languages are **interpreted, instead of compiled**. And this simple fact makes all the difference from our point of view. Generally, a language is interpreted when it's lines of code are run into a special environment without being translated into code machine.

Some examples, again: C is a compiled language; So are C++ and Java. Perl, instead, is an interpreted language. The same holds true for Javascript and Python.

Unfortunately, things are not so much black and white. It turns out most programming languages are both interpreted and compiled. The classic example is Java. A Java source code is **first compiled**, but not into machine code, rather into a special code called bytecode, which is **then interpreted** by a special interpreter program, famously known as Java Virtual Machine.

I've already written a lot about compiled versus interpreted languages, in a previous article. Let's go back to typing, instead.

The reason why I said above that types have to be "understood" is because many programming languages, in particular interpreted languages, totally hide the types specification from the eyes of the developer. This often results in misunderstanding and, unavoidably, low-quality code.

I really don't mean to be frigid or overly critical, but let me give you one example. I've often seen code like this written in Python

```python
# WARNING: This is code is not well written
def foo(arg1):
    if arg1:
        return [1,2,3]  # list
    else:
        return -1  # integer
```

Now, this code may be correct, may do what you want, may be fast to type, and can be a lot of other nice things. But why on earth your function sometimes returns a List type, and other times returns an Integer type?! You're leaving your "customer", that is whoever would like to use your code, dealing with all issues generated by not knowing the type.

In the good old days, functions used to have a complete signature. Not just a `def` keyword, but a complete signature such as:

```
<return-type> <function-name> ( <list of arguments with types> )
```

For example, the previous Python example would be written somehow like the following:

```
list foo (boolean arg1)
```

and you would know for sure that the function named `foo` returns a `List` type, and receives as input only one object, and that has got to be a `boolean`.

Why do I like so much signatures? Because in compiled languages, the Compiler would give an error if the implementation didn't match the signature. So for example, if you did something like this

```python
list foo (boolean arg1):
    if arg1:
        return [1,2,3]
    else: 
        return -1
```

your code would not compile! There would not be any stupid runtime crash due to this bad code, because the Compiler wouldn't let you proceed further. You would stop during compilation, with a message like the following (this one is from the Java Compiler):

```
error: incompatible types: int cannot be converted to int[]
		return -1;
```

At some point in Computer Science history, someone thought to remove typing because _it allows faster prototyping_ (??!).

I have quite a harsh opinion on this point. What does that even mean? Does writing weak and possibly buggy code allow faster prototyping? I really don't think so.

And I am not the only one to think that. In fact, starting from the version 3.5, with the Python programming language you can use something very similar to signature (see the references at the end)! The official name is _Type Hints_, and you would write this way:

```python
def foo (arg1: bool) -> list :
    if arg1:
        return [1,2,3]
    else: 
        return -1
```

I like that. A great step in the good direction.

In my opinion, **programming languages should be designed as to encourage to write good and high-quality code**, not just some code that maybe works.

Now, to tell you the full story, Python still is an interpreted language, so there's no compiler which would generate an error for the above function. But I believe many IDE would at least show a red warning, and that's already something.

At this point it should be pretty clear that I like **static typing**! Static typing, roughly said, is just another way of saying typing that is checked by the Compiler, **before the program is run**.

Basically, the Compiler can know the type of each object just by looking at the source code. The other side of the coin is **dynamic typing**, when the type of an object is fully known only at runtime.

Even though I like static typing, I must say it has some drawbacks. In particular, it's clear that **static typing imposes very strict constraints** and therefore some program that would in fact run correctly is disabled by the compiler before it's run. In simpler terms, programs that are not correctly typed don't even get a chance to prove they are good during runtime! They are aborted long before that (during Semantic Analysis, in fact!).

### Before continuing...

I have already been talking about typing and types for a little while. And yet I didn't answer the most obvious question: **what is a type?**

Such question does not have a universally accepted answer. It may change from one language to another. Generally speaking though, the most common answer is that a type is:

- A set of values, along with
- A set of operations that are allowed on those values.

Let me give an example to clarify.

In many languages, `int` is a type. That's fine with the definition above, because the set of values is clearly defined (for example, from `-2^31` to `2^31 - 1`) as well as the valid operations on them. Most commonly, addition, subtraction, multiplication, division, modulus, but there might be others depending on the language. For example, Python does not define increment and decrement, while Java, C and C++ do, with the operands `++` and `--`.

It's worth noting that the second point in the definition, about the set of valid operation, is extremely important. And it's actually even trickier to check for Semantic Analysis. Let's jump back straight into that.

## Understanding Types in Semantic Analysis

We should now have a pretty good understanding of what a type is, and what is the difference between a static and a dynamic type. The question is, **how does Semantic Analysis understand all of this?**

After I studied these concepts, I now believe that the overall concept can be split into two subtasks to accomplish:

1. Understanding the type of an expression.
2. Understand the type of a declaration.

Arguably, a declaration can be an assignment, and in such case an expression is assigned to a variable. If that's the case, then point 2 collapses onto point 1. Let's talk about them separately anyway, starting by the easiest: declarations.

In statically typed languages, declaration are no-tasks for Semantic Analysis: the type is declared before the variable name, such as in

```c
int x = 0;
```

which is a valid line in a lot of languages, for instance C, Java and C++. In this case, Semantic Analysis doesn't need to do complicated stuff:

- To handle the Symbol `x`. Probably, check in the Symbol Table, within the current scope, if that Symbol already exists. In many languages you cannot declare the same symbol twice. Then, if it's all good, add that Symbol into the Table.
- Simply assign the type int to the Symbol `x`. There's really nothing else to decide here, because that's what the source code says.
- Check that the right-hand side of the declaration, if present, has the same type (`int`, in this case). This boils down to understanding expressions type; I'll talk about it in a minute.

Overall, this is a fairly easy task. The scenario becomes more interesting if the language is not explicitly typed. Consider the following line of valid Python code.

```python
x = 3 + 5 >= 7
```

How should we change the steps above, in this case? Let's see:

1. To handle the Symbol `x`. I don't know any non-typed language that forbids to declare the same symbol twice. So, in this case you would still need to check the Symbol Table and, if `x` was already defined, override the definition in the current scope. I would also print a warning message at screen!
2. Now, we can't actually assign any type to the Symbol `x`. We must first analyze the remainder of the line, and figure out what type `x` is. This procedure is called **type inference**.

I find type inference very interesting, and this is why **I designed a new programming language that is not explicitly typed**: mainly because I wanted to practice types inference in Semantic Analysis and Compilation!

Type inference is best shown when we have to **figure out the type of a complex expression** (the original point 1 of this discussion), so let's get to it.

To infer the type of an expression we have to master the algorithm to walk down the Parse Tree. I explained to them in detail in a previous article, so I can only recommend you to read that, or to google for the following algorithm: "deep-first search", "tree visit".

Why do we need to walk through the Parse Tree? The reason is that, no matter how complex an expression is, **the real content is in the leaves of the tree**. For example, consider the following expression:

```
x = ((1 + 4) > y) or (5 + (3 * z) % 7 < 3)
```

The subtree that stores this expression is likely to be quite large. Still, the nodes that are at the deepest level (leaves) will contain the Tokens

```
<1, int>
<4, int>
<y, identifier>
<5, int>
...
```

Thus, when we get down to a leaf, for example `<1, int>`, that's the leftmost leaf in the tree, it takes nothing to infer that this token's type is `int`.

The most difficult case for a leaf is when it is an identifier. In such scenario, we must look up in the Symbol Table for the current scope, and get the type of the symbol from there. If the identifier is not in the Symbol Table, then we should reject the code and display an error, such as `Undefined Variable`.

When we've done that for all leaves, we start walking backwards in the tree. In other words, we uplift by one level the types inferred at the last level in the tree. In the second to last level we should meet all operators Tokens. For the previous example these would be

```
<'+', operator>
<'>', conditionalOperator>
<'or', logicalOperator>
...
```

Each operator, by definition itself, it's applied to a given number of operands. Most commonly two operands. Given that the operands are the types that we computed at the leafs, how can we compute now the type resulting from the application of each operator?

There's really nothing magic here. We simply must check for each operator, and for each type of the first operand type, and for each type of the second operand, what's the result type.

This can be realized, for instance, with a cascade of if-then-else. Something like the following snippet:

```
if (operator is '+')
    if (operand-1 is int)
        if (operand-2 is int)
            return "int";
        else if (operand-2 is double)
            return "double";
    // and so on...
```

However, this would be a pretty stupid way to implement it. Tedious and slow too. I believe the best way is with a set of **lookup tables**. We would create a look-up table for each operator. The columns of these tables are the possible types for the first operand, and the rows for the second operand. If the operator works with more than two operands, we would simply use a multi-dimensional array.

For example, the result types of the '+' operator would be given by the following table:

```C
int plusOperatorTypes[6][6] = {
    /*               int        float    string     bool    null     list   */
    /* int    */   { _int,     _float,  _undef,   _undef,  _undef,  _undef},
    /* float  */   { _float,   _float,  _undef,   _undef,  _undef,  _undef},
    /* string */   { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* bool   */   { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* null   */   { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* list   */   { _undef,   _undef,  _undef,   _undef,  _undef,  _undef}
}
```

This snippet was taken from the Semantic Analysis implementation in the Compiler for my own programming language.

The Grammar I designed defines as basic types `int`, `float`, `null`, `string`, `bool` and `list`. I am using symbolic names, implemented like an `enum` object, but with integer values to easily access the lookup table. For instance, `_int`, to refer to the integer type.

Let's take a look at one example, to clarify things.

We could use the look-up table when we are to decide the type of the following expression:

```python
x = 4 + true
```

The type for the token `4` is `_int` (that's implemented as `0`), while the type for the token `true` is `_bool` (that's `3`). Thus, the **fast lighting lookup operation**

```C
plusOperatorType[_int][_bool]  // equal to plusOperatorType[0][3]
```

would give the result type of the operation `(4 + true)`. In this example, the result is `_undef`, a special value that I used to say the operation is not allowed.

If the lookup operation says that the operation is not allowed, then again we should reject the source code and give an error message as clear as possible.

When we have done that for all operators at the second to last level in the Parse Tree, we simply have **to repeat the procedure recursively**. Uplift the newly computed types to the above level in the tree, and compute again types.

This has to be done over and over until we reach the root of the subtree for the expression. At that point, we will have the result type of the overall expression, or an error code if the expression wasn't typed correctly. That's it!

### Extensions to static typing check systems

You can easily imagine what a debate has taken place, over many years, between sustainers of static typing on one side, and supporters of dynamic typing on the other. As of today, in my opinion, **there's no winner**.

The main criticism moved to statically enforced typing is that many source code may be correct at runtime, but the compiler kills them before they are ever run. Why is that?

The thing is that source code can get very tricky, especially when the developer plays with high-level semantic constructs, such as the ones available in OOP. For example, let's take a look at the following Java snippet.

```java
class A {
  ... // something
}

class B extends A {
 ... // something more than A
}

class MainClass {
  public static void main (String[] args) {
    A a1;  // from now on, the static type of the identifier a1 will always be A
    a1 = new B();  // this should be allowed according to inheritance rules
                   // (and indeed is valid, in Java)
  }
}
```

The code above is a classic example that **highlights the difference between the static and dynamic types**, of the same identifier.

When the variable `a1` is first declared, the compiler must add it into the Symbol Table and assign the type `A` to it. Thus, the type `A` will be the static type of the identifier `a1` for the rest of the program.

However, according to the inheritance rule _is-a_, the assignment `a1 = new B();` is valid.

How to use the is-a rule? It's very simple, just answer the following question: Is `new B()` an `A` object? Yes, it is, because the class `B` extends the class `A`. Therefore, the assignment is valid. And if we are building an OOP, robust, programming language, then our compiler has to understand all of what I just said.

Another problem that static typing carries with itself is about the type assigned to an object when a method is invoked on it. Let me clarify with an example, again, in Java.

```java
class A {
  public A method1 () {
    ... // some operation
    return this;
  }
}
```

This type of code where the object itself is returned is actually quite common, for example in many API calls, or in the Builder Design Pattern (see the references at the end).

Let's now say that we want to extend class A.

```
class B extends A {
   ... // something here, without overriding method1
}
```

So far so good. Now, let's say we want to initialize a `B` object using the builder method1. What we should (and would like to) do is:

```
B bObject = (new B()).method1();
```

This is a conceptually correct line of code. We instantiate a bare-bone `B` object, using the normal `new B()`, and then call the `method1` on it, because we know it will do some operations and then return this.

If you start being a bit confused, then don't worry. This stuff is tricky! Just take your time and read slowly.

The point is that line of code give a compilation error! Why is that? The problem lies in the fact that **the return type of `method1` is declared to be A**. And even though we can assign a `B` object to a variable of type `A`, the other way around is not true.

In other words, while a `B` object _is-a_ `A` object, the inverse is not true. Therefore, the compiler will see a wrongly-typed assignment, and will reject the source code. What a setback!

To solve this specific issue, some languages define "self-type". Practically speaking, this is a keyword (e.g., `SELF_TYPE`). What's that? The important thing to know is that self-type is a static concept, NOT dynamic, which means the compiler knows how to handle it.

Unfortunately Java does not support self-type, but let's assume for a moment it does, and let's see how to rewrite the previous method.

```java
// inside class A
public SELF_TYPE method1() {
  ... // some operation;
  return this;
}
```

With that, a Java Compiler modified to handle `SELF_TYPE` would know that the return type of `method1` is-a `A` object. And although this is a static check, it practically means that at runtime it can be any subtype of `A`.

Thus, the line that previously gave compilation error

```java
B bObject = (new B()).method1();
```

would now pass all checks, including Semantic Analysis' ones.

Why?

The compiler will see that the return type of `method1` is `SELF_TYPE`, then it will see the dot notation `<object>.<method>`, in this case `(new B()).method1()`, and will understand that in this context `SELF_TYPE = B`. So, everything will go fine.

## Summing up

So far we have seen in detail static and dynamic typing, as well as self-type. These are **just two examples, among many**, of what extensions have been made over the years to static typing check systems.

To complicate things further, there's a great deal of other, creative, things that happen in modern languages. I can't possibly mention all of them, and even if I did the list would become incomplete in a day.

Thus, to wrap up this article, I just want to give a partial list of things that have been tried in one or more programming languages. It will look like a random list of words, but you may recognize some names, and I warmly recommend you to do your own research about them (Wikipedia is a good starting point).

- Generics.
- Self-Type.
- Agent Oriented Programming (the new frontier of OOP).
- Point-Free Style.
- Metaclasses (Read [this amazing StackOverflow answer](https://stackoverflow.com/a/6581949)).
- Asynchronous Programming.

## What's next

In this article, I went through a lot of advanced content. It wasn't easy for me at first place to study it, and I do have a good background in Computer Science, so don't worry if you feel overwhelmed.

What I want to do next is to avoid leaving all these concepts lost in the wind. This is what I always try to avoid in my self-assigned classes. I found that the best way to do so is to assign myself a real, and quite complex project. Not at the industrial-strength level, but far more advanced than the typical MOOC assignments.

If you have seen my previous articles then you know that for this class about Compilers I decided to build a new programming language. It's not too fancy, but I am building it from the ground, and without using any automatic tool.

I have already designed the Grammar (by hand), implemented the Lexical Analysis (by hand, in C) and implemented the Parser (by hand, in C). Therefore, I now want to implement all those concepts that I learnt about Semantic Analysis and that apply to my language. And will tell you about it in the next article!

## References

Language Implementation Patterns. A book by Prof. [Terrence Parr](https://parrt.cs.usfca.edu/).

My article [Why There Are So Many Programming Languages?]({{< ref "/page/why-so-many-programming-languages" >}}). (Talks about front-end, back-end compilation).

My article [How to Design A Programming Language Parser]({{< ref "/page/how-to-design-a-parser" >}}) (If you don't know yet what a Parse Tree is).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

Wikipedia page about the [Builder Software Design Pattern](https://en.wikipedia.org/wiki/Builder_pattern).

Python PEP 0484 about Type Hints, <https://www.python.org/dev/peps/pep-0484>.
