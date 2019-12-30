---
title: "Code Generation for a New Programming Language"
date: 2019-10-14
draft: false
---

Code Generation is the final step of the journey through Grammars, Parsers, Semantics and Compilers.

In this article, I will discuss with you the implementation I made of a Code Generation module, as part of the Compiler I wrote for a new programming language that I designed.

Let's also briefly review what we've learnt so far in this course about Compilers and Programming Languages. We went into the details of many central subjects as well as some side-topics that are useful to understand the whole picture. Here's a partial, but well representative list:

1. What is a Compiler.
2. What stages make the whole compilation process.
3. What is a Formal Grammar, and several existing types of Grammars.
4. A real design example of a new Context-Free Grammar, for a new Turing-complete programming language (that I often refer to as "my language").
5. What is Parsing, the second stage of compilation.
6. A real implementation of a LL(1) Recursive Descent Parser, made for the new programming language mentioned above.
7. What is Semantic Analysis, the third stage of compilation.
8. A real implementation of Semantic Analysis, for my language.
9. How the computer actually runs code, and how the internal memory of the machine is organized when someone wants to run a program.
10. How to generate code that can be run by a very low level architecture, such as a stack machine (ideal) or a MIPS processor (a real one!).

## Motivations

Code Generation is the most important part of the **back-end compilation**. While front-end takes care of ensuring a source code is well-written, the back-end is all about making that source code runnable in the computer.

What does well-written mean? The front-end modules (Lexical Analysis, Parsing and Semantic Analysis) perform a lot of "security" checks, for example:

- Lexical Analysis checks that all characters in the source code are allowed in the Language's Alphabet. For example, in my language, you cannot use the character `@`.
- Parser checks that all sequence of tokens satisfy at least one rule in the Formal Grammar. For example, in my language, if you type `if` then you must also type a parenthesis `(`, then you must type a boolean expression, then a parenthesis `)`, and so on.
- Semantic Analysis try to catch all errors that were uncaught so far. Most of the checks it does are about typing. For example, in my language, you cannot mix types in a list: the declaration `a = [1, 2, "x", 5]` is illegal and Semantic Analysis will stop the compilation with an error message.

This is just a small subset of what front-end really does. I can't help but suggest to check the references at the end for further insights!

At this point, it's hard to believe Code Generation needs to be motivated in itself. It's quite obvious that just passing all checks made by the front-end is not enough to actually run the code!

The front-end giving its "OK" just means that the code may be run.

Let's look at it from a different angle. If you design a new Programming Language, you specify all rules that people must follow in order to write a correct source code. Then, when you implement each front-end module, you are just making sure people followed all grammatical rules. But now, there's nothing that actually runs that source code in the machine, because it's a new programming language!

Here's where Code Generator enters the play.

Quite simply, Code Generator takes whatever was produced by the front-end, such as Parse-Tree and Symbol Table, and produce some other code that can actually be run in the machine, based on the fact that the front-end already verified the source code's soundness.

Let me repeat it. Within a Compiler, **Code Generator's output is a string**. This string is in fact a source code in a language different from the original source code, one that can be executed.

So far, so good. The real question, in my opinion, is: **what is the language used as output of the Code Generator?**

In a previous article we learned how to get down to actual CPU level, and we saw in detail how to build a Code Generator module that generates code for the MIPS architecture, or for a stack machine (which is an abstraction). Obviously, I like the MIPS better, for the simple reason that is a real one! The stack machine, however, brings a lot of learning experiences on the table too. Either case, Code Generator produces something **very similar to Assembly code**.

That's one of the best options we have. If you are building up a strong team because you want to create a super strong Programming Language, then you definitely should consider to **generate machine-code as your first option**.

That's not my case, clearly. My objective in this course is to learn the concepts in real depth, and to work on a project hard enough to enhance my learning experience. To produce Assembly code would be, in my opinion, a large deviation from such a goal, for little gain.

The other available option is to generate code in a language that can already be compiled/interpreted. This is the option I chose.

Why? The motivation is that working on a Code Generation module for me is simply the "excuse" to learn more about what we saw in a previous article, mainly **Code Templates**. _To learn by doing_, as they say. From this point of view, it doesn't really matter whether the final code is in Assembly, C, Java or Python.

And in fact, **I chose Python as output language**. Why that one? The reason is that the Compiler I am building for my language is written entirely in C, so I didn't want to generate C code again.

Other languages made little sense to me. Python, on the other hand, is a very intuitive language exactly like mine. Of course, my language is much simpler than Python, but in both cases you should be able to look at the source code and "read" it.

The other big advantage is that some parts of my language's Grammar are very similar to Python's Grammar. Thanks to this, while working on Code Generation **I can focus my attention on what really matters and on the learning experience**, instead of spending time in literal, boring translation of single pieces of the Grammar.

## Code Organization

To organize the implementation of the Code Generation was a tricky affair. The main problem is that **the whole module must handle, create and modify a lot of strings**.

And as we all know, the C language isn't too user-friendly when it comes down to strings. On the other hand, I wouldn't accept any other language to write my Compiler with!

### A step in the wrong direction

I knew since the beginning that the overall implementation strategy would resemble a recursive descent. How did I know that? It's written in my previous article!

So, one thing was clear from the start. The input of the Code Generator must be the Parse Tree. Or, to be precise, at least one of the input arguments.

At the beginning, though, I thought I would propagate a double pointer (a char pointer) across all recursive calls, so that once the execution would get at the bottom of the recursion (for **a leaf node**) there would be a function taking care of that node, and it would modify the current pointer by inserting the code for that leaf. As we all know, to modify the content of a pointer within a C function, we need a double pointer as argument to that function.

Note what I just said: **inserting the code**. Here's what Code Template enters the play. The idea is that the boilerplate code would be prepared by some of the high-level functions, for example the one taking care of an Assignment Line. Then, this function would start invoking other functions, possibly in a recursive way. And those functions would fill the spaces in the boilerplate template.

I even prepared a nice utility function that inserts a sequence of characters inside a string, at a given position. Here it is.

```c
int str_insert (char** s1, char* s2, int pos) {
    /*
        Insert a sequence of characters into a string, starting from a given index.
        The old string is moved to a new memory location, and the old memory is freed.

        - **s1: The string to modify.
        - *s2: The sequence of characters to insert into s1.
        - pos: The starting index.
    */
    if (! s2)
        return -1;

    char* new;
    int l1, l2;

    l1 = strlen(*s1);
    if (pos > l1 || pos < 0)
        return -1;

    l2 = strlen(s2);
    if (l2 <= 0)
        return 0;

    new = calloc(l1 + l2 + 1, sizeof(char));
    if (! new)
        return -1;

    memcpy(new + pos, s2, l2 * sizeof(char));
    memcpy(new + pos + l2, *s1 + pos, (l1 - pos) * sizeof(char));
    memcpy(new, *s1, pos * sizeof(char));

    free(*s1);
    *s1 = new;
    return 0;
}
```

However, as I started designing the module, I understood things would get too messy in this way.

To start with, each function should not only receive the string template (the double pointer), but also the position where to fill space in the template.

To make things worse, when filling some space in the template, this would shift in space all the other slots to be filled, according to the length of the inserted string.

So, to summarize:

- The function that actually inserts a string into the template would need to know the position where to insert, and how much space is available. - If the space is not sufficient, then it has to reallocate the template, shift a bit to create more space where the new string has to be inserted, and finally insert it.
- The caller function, the one that created the template, will have to know the length of the inserted string, and recompute the positions for all the other "empty slots" in the template. Possibly, a new reallocation of the string must be done.
- All this, for every blank space in the template. Believe me, there are a lot of them!

I am not saying it can't be done this way. It can, obviously. However, I have the impression the implementation would be very awkward and inefficient. I might be wrong though, since I didn't go down this road at all!

### Back to business

After I excluded the option to create a big boilerplate template and propagate it down to the Parse Tree's leafs, finding an alternative was not too difficult.

I needed to design **each of the functions** that handle nodes in the tree in a way that they can **build the string that only concerns that node**. Then, such string will be returned to the caller of the function, and **the caller will take care of inserting it into the Code Template**.

So, to start with, this approach solves one problem. The caller gets the string to be inserted as a result, and can easily compute the length of it, for example with `strlen(char *str)` in C.

However, it doesn't seem to solve the reallocation problem at all. In fact, there's an obvious solution to it.

Instead of allocating the Code Template, then calling the nested functions (without passing the template to them), grab the results and plug them into the template, **I executed the same steps in a different order**!

First, I'd call the functions created for the lower levels of the Parse Tree, and grab the results. Then, and only then, I would allocate the Code Template based on the length of those results! Simple, and it works.

Let's take a look at a (partial) example, to understand better.

Consider the assignment Line in my programming language, defined in the Grammar as it follows:

```
assign      :   var '=' expr
```

As you know by my previous articles about Parsing, there will be a node in the Parse Tree for each Assign Line in the source code. Each of these nodes will have three children, one for the `var` Token, one for the `=` Token, and a third one for the `expr` Token.

The whole idea is to prepare a function with the following signature:

```c
char* cgen_Assign (struct ParseTree *assign_node)
```

and make sure it returns the string that corresponds to the code generated for the entire `assign` Line.

Here's how such function works:

1. To start with, it invokes another function, `cgen_Var (struct ParseTree *var_node)`, with argument the first child of `assign_node` (because that's the `var` Token!). The result will be stored in a `char *var`.
2. Then, it will invoke a function, `cgen_Expr (struct ParseTree *expr_node)`, giving the third child of `assign_node` as argument. The result will be saved into `char *expr`.
3. It will now compute the length of the two results. This is as simple as

    ```
    int l1 = strlen(var)
    int l2 = strlen(expr)
    ```
4. Now, and only now, it will create the Code Template. This will be a string, and it will be allocated directly with length equal to `l1 + l2 + 1` (for the `=` character) `+ 1` (for the last null-character, in C-strings) `+ padding_space`, if you want to use some padding around the `=` sign.
5. The two previous results will be copied into the new template, and their memory released.
6. Finally, the template will be returned as result to whoever invoked the function `cgen_Assign`.

If you are now wondering how the other functions, such as `cgen_Var` and `cgen_Expr`, work, the answer is: quite similarly to `cgen_Assign`!

The logic is exactly the same. Each function needs to invoke the functions built for sub-token, according to the Grammar, and then paste together these results in a bigger string, release the memory when needed, and finally return the whole string.

In my opinion this approach offers many advantages:

- To start with, it's extremely clear, which makes the code more easily understandable and maintainable.
- It's more efficient, at least intuitively, than the other approach I discussed before.
- It closely follow the Grammar, which is always a good thing to do when implementing any Compiler's module.

### Entry Point

The Code Generator needs an entry point that can be invoked from outside. An API, if you will.

I kept it simple, and simply provided the following function in a header file named _cgen.h_.

```
char* code_gen (struct ParseTree *root)
```

### A Python Specialty

You probably already know that Python has a very unique feature, among famous languages: good indentation is required.

In Python, if blocks, loop blocks, functions definitions, classes, every kind of block actually, are delimited by indentation. Not by curly brackets as in many other languages (C and Java, for example).

Therefore, my Code Generation module needs to put the right indentation in the output code.

I solved this problem by adding one argument in most of the functions' signatures. For example, the function we saw in the earlier example is actually defined as

```
char* cgen_Assign (struct ParseTree *node, int indent)
```

What does it do, in truth, is just to allocate the final template with indent more slots of memory, and add indent blank spaces at the beginning of the string.

How's the indent argument calculated? It's not. It starts from a fixed value, zero, and it's increased in some of the recursive calls, not in all of them.

For example, the function cgen_IfLine receives the indentation integer as second argument. The body of the `ifLine` in my Grammar is made by a sequence of Lines. Remember that a `line` can be really anything:

```
line        :   assign
            |   input
            |   output
            |   ifLine
            |   loopLine
            |   breakLine
            |   contLine
```

Therefore, each Line that is met within the body of the `ifLine`, will generate a call to a function `cgen_Line`. The arguments for this nested call will be a pointer to the node for that `line` in the Parse Tree, and a new indentation level. 

Let me clarify the whole strategy with more details. First of all, I defined a constant

```
#define INDENT_LEV 4
```

Which simply represents how many spaces each indentation level should add in the final Python code.

Then, I defined a function that takes care of the Line Tokens in the Parse Tree:

```
char* cgen_Line (struct ParseTree *node, int indent)
```

As you can see, this function has the same signature as the others. All this function does is to check what's the Token Type of its child and call the related function. For example, if the child is a `ifLine`, then it will invoke the function `cgen_IfLine`.

On its part, the function `cgen_IfLine` will need to call many other functions:

- It will call `cgen_Expr`, to generate code for the `ifCond` Token (the boolean expression that determines whether to enter or not the if).
- It will have to call `cgen_Line` for each line that it's inside the body of the if block.

Here comes the trick. When the nested `cgen_Line` is called from within the `cgen_IfLine`, it will be invoked in the following way.

```c
// This is inside the cgen_IfLine function
// where next_node is the next Line, and indent is the current argument.
char* line;
line = cgen_Line (next_code, indent + INDENT_LEV);
```

This way the nested `cgen_Line` will receive the indentation increased, and will apply the right number of blank spaces.

If there are more nested calls, such as an `ifLine` inside another `ifLine`, maybe inside a `loopLine`, the `+ INDENT_LEV` part will always make the argument to the nested function larger. The more nested are the functions calls, the greater the value for the indent argument will be. Exactly as it should in Python code!

Now that you understood all of this, it's easy to see why the main entry point has actually a very simple implementation.

```
char* code_gen (struct ParseTree *root) {
    return cgen_Program(root, 0); // start with indent = 0
}
```

## Examples - Basic Grammar Blocks

After clarifying (and, in fact, understanding) a lot of concepts, it's time to code!

To start off, let me recall the two main data structures I used in the previous modules. The first one is called `Token`:

```
struct Token{
    char* lexeme;
    enum TokenType type;
};
```

The field `lexeme` is simply the sequence of characters that make that Token. And the field `type` is one among a-priori defined types, such as Int, Var, Assign, Line, etc.

For example when the Lexical Analysis module finds the following sequence of characters

```
x = 5;
```

Will creates many Tokens:

```
{lexeme = "x", type = Var}
{lexeme = " ", type = Whitespace}
{lexeme = "=", type = Equal}
... 
```

The other key data structure is the Parse Tree. Here's how I defined it

```
struct ParseTree {
    struct Token* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};
```

As I explained thoroughly in a previous article, each node of the Tree has a pointer to the first child and the first sibling. This allows an easier navigation of the Tree, in my opinion.

The first example I want to discuss with you is perhaps the easiest case in the entire Code Generation module.

Suppose that the Code Generator is walking up and down the Parse Tree and it finds a subtree that it's a `continue` type of Line. That's basically just one line

```
continue;
```

How should it generate code for it? Here's what I did.

```c
char* cgen_Continue (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Continue)
        return NULL;

    char *result;

    result = calloc(indent + 9, sizeof(char));
    if (! result)
        return NULL;

    if (indent > 0)
        memset(result, ' ', indent * sizeof(char));
    memcpy(result + indent, "continue", 8 * sizeof(char));
    return result;
}
```

In the output language (Python) the equivalent of a `continue` instruction of my Language is ... `continue`. The same string.

Therefore I generate code in this case by doing the following:

1. Check whether the function was invoked with the correct subtree. If not, return a `NULL` pointer.
2. Allocate a string in memory that will contain the result. Reserve 9 slots of memory, one for each character of the word `continue`, and one for the terminating character `/0` (this is C-specific). Also add as many slots as the indent argument requires. Keep in mind that the C function `calloc` allocates all requested memory to `/0`, so we will not need to touch the last character.
3. Set the first indent characters to blank.
4. Set the remaining 8 characters to 'c', 'o', 'n', 't', 'i', 'n', 'u', 'e'.
5. Return the pointer to the new string.

That was very easy, wasn't it?

Another easy case is when subtree for which we must generate code is nothing more than an `int`. Here's how I addressed this case.

```
char* cgen_Int (struct ParseTree* tree) {
    if (! tree|| tree->data->type != Int)
        return NULL;

    char* integer;
    int len;

    len = strlen(tree->data->lexeme);
    integer = calloc(len + 1, sizeof(char));
    memcpy(integer, tree->data->lexeme, len * sizeof(char));
    return integer;
}
```

Again, very simple. The `lexeme` field for a Token whose type is `int` will contain exactly the string representation of that integer number.

What I am saying is that if source code contains the line `x = 234;` then in the Parse Tree there will be a Token `{type = Int, lexeme = "234"}`. Hence:

1. Get the length of the lexeme string (using C function `strlen`).
2. Allocate a new string with the same length plus 1, for the terminating null character.
3. Copy the content of `lexeme` into the new string and return it.

Why did I copy the content of lexeme, instead of just returning its pointer? Because that object in memory can be erased or modified if any change is made to the Parse Tree. Thus, it's better to allocate a separate space of memory. It's easy to understand this stuff after having studied Compilers!

Many other parts are as simple as the two functions we've already seen: the `break` subtree, the `var` subtree, the `bool` and `Null` subtrees. These are all examples of subtrees that are very simple to handle in terms of code generation.

Let's look at a slightly more complex case. I believe looking at the function that generates code for the `float` Token will be really helpful.

First of all, we have to look at the Grammar definition for the Float Token. Here it is.

```
float       :   (frac exp?) | (int frac? exp?)
```

Knowing that, here what we need to do:

1. Determine whether the first child in the subtree is an `int` or a `frac` TokenType. The rule in the Grammar says that these two are the only options.
2. If it's an `int`, then invoke the `cgen_Int` function giving to it the subtree whose root is the first child of the `float` subtree.
3. Then, invoke the functions `cgen_Frac` and `cgen_Expr` (that we haven't seen yet), with the remaining children of the root node.
4. Paste all results into a unique string, newly allocated.
5. Free the memory used for the intermediate results and return the main result.

The logic is still quite simple. The implementation is slightly more involved, simply because of C's syntax. Here it is.

```c
char* cgen_Float (struct ParseTree* tree) {
    if (! tree || tree->data->type != Float)
        return NULL;

    char *integer, *frac, *pow, *result;
    int l_int, l_frac, l_pow;

    integer = frac = pow = NULL;
    l_int = l_frac = l_pow = 0;
    tree = tree->child;

    if (tree->data->type == Int) {
        integer = cgen_Int(tree);
        tree = tree->sibling;
    }
    if (tree != NULL && tree->data->type == Frac) {
        frac = cgen_Frac(tree);
        tree = tree->sibling;
    }
    if (tree != NULL && tree->data->type == Pow)
        pow = cgen_Pow(tree);

    if (integer)
        l_int = strlen(integer);
    if (frac)
        l_frac = strlen(frac);
    if (pow)
        l_pow = strlen(pow); // pow contains the 'e' too

    result = calloc(l_int + l_frac + l_pow + 1, sizeof(char));

    if (integer)
        memcpy(result, integer, l_int * sizeof(char));
    if (frac)
        memcpy(result + l_int, frac, l_frac * sizeof(char));
    if (pow)
        memcpy(result + l_int + l_frac, pow, l_pow * sizeof(char));

    free(integer);
    free(frac);
    free(pow);
    return result;
}
```

The only trick you'll need to look up is the correct usage of the C's function `memcpy`. As for the functions `cgen_Frac` and `cgen_Pow`, I can't but recommend to check out my GitHub project!

## Examples - Higher Level Tokens

To generate code for the higher-level Tokens is a more complex task. In particular, the trickiest ones are the Tokens with a variable size definition in the Grammar.

What am I talking about? For instance, Tokens such as the `quotedStr`, whose definition is

```
quotedStr   :   '"' (char)* '"' (',' obj)*
```

The main difficulty with this Token (and similar ones) lies in the final part. After a sequence of characters (that's not difficult to get), there's a repeated pair of Tokens `(Comma, obj)`.

The number of times such a pair is repeated depends on the special characters `"%s"` inside the previous sequence. Therefore, it's not known a priori how many of them there will be. Especially in C, this makes it tricky in terms of memory allocation.

There's one good news, at last: the Parser already checked that the number of pairs and the number of `"%s"` characters are the same, so Code Generator does not need to take care of it.

Here's how I organized the function `cgen_QuotedStr(struct ParseTree* tree)`:

1. Get the node that contains the sequence of characters (first part of the definition in the Grammar), and keep a pointer to it `*q`. Also, save the length of this sequence, `int len_q = strlen(q)`.
2. Walk down the tree once, just to count how many pairs `(Comma, Obj)` there are. Don't do anything, just count them and save it in a variable `count`.
3. Initialize an array of pointers to string with the right length: `char* objs [count]`.
4. Set `objs[0] = q`.
5. Set a variable `int total = len_q`.
6. Walk again down the parse tree. This time, for each `obj` node found invoke the function `cgen_Obj` and save the results inside the array objs (use a counter as index, starting from location 1). Also, for each of these calls to `cgen_Obj` increase the integer total with the length of the result.
7. Allocate a new string `result` with size given by `total + 1 + padding` (for the terminating null character), where padding takes into account all additional characters we need in Python. These are the `"%("` and `")"` that you must use in Python's string interpolation, like in `"this are two numbers %s, %s" %(5, 6)`.
8. Finally, loop over the array `objs` and, for each string in it, copy its content into `result`. Mind the initial index to copy into! Also add the characters required by Python at the right spots.
9. Free the memory occupied by every string in `objs`, then return result.

And here's the code (although I guess looking at it in GitHub is better).

```c
char* cgen_QuotedStr (struct ParseTree* tree) {
    if (! tree || tree->data->type != QuotedStr)
        return NULL;

    char *result, *obj;
    int total, last, count, l_str;
    struct ParseTree *tmp;

    tmp = tree->child; // actual quoted string node

    total = strlen(tmp->data->lexeme);
    last = 0;
    count = 1; // counting the first quoted string too
    obj = NULL;

    // Count total Obj
    while (tmp->sibling != NULL) {
        count ++;
        tmp = tmp->sibling->sibling; // skip Comma
    }
    if (count > 1)
        total += 4; // the padding " %(" ... ")"


    // To avoid triple pass store computed Obj(s)
    char* objs[count];
    count = 0;

    tmp = tree->child; // start over

    l_str = strlen(tmp->data->lexeme);
    objs[count] = calloc(l_str + 1, sizeof(char));
    if (objs[count] == NULL)
        return NULL;
    // copy the first part of QuotedStr in objs[0]
    memcpy(objs[count], tmp->data->lexeme, l_str * sizeof(char));
    count ++;

    while (tmp->sibling != NULL) {
        tmp = tmp->sibling->sibling;
        obj = cgen_Obj(tmp);
        if (obj == NULL)
            // _bail_out_Str is an internal function that cleans
            // memory and returns an error code
            return _bail_out_Str(objs, count);
        objs[count++] = obj;
        total += strlen(obj) + 1; // + 1 for the ","
    }

    result = calloc(total + 1, sizeof(char));
    if (! result)
        return _bail_out_Str(objs, count);

    for (int i=0; i<count; i++) {
        l_str = strlen(objs[i]);
        memcpy(result + last, objs[i], l_str * sizeof(char));
        last += l_str;
        free(objs[i]);
        if (i == 0 && count > 1) {
            result[last++] = ' ';
            result[last++] = '%';
            result[last++] = '(';
        }
        if (i > 0 && i < count - 1)
            result[last++] = ',';
    }
    if (count > 1)
        result[last] = ')';
    return result;
}
```

The other very interesting part of Code Generator is about the `expr` Token and its recursive definition. Do you remember the chain of Tokens Expr - Pred - Term - BaseExpr, that I spent so many words on in a previous article? I hope you do!

Here's how the Expr Token is defined in the Grammar.

```
expr        :   pred (condOp expr)?
pred        :   term ( ('+' | '-') pred)?
term        :   baseExpr ( ('*' | '/' | '/.' | '%') term)?
baseExpr    :   obj
            |   '(' expr ')'
```

Let's just look at the Code Generator function for the `expr` Token, because that alone will clarify a lot also for the others.

The steps to take are actually pretty obvious once we look at the Grammar.

1. Call the function `cgen_Pred` and save both results and the length of it.
2. Check if there are more nodes in the subtree.
3. If there isn't any then go to step 4. If there are, then they must be a `condOp` and another `expr`. So, quite simply, invoke the two related functions `cgen_Op` and `cgen_Expr`. Note that the latter is a recursive call. Save those results and their length.
4. Compute the total length (summing up the partial ones).
5. Allocate space for the final results.
6. Copy the previous results' content into the newly allocated space. Plus any padding space.
7. Free memory for partial result and return the final one.

And here's the code. Have fun reading it!

```c
char* cgen_Expr (struct ParseTree* tree) {
    if (! tree || tree->data->type != Expr)
        return NULL;

    char *pred, *op, *expr, *result;
    int l_pred, l_op, l_expr;

    pred = op = expr = result = NULL;
    l_pred = l_op = l_expr = 0;
    tree = tree->child; // must be Pred

    pred = cgen_Pred(tree);
    if (pred == NULL)
        return NULL;
    l_pred = strlen(pred);

    if (tree->sibling != NULL) {
        tree = tree->sibling; // the Op
        op = cgen_Op(tree);
        if (op == NULL) {
            free(pred);
            return NULL;
        }
        l_op = strlen(op);
        tree = tree->sibling; // the other Expr
        expr = cgen_Expr(tree);
        if (expr == NULL) {
            free(op);
            free(pred);
            return NULL;
        }
        l_expr = strlen(expr);
    }

    if (l_op > 0)
        result = calloc(l_pred + 2 + l_op + l_expr + 1, sizeof(char));
    else
        result = calloc(l_pred + l_op + l_expr + 1, sizeof(char));
    if (! result) {
        free(pred);
        free(op);
        free(expr);
        return NULL;
    }
    memcpy(result, pred, l_pred * sizeof(char));
    if (l_op > 0) {
        result[l_pred] = ' ';
        memcpy(result + l_pred + 1, op, l_op * sizeof(char));
        result[l_pred + 1 + l_op] = ' ';
        memcpy(result + l_pred + l_op + 2, expr, l_expr * sizeof(char));
    }
    free(pred);
    free(op);
    free(expr);
    return result;
}
```

Again, other pieces of the Code Generator may seem complex, but in general they just follow the Grammar's definitions. For example, here's what the function `cgen_List` will do:

1. Generate a `[` character, because in Python lists start with that too.
2. Invoke the function `cgen_ListExpr`, and add its result after the initial square bracket.
3. Generate the closing bracket `]`.

And so on, all the other functions. As for this article, I will stop here, and for further analysis of the code I recommend you to reach out on GitHub!

## Summary and Outlook

We've just learnt a lot about how to generate executable code in a Compiler. Remember: a Compiler starts from a source code written in some high-level language (grammar), and produces a lower-level code that can be run into a computer.

This task is not done casually: rather, it's driven by well-established theoretical concepts. The most important one being Code Template. We explored that one in detail too.

As this if the final article in this course, you have all instruments you would need to create a simple programming language. More importantly, in my opinion, you have acquired a great deal of knowledge that will give you a competitive edge in your future projects as scientist and engineer. Good luck!

## References

My [GitHub repository](https://github.com/pgrandinetti/compilers) for this course.

[Table of contents]({{< ref "/" >}}) for this course.
