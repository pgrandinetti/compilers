---
title: "Implementation of Semantic Analysis"
date: 2019-10-13
draft: false
---

This article is part of my course on Compilers, Formal Grammars and Programming Languages. Up to this point we have studied:

- What is a Compiler.
- What are the several stages of the compilation process.
- What is a Formal Grammar, and several existing types of Grammars.
- A real design example of a new Context-Free Grammar, for a new Turing-complete programming language.
- What is Parsing, the second stage of compilation.
- A real implementation of a Recursive Descent Parser, made for the new programming language mentioned above.
- What is Semantic Analysis, the third stage of compilation.

You'll find, roughly speaking, one or two articles for each of the topics above. In the course's content you can also find other articles meant to explore related subjects and provide a better, deeper understanding of the whole theme. Check the references!

In this article, I will show and discuss my personal implementation of a Semantic Analysis module.

As you already know, to complement my theoretical studies, I decided to build a new (simple) programming language, along with its compiler. The Semantic Analysis is the third module of this Compiler, and the last of the front-end part.

## Background

For the sake of understanding this article, it's worth reviewing what we've already learnt about Semantic Analysis. In brief:

- It's the third step of the Compiler, the last one of the font-end part.
- It takes care of all errors that went unnoticed through the first two steps (Lexical Analysis and Parsing).
- It's the last one among the steps that are meant "to reject" ill-typed codes. After Semantic Analysis, we assume the code is correct, although it might not do what the developer meant!.
- The most important things it takes care of are: symbols definitions, scopes, typing.

In Lexical Analysis (as well as in Parsing), some large part of the code can be realized with standard algorithms. This is why there exist tools, such as ANTLR, that implement them almost automatically. As said before, **I didn't use any of such tools**, because I think it's worth doing it "by hand" once in a lifetime, for the sake of learning.

The beautifully complex thing about Semantic Analysis is that most of the things it does depends on the given language specification. There are several pillars that all Semantic Analysis implementations are based on, and I discussed them in a previous article. In this one, **we are going to see the implementation I made of these concepts**, specific for the language I designed.

## Grammar and Language review

Before going further, I think it's worth taking a minute and review the peculiarities of the Grammar I designed, and of the Language I am building. Given the nature of what Semantic Analysis has to do, is very important to understand the key concepts of the Language.

The language I designed:

- Is extremely **explicit**. There are almost no "implicit" instructions or assignments. I believe this encourages the developer to think more about what she writes.
- Is also **compact**. Although things are explicit, you don't need to type a lot on the keyboard. The main reason for this is because types are inferred by the compiler. Yes, there's something "implicit" after all.
- It's **compiled**. Of course!
- It's **statically typed**. I discussed static and dynamic typing in vast detail in a previous article. Basically, the compiler will also check that each instruction is correct from the point of view of the types. This is what C, Java, C++ do (but types are explicit there), and what Javascript, Python, Perl do not.
- It support as **basic types** int, float, boolean, string and list.
- It's **Turing-complete**. Yes, I've got you covered with a conditional instruction (if) and a loop (while), as well as I/O instructions.
- It's designed as a **learning tool and benchmark**. As I said, a fairly advanced assignment to complement my studies.
- **Logical lines are terminated with Physical lines**. In practice, this means that the developer is forced to use two given characters anytime a statement is completed. These are semicolon and newline `';\n'`, which means two logical statements can never be on the same physical line.
- **It lacks advanced features**, such as OOP, routines, etc. which of course are very valuable additions. They are on my road map, but for the pure objective of learning the compilation process, I decided to postpone their design and implementation.

How does a source code look like, in this language? In fact, I am quite satisfied with many design choices I made. The result is **a language that looks like a scripting language, but it's compiled and statically typed**. As already said, the code must be monolithic, as routines (methods, functions) are not allowed for the moment.

The easiest is to look at an example. I am going to repeat one that I've already shown in a previous article: given an integer $N$ (to be entered by whoever runs the code), compute the sum of all prime numbers that exist in the interval $[2, N]$. This time though, I will add some comments to highlight the semantic of the code, not just the syntax.

```
readInt N;  // read an integer from standard input, save it in the variable N
totSum = 0;  // totSum will be automatically assigned type int, inferred by the compiler
i = 0;
while (i <= N)  // i and N are both integer, so the operator '<=' is well defined
    j = 2;  // j is assigned integer type and value 2
    isPrime = True; // isPrime is assigned bool type and value True
    while ((j <= i / 2) && isPrime) // the two sub-expressions have both boolean type
        if (i % j == 0)
            writeOut "%d is not prime", i;  // write on standard output
            isPrime = False;
        ;
        j = j + 1;  // assignment, the left-hand side is an integer, so it's the right-hand side
    ; // this is the end of the logical line started with "while"
    if (isPrime)
        totSum = totSum + i; // totSum+1 has type int (inferred) so can be assigned to the
                                         // variable totSum
    ;
    i = i + 1;
;
writeOut "Total primes sum until %d is %d", N, totSum;
```

## Tasks for Semantic Analysis

Let's now discuss what tasks Semantic Analysis should accomplish for this language. When I arrived at this point, I literally asked myself the question - **what errors can still be uncaught?**

### Assignment Statements

I started off by thinking about the assignment. After all, assignment statements are some of the most common instructions in a source code, of any language. I went back to the Grammar definitions and found it:

```
assign      :   var '=' expr
```

I knew that the Parser would take care of checking most issues there:

- It would check that the first Token really is a `var` Token.
- Then, it would check there is an Equal Symbol (that's a Token too).
- It would also check that after the Equal there is a well formed `expr`, made by a potentially very long sequence of Tokens, or even by just one Token.

What's left? There are a few things Semantic Analysis has to do, still.

1. It must understand what is the type of the whole `expr` Token.
2. If the Symbol contained in the `var` Token (also called lexeme) was never defined before, then it must be added into the Symbol Table. To know what such a Table is, I recommend checking my previous articles (see the references at the end of this one).
    1. It should also assign to this Symbol the type computed at step 1. This information must be saved somewhere in memory (somewhere linked to the Symbol Table, of course).
3. If the Symbol in Var was already defined then things are quite a bit different.
    1. Optionally display a warning that a variable has been overwritten. I would not do this at this stage though (and I will explain why in a minute).
    2. Check what was the previous type assigned to such Symbol, in the Symbol Table.
    3. If the type found in 3.2 is not the same as the type computed at step 1, then two options are possible:
        1. Reject the code with a compilation error.
        2. Overwrite the type of that Symbol, in the Symbol Table. In this case a warning message is also opportune, in my opinion.

The point I discuss in 3.3 is very important. In fact, I initially decided to go with the option 3.3.2, because I wanted to give the developer the possibility to modify the type of a variable.

However, **I later changed my mind** and went for 3.3.1. That means that now you cannot modify the type of a variable after it's defined the first time, in my language. This is also what happens in C and Java, for instance. Why did I do that? The problem is that option 3.3.2 clashes with my ideas on how to handle List objects, as we'll see in a minute.

Overall, this task is not very complex, assuming that the first step (understanding the type of a whole Expression) is already solved. Before talking about Expressions though, I want to take you through small digression.

### Pseudo-code to descend the Parse Tree

The concept I want to discuss is well shown in the following pseudo-code:

```c
function analyzeNode (Node n, SymbolTable table, Scope scope) {
    preprocessNode(table, scope, n);
    analyzeNode(n.child);
    postprocessNode(table, scope, n);
}
```

This code seems not doing anything at all, but in fact it shows the core principle of how to execute Semantic Analysis in complex cases such as the Expression.

First of all, remember that when we want to analyze some complex structure what we really have access to is the root node of the subtree, within the Parse Tree, that was created by the Parser.

We therefore start the analysis at such node. As the code perfectly clarifies, we "just" do three things: some pre-processing for this node, the analysis of the child, and some post-processing.

The key here is that **the function `analyzeNode` is recursive**, of course. By calling it over and over for each child, we will eventually arrive at the leaves of the tree. Leaves are very simple Token Types, therefore understanding their semantic is extremely easy. Moreover, at the leafs there's usually no need to pre-process or post-process anything.

Let's look at one example that clarifies everything.

A leaf can be, for instance, a `num` Token, which is either a `float` or an `int`. I want to show you the code I used to analyze a `num` node in my Semantic Analysis. Although you don't have yet the full picture, its simplicity will be evident.

```c
int analyze_Num (struct ParseTree *tree) {
    struct ParseTree *num;

    num = tree->child; // the Float node
    if (num->data->type != Float)
        // it's the sign, skip it
        num = num->sibling;
    if (num->child->data->type != Int)
        return _float;
    if (num->child->sibling != NULL)
        // then there is either frac or pow
        return _float;
    return _int;
}
```

All I do in the code above is to check the shape of the subtree that starts at the `num` node (the pointer given as argument to the function), and return the type of it.

The literals `_int` and `_float` are constants that I defined (with the `#define` instruction in C). `parseTree`, `floa`, `int` are all struct definitions or Token Types I defined in the Lexical Analyzer or in the Parser.

How did I know the structure of the subtree? I simply looked at the Grammar.

```
num      :   ('+' | '-')? float
float    :   (frac exp?) | (int frac? exp?)
int      :   '0' | digit+ digit0*
```

If you carefully analyze that portion of the Grammar side by side with the `analyze_Num` function, you'll see things more clearly.

Now, imagine that we are analyzing an Expression. We'd keep initiating recursive calls following the pre-process, analyze-child, post-process strategy. But when we arrive to a Num node, **the recursion will stop** because the function `analyze_Num` will simply return the type of the Num subtree.

You are probably thinking - wait! The Num subtree is not a leaf, why aren't we descending further with more recursive calls?

Even though you are formally right, there's no reason to **pile up unnecessary recursive calls**. Even though `num` is not formally a leaf, it's simple enough that we can analyze it directly.

This is all about "analyzing" a Node: either call the same function recursively on its child (or children), or just return the type of that node. The latter works for simple nodes.

What about the pre-process and post-process functions? The actual steps you have to take in these functions depends on the type of Node you are currently analyzing, as well as the overall language specification. Here's some things that could be required:

- Push information into the stack of Scopes (pre-process).
- Pop information from the stack of Scopes (post-process).
- Push information about the Context (pre-process).
- Pop information about the Context (post-process).
- Add a new Symbol in the Symbol Table (pre-process, usually).

In fact, there are many other things that can potentially be done. I can't really offer a universal recipe about this, but I will show you what I did in my Semantic Analysis implementation.

For the moment being, I want to give one example that would help clarify. My Grammar supports the `continue` and `break` instructions. Their semantic is the same as in many other languages: to exit from the innermost loop, or conditional (`if`) statement.

There's a catch with these instructions. They can only appear inside a loop or a conditional block. For example, the following code is correct

```
while (x <= 5)
    writeOut "OK";
    break;
;
```

Whereas the following one isn't, and should be rejected.

```
while (x <= 5)
    writeOut "OK";
;
break;
```

This is a perfect example of information that can be pushed onto a stack in the pre-processing stage of a Node, before going further down in the recursion. In my case, I handle these situations by simply pushing a flag that says the current Context allows for break/continue instructions. Then, I pop this flag from the Context in the post-processing stage.

Let's go back to the analysis of the most interesting piece of Grammar: the Expressions.

### Expressions

In my programming language Expressions have a complex, recursive definition very typical of _Context-Free Grammars_. I have explained it in great detail in my article about the Grammar (again, see the references at the end).

What has Semantic Analysis got to do about them?

- It must check that every Symbol used within the expression was defined earlier in the code.
- It must check that every operator in the expression is used with compatible types.
- It must compute the final return type of the entire expression.

Let's go with three examples, one for each point above.

```
x = 3;
y = x + z;
```

Clearly, the two lines above should not compile, because the symbol `z`, used in the expression to assign a value to `y`, was never defined.

```
x = 3;
z = "abc";
y = x + z;
```

The three lines above should also generate a compilation error. The reason is that the operator `+` is used with a `int` type (`x`) and a `string` type (`z`). Even though this kind of operation may be allowed in some languages, the semantic of my language forbids it. It was my choice, of course.

```
x = 3;
z = "abc";
y = z + "%d", x;
```

These three lines should be compiled correctly. No error should be reported. Moreover, the Symbol `y` should be assigned type `string`. Why? Because Semantic Analysis must be able to understand that the result of the expression

```
z + "%d", x
```

is a string. This, of course, under the condition that the operator `+` is well defined between two strings.

How should we go about implementing that? Here's where the previous digression about pseudo-code and analysis strategy kicks in.

Every expression is analyzed with multiple recursive calls, that descend into the Parse Tree until a leaf, or a simple subtree is found. In my code, some examples of these are:

- The Num Token, as we already saw.
- The Var Token. In this case, the string literal of the Token (formally called lexeme) is searched in the Symbol Table. If found, its type is returned. If not found, an error is reported.
- Int, String, Bool, Null, Float and ListElem Tokens. Notice that Int and Float are actually always nested into Num.

Thus, **all recursive calls will eventually stop** when any of those Token Types is found. Then, the result will be returned by the function that analyzed them. You can imagine the procedure as to descend into the Parse Tree first, and then climb up to higher levels carrying the results computed at the bottom.

While climbing up and carrying the results from the lower levels, all sorts of checks are performed. For example, consider the simple expression

```
3 + "abc"
```

At the bottom of the Parse Tree the recursive call would stop:

- On the left side of the tree, when the Num subtree that contains the `int` Token with value `3` it's met. In this case, the type `_int` will be returned.
- On the right side of the tree, when the String subtree that contains `"abc"` is found. Here, the `_string` type will be returned.

These two results will be carried over while climbing up the tree. The algorithm will then meet the Node that contains the operator `+`. Here, it will check if such operator is valid. In this specific example, an error message will be generated because the `+` is not defined (in my language) between int and string types. The source code that contains that expression will then be rejected.

No matter how complex the Expression is, or, that's the same, how many nested levels there are in the subtree for this Expression. The general strategy does not change by an inch.

### Lists

List is a data structure that I really wanted to add in my programming language. First of all, because containers help to write readable source code. Secondly, because I find the notation à la Python quite beautiful and handy. Thus, my language supports statements such as

```python
x = [1, 2, 3]
```

However, the semantic Lists have in my language is more similar to the typed arrays in C and Java, than to actual Python lists. Let me explain better.

In my professional experience, where I use Python a lot, I've seen that many developers use lists (and tuples) with mixed types. For example:

```python
x = [1, "string", {'a': 'dict'}]
```

Now, even though this is valid in Python, and perhaps in some cases can lead to more compact code, and everything works nicely in your code, **I discourage this type of code**. In my humble opinion, it's bad practice to mix different types in the same container.

This is why I decided to forbid that in my language. If you try to do it, the Compiler will reject your code!

Who's taking care of checking it? Semantic Analysis, of course! Here's how it works.

First, when the root of the subtree containing a List is found, Semantic Analysis skips the initial bracket `[`. Why? We know it's there, because the Parser already checked it.

Then it comes the actual list content. Now, thanks to the Grammar we know that objects in the list are comma-separated.

```
list        :   '[' (listExpr)? ']'
listExpr    :   obj (',' obj)*
```

Thus, the function that analyze the listExpr subtree will analyze object by object, skipping the commas. What does "to analyze a obj Token" consist of? Let's look at its definition

```
obj         :   var | num | str | bool | null | list | listElem
```

Therefore, we would simply switch among those token types. Notice that they are all very simple Token Types, precisely those types that do not require recursive calls.

After each object in the `listExpr` has been analyzed, the Semantic Analysis checks that all of them have the same type:

- If not, then report an error and reject the source code.
- If yes, then save this information somewhere linked from the Symbol Table. The reason is that we must know what type of objects a list contains. This is actually how the type for the basic Token listElem is found.

Let me repeat again the second point above. Say we are analyzing the following expression

```
x = [1, 3, 6]
```

When the list on the right-hand side will be analyzed, we know that it's a valid list of integers. Thus, we must save two information in the Symbol Table (and in the current scope):

- The Symbol `x` refers to a `list` type.
- The list referred by `x` contains all `int` types.

Why is point 2 important? Because in future parts of the code we could find a `listElem` Token, for example

```
y = 3 + x[1]
```

And we must have a way to check what's the type of the elements inside the list `x`, so we can decide whether `3 + x[1]` is a well-typed Expression or not.

At this point it should also be clear why I decided to forbid to change a variable type after it's defined the first time. If that was allowed, then it would be easy to mess up with containers, for example

```
x = 3; # x is int
y = 5; # y is int too
z = [x, y]; # z is a list of int
x = "ooops"; # a string! Now the list is mixed up.
```

Similar case will be handled by Semantic Analysis, which will reject the code. Note, however, that I can't stop you from writing weird code, if you really want: the following code is valid, but strongly discouraged.

```python
x = [1, 2]; # list of integers
y = ["a", "b"]; # list of strings
z = [x, y]; # list of lists, where inner lists have different types
```

However, since I don't support double indexing in the Grammar (such as `z[0][0]`), this should have limited effects. At any rate, just don't do it!

### Basic Operations

The type of checkings Semantic Analysis will do most often is the basic check about correct usage of an operator. For example, when it finds the subtree relative to code such as

```
if (x + 3 <= y)
```

it will have to decide (among many other things) whether the operations `+` and `<=` are valid.

Why did I say this is the most frequent type of check? Simply because Expression is the most frequent Token Type you will find in computer languages, not only in mine. Here's a partial lists of where it's used:

- Assignments (`var = expr`)
- Conditionals (`if (expression) do something`)
- Loops (same logic as conditionals)

As we have seen in the previous subsection, the semantic evaluation of an Expression eventually boils down to two steps:

1. Understand types of basic objects involved (`x`, `3`, and `y` in the previous example).
2. Decide if the operators between these objects are well used. For example, you cannot use `+` between a `int` type and a `string` type, in my language.

And this is why to check basic operations is a task that Semantic Analysis will perform a lot.

How to go about it? Strictly speaking, this is an easy type of reasoning. All we have to do is to check, for each operator, if the two operands have types that can be used with that operator. Theoretically, this corresponds to a nested sequence of if/then/else.

```
if (operator == '+')
    if (operand1.type == "int")
        if (operand2.type == "int")
            return VALID;
        if (operand2.type == "float")
            return VALID;
        if (operand2.type == "string")
            return FAIL;
// many more...
```

However, this type of implementation is quite silly:

- It's slow. Every time we want to check an operation we have to go through the evaluation all these nested conditions.
- It's **verbose and error-prone**, as every bad code ever written.

The much better solution, perhaps the best, is to **use lookup tables**. The idea is to have a lookup table for each operator, implemented as a 2-dimensional array. The row and column index correspond to the type of operand1 and operand2.

I am going to illustrate this much more in detail in the next section, for now let's just grasp the idea. Here's a snippet from my code.

```C
#define _int 0
#define _float 1
#define _string 2
#define _bool 3
#define _null 4
#define _list 5
#define _undef -100

// ... 

int plusOperatorLookup[6][6] = {
    /*                 int       float    string    bool     null    list   */
    /* int    */    { _int,     _float,  _undef,   _undef,  _undef,  _undef},
    /* float  */    { _float,   _float,  _undef,   _undef,  _undef,  _undef},
    /* string */    { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* bool   */    { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* null   */    { _undef,   _undef,  _undef,   _undef,  _undef,  _undef},
    /* list   */    { _undef,   _undef,  _undef,   _undef,  _undef,  _undef}
}

```

To see how this works, let's get back to the simplest example of expression

```
y = x + 2;
```

We already know that Semantic Analysis will evaluate this assignment statement by evaluating the right-hand side (after the `=`) sign, which is an expression.

It will walk down to the leafs of the Parse Tree, via recursive calls. The branching will stop in two places:

- When it finds the `var` Token, for `x`, it will use a function probably named `analyze_Var`. Such function does not start any recursive call, because to analyze a `var` Token is a simple enough task, as we saw earlier. Let's assume that `x` was already defined in the Symbol Table (so the code is correct), and it's type is `_float`.
- When it finds the `num` Token, for `2`. We already know what happens in the function `analyze_Num`. In this case, it will return `_int`.

Now that we have got the two results `_float` and `_int`, the magic happens. It will simply call

```
int result_type = plusOperatorLookup[_float][_int];
```

Notice that `_float` and `_int` are defined as integer value corresponding to the correct indexes of the lookup table! A small, but very useful trick. The above operation is **very, very fast**.

Furthermore, notice that the lookup table gives one more information, not just whether the operation is valid or not. The lookup table tells us what the result type is.

For example `plusOperatorLookup[_float][_int]` gives the type `_float`. In fact, if you do `3.2 + 4` you want the result to be stored in a floating point object (`7.2`, hopefully!).

Finally, notice also that I defined one more constant value, `_undef`, that is returned when the operation is not valid. For example, we would get `_undef` from `plusOperatorLookup[_null][_int]`, because operations such as `3 + NULL` are not valid, in my language as well as in many others.

I defined more lookup tables like that, although not one for every and each operator. The reason is that the same rule applies to more than one operator.

For example, the same lookup table works well for `+`, `-`, `*`, `/`. Another table works for comparison operators, such as `<=`, `<`, `>`, `>=`. And so on, I grouped operators together and defined only the strictly necessary lookup tables.

## Code Organization

In the previous section you saw the main items on the list what does Semantic Analysis has to do? You've also seen that the answer is very specific to the Grammar and the Language you're building the compiler for.

With this information, you are now able to implement your own Semantic Analysis module. Nonetheless, I want to show more details about my implementation, so you can get a more tangible  perspective of what my code looks like, and possibly give me some good suggestions.

As you know, my whole Compiler is implemented in C. I created the header file _semantic.h_ for the Semantic Analysis part. First of all, I defined some constant for the types (that we already saw), and for error messages.

```c
#define _int 0
#define _float 1
#define _string 2
#define _null 3
#define _bool 4
#define _list 5

#define NODE_OK 100

#define _undef -100
#define UNDEFINED_SYMBOL -1
#define NODE_TYPE_ERROR -2
#define LIST_TYPE_ERROR -3
#define SEMANTIC_ERROR -4
#define BREAK_OUT_OF_CONTEXT -5
#define CONTINUE_OUT_OF_CONTEXT -6
#define OVERWRITE_TYPE_ERROR -7
```

The constant `NODE_OK` is meant to signal that the analysis of an entire Node (that is, a subtree) went fine, in case we do not have to return a type.

The above sentence deserves more details. The points is that, even though the algorithm to analyze any subtree is pretty much always the same, the conclusions of such analysis can be threefold:

- A simple `NODE_OK` value, that is like saying "This subtree is OK, go ahead with the analysis".
- An error message (will be discussed in a second).
- An actual type. This is the case, for example, for subtrees that represent Expressions. The analysis must return the type of that Expression subtree, if it's well-typed of course.

Thus, the negative constant values I defined are meant to represent different error types

For example, the constant `BREAK_OUT_OF_CONTEXT` will be returned when Semantic Analysis find a `break` keyword used outside of a loop or a if/else.

`LIST_TYPE_ERROR` is used when the objects in a List Token do not have all the same type.

`SEMANTIC_ERROR` instead is a generic error returned in several cases. `OVERWRITE_TYPE_ERROR` is when you try to change the type of a variable, by redefining it.

The next things I defined in the header file are the lookup tables for the basic operations. I have four of them:

1. `resultType_aritm`. This is for the operators `+`, `-`, `/`, `*`, `%`.
2. `resultType_floatDiv`. This is for the operator `/.` that means float division in my language.
3. `resultType_compare`, for `<`, `>`, `<=`, `>=`.
4. `resultType_logic`, for `&&`, `||`, `!=`, `==`.

After this, the next step was to define the data structures that I will use. I will not discuss in too much detail what data structures are typically used for Semantic Analysis as I illustrated that in great detail in my previous article (see the references at the end).

Here's what I needed to use:

- A Symbol structure.
- A Symbol Table, that keeps references to a collection of Symbol structs.
- A Stack of Context objects. I will explain this in a moment.

In general though, with complex languages and especially OOP languages, you will also need a Stack of scopes, where each scope is intended like real scopes defined by language elements. For example, each method in a class defines its own scope. A class has a scope that contains more scopes (one for each method, for example). And then there's also the global scope, with the variables that are visible in all parts of the code. Each of these scopes need to have its own Symbol Table. My Grammar is very simple, and therefore there exists only a global scope.

What about the data structure I called Stack of Context? The motivation for it lies in tracking down the correct usage of keywords such as `break` and `continue`: we need to make sure they are used only within a loop, or a if/else statement.

The easiest way to do this is to keep a pointer to a stack-like data structure, where information about the current context is pushed on the top, or popped from the top. This is exactly the definition of stack in computer science.

Thus, when we enter a loop in the source code, I would push on the Context Stack this information. Then, when we leave the loop, I would pop from the stack. If we enter a if/else before leaving the loop, I would push that information too, onto the stack. So, if the code is well written, we would first leave the if/else (one pop from the stack), and then leave the loop (another pop). At any moment, if a `break` or `continue` keyword is found, I would check what's the context at the top of the stack, and if it's not a loop or a if/else then I would return an error value.

After all, the information we need in the Context is entirely contained in the TokenType structure, defined a long time ago when I implemented the Lexical Analysis, in the header file _lexer.h_. Once more, I can only recommend to check out previous articles of this series.

Here's how the Context Stack is implemented.

```c
struct ContextStack {
    enum TokenType top;
    struct ContextStack *next;
};
```

And here are the operations it supports:

```c
include parser.h // it includes lexer.h

struct ContextStack* alloc_Context(); // create
void free_Context(struct ContextStack *stack); // free memory
void push_Context(struct ContextStack **stack, enum TokenType type); // push on the top
enum TokenType pop_Context(struct ContextStack **stack); // pop from the top
void print_Context(struct ContextStack *stack); // print at screen
```

Regarding Symbol and Symbol Table, here's how I implemented them. Just keep in mind that **a clever solution for the Symbol Table is to implement is as a Hash Map**. The reason is that we will have to perform a lot of searches in this table; at any rate I discussed this subject, as well as alternative implementations, in the previous article.

Since my focus wasn't on implementing a great hash function (that, by the way, is a very interesting task and one that is worth studying in depth), I decided to keep it simple and used a very standard linked list.

```c
struct Symbol {
    char *sym;
    int type;
    int list_type; // type of each element if list
};

struct SymbolTable {
    // currently implemented as a linked list
    struct Symbol *head;
    struct SymbolTable *next;
};
```

Note that each Symbol structure keeps track of both its own type, as well as the type of the objects it contains in case it's a list. We saw a lot of Linked List-like data structure in the previous articles, so I will not discuss it further at this time.

The entry point of Semantic Analysis is also specified in the header file _semantic.h_, and it's the following function

```
int analyze_Program(struct ParseTree *node);
```

The argument node is assumed to be a pointer to the root of the entire Parse Tree that was built by the Parser. The whole thing, not a subtree.

Switching to the actual implementation (the file _semantic.c_), let's start by seeing the analyze_Program function.

```c
int analyze_Program(struct ParseTree *node) {
    struct SymbolTable *table;
    struct ContextStack *stack;
    int res;

    table = alloc_SymbolTable();
    stack = alloc_Context();

    res = _analyze_Program(node, &table, &stack);

    free_SymbolTable(table);
    free_Context(stack);

    return res;
}

int _analyze_Program(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int status;
    int res;

    line = node->child;
    status = _undef;
    res = NODE_OK;
    while (line != NULL){
        status = analyze_Line(line, table, stack);
        if (status < 0){
            printf("ERROR: %s\n", type2str(status));
            res = SEMANTIC_ERROR;
        }
        else
            printf("OK. Type is %s\n", type2str(status));
        // Skip Endline
        line = line->sibling->sibling;
    }
    return res;
}
```

Basically, the entry point `analyze_Program` is just an interface function that creates the data structure we will need along the process (SymbolTable and ContextStack) and then calls the internal function `_analyze_Program`.

This internal function has a very simple behavior. It takes each Line node, child of the root node (that is Program TokenType), and calls the internal function `analyze_Line` for each of them. Notice that Endline Tokens are skipped, because we know the Parser already checked they are there.

Let's now take a look at the `analyze_Line` function.

```c
int analyze_Line(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int res;

    line = node->child;
    if (line->data->type == Assign)
        res = analyze_Assign(line, table);
    else if (line->data->type == IfLine)
        res = analyze_IfLine(line, table, stack);
    else if (line->data->type == Break)
        res = analyze_BreakLine(line, table, stack);
    else if (line->data->type == Continue)
        res = analyze_ContinueLine(line, table, stack);
    else if (line->data->type == Input)
        res = analyze_Input(line, table);
    else if (line->data->type == Output)
        res = analyze_Output(line, table);
    else if (line->data->type == LoopLine)
        res = analyze_LoopLine(line, table, stack);
    else
        res = SEMANTIC_ERROR;

    if (res < 0)
        return res;
    else
        return NODE_OK;
}
```

In this function, I quite simply identify the type of Line that is currently under analysis, and invoke the right function.

One point I want to drive your attention on is that **all functions named `analyze_XYZ` have similar signatures**, and they call each other depending on the current Token Type.

Let's keep digging deeper, and have a look at the `analyze_Assign` function. Like its name says, this function will be invoked when a Token `assign` is found. It's worth recalling once more that this kind of Line is grammatically defined as

```
assign      :   var '=' expr
```

You will see that the code uses this definition.

```c
int analyze_Assign(struct ParseTree *node, struct SymbolTable **table) {
    struct ParseTree *var, *expr;
    int found_var, valid_expr;
    struct Symbol *sym;

    // Get the part of this subtree we are interested in
    var = node->child;
    expr = var->sibling->sibling;

    found_var = analyze_Var(var, table);
    if (found_var == UNDEFINED_SYMBOL)
        // the add_symbol function prints a log message
        // and add the symbol in the table
        add_symbol(table, var->data->lexeme); 

    sym = search_symbol(*table, var->data->lexeme);
    valid_expr = analyze_Expr(expr, table, &sym);
    if (valid_expr < 0){
        printf("Expression is not valid\n");
        return valid_expr;
    }
    if (sym->type != _undef && valid_expr != sym->type){
        printf("ERROR: Cannot Modify Type for Identifier %s to %s. It was %s.\n",
               var->data->lexeme, type2str(valid_expr), type2str(sym->type));
        return OVERWRITE_TYPE_ERROR;
    }
    assign_type(table, var->data->lexeme, valid_expr);
    return valid_expr;
}
```

After all, this function is quite easy too. It gets pointers to the subtree that contain the Var and Expr parts of the Assign Line, then it calls the function _analyze_Var_ and _analyze_Expr_. I am sure you start getting the pattern by now.

Let's **keep walking down the tree**, and look at the `analyze_Var` function.

```c
int analyze_Var(struct ParseTree *node, struct SymbolTable **table) {
    struct Symbol *found;

    found = search_symbol(*table, node->data->lexeme);
    // found contains the _type of the symbol, or UNDEFINED
    if (found != NULL)
        return found->type;
    printf("Variable Symbol Not Found In Symbol Table: %s\n", node->data->lexeme);
    return UNDEFINED_SYMBOL;
}
```

And so we see one case where the recursive calls end. The function `analyze_Var` simply checks if the symbol contained in the current Node was already defined. If yes, it returns its type. If not, it returns the error value `UNDEFINED_SYMBOL`.

Let's briefly turn our attention to the function `analyze_Expression`, that I discussed in detail in a section above. We can thus see how **the theory is implemented into code**.

Recall that the expressions are defined in the Grammar with a recursive rule:

```
expr        :   pred (condOp expr)?
pred        :   term ( ('+' | '-') pred)?
term        :   baseExpr ( ('*' | '/' | '/.' | '%') term)?
baseExpr    :   obj
            |   '(' expr ')'
```

You will see that the code leverages on this structure quite a lot, and uses functions such as `analyze_Pred`, `analyze_Term`, `analyze_BaseExpr`.

```c
int analyze_Expr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym){
    struct ParseTree *child, *op;
    int type1, type2, result;

    child = node->child; // the Pred Token
    type1 = analyze_Pred(child, table, sym);
    if (type1 < 0){
        printf("Sub Expression Ill-Formed ");
        if (sym != NULL)
            printf("For Symbol: %s\n", (*sym)->sym);
        else
            printf("\n");
        return type1;
    }

    if (child->sibling == NULL)
        // then this expression is completed
        return type1;

    // Save the operator, skip it for now
    op = child->sibling;

    // Analyze the second part of this expression
    type2 = analyze_Expr(op->sibling, table, sym);
    if (type2 < 0){
        printf("Sub Expression Ill-Formed ");
        if (sym != NULL)
            printf("For Symbol: %s\n", (*sym)->sym);
        else
            printf("\n");
        return type2;
    }

    // Now compute the result type with the lookup tables
    // based on the operator previously saved
    if (is_ComparisonOp(op->data->type))
        result = resultType_compare[type1][type2];
    else if (is_LogicOp(op->data->type))
        result = resultType_logic[type1][type2];
    else
        result = NODE_TYPE_ERROR;
    return result;
}
```

I think I explained the entire logic in a previous section (see Tasks for Semantic Analysis - Expressions), so I will let you connect the dots!

## Take-home Messages

If you made it so far, and you also read other articles, you have now got a good grasp on all concepts around Semantic Analysis. In the past articles I discussed a lot of theoretical details, whereas in this article we saw many practical details and implementation tips.

You are now ready to implement your own Semantic Analysis! Remember that:

- You'll need data structures such as Symbol Tables, Scopes/Context and Stack.
- Your algorithm is going to be heavily recursive.
- You should identify the places where the recursion will end. These are usually very close to the bottom of the Parse Tree.
- You can take a look at my own implementation, but don't be afraid to experiment and strive for further optimization. Focus on learning the concepts though, without spending time on "irrelevant" details. Irrelevant for the sake of learning, of course.

## Summary and Outlook

You are now able of building your own front-end Compiler!

Semantic Analysis is indeed the last module in what's usually called front-end compilation process. The previous two are Lexical Analysis and Parsing.

We have studied the three of them in great detail, both theoretically and practically. Therefore you can take what you've learnt and put it into practice.

After this, the path towards building a new programming language and its Compiler is waiting for one more milestone only: Code Generation.

We have already studied Code Generation at the theoretical level, and in the next article I will show how I put those concepts into practice and built my own Code Generation Module.

## References

My [GitHub repository](https://github.com/pgrandinetti/compilers) for this course.

[Table of contents]({{< ref "/" >}}) for this course.
