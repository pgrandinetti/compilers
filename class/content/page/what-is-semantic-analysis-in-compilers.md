---
title: "What Is Semantic Analysis in a Compiler?"
date: 2019-10-03T14:41:56+02:00
draft: false
---


Semantic Analysis is **the last step in the front-end compilation**. It's called front-end because it basically is an interface between the source code written by a developer, and the transformation that this code will go through in order to become executable.

In different words, **front-end is the stage of the compilation where the source code is checked for errors**. There can be lots of different error types, as you certainly know if you've written code in any programming language.

Let's briefly review what happens during the previous parts of the front-end, in order to better understand what semantic analysis is about. If you have read my previous articles about these subjects, then you can skip the next few paragraphs.

First, the source code is given to the Lexical Analysis module. What's the objective here? The Lexical Analyzer is often implemented as a Tokenizer and its goal is to **read the source code character by character**, groups characters that are part of the same Token, and reject characters that are not allowed in the language.

For example, if the source code contains the following sequence of characters

    'w' 'h' 'i' 'l' 'e'

the Tokenizer will group them in a unique token `while`, which is very likely to be the beginning of a loop.

On the other hand, if the Tokenizer sees the following sequence

    '?' '=' '!'

it's quite likely (although it depends on which language it's being analyzed) that it will reject the whole source code because that sequence is not allowed. As a more meaningful example, **in the programming language I created**, underscores are not part of the Alphabet. So, if the Tokenizer ever reads an underscore it will reject the source code (that's a compilation error).

I believe full understanding of this point is crucial. In some sense, **the primary objective of the whole front-end is to reject ill-written source codes**. Each level of the front-end takes care of some types of error. Lexical Analysis is just the first of three steps, and it checks correctness at the character level.

The second step is Parsing. More precisely, the output of the Lexical Analysis is a sequence of Tokens (not single characters anymore), and the Parser has to evaluate whether this sequence of Token makes sense or not.

What does it mean for a sequence of Tokens to "make sense"? It has to do with the Grammar, that is the syntactic rules the entire language is built on.

For example, one rule in the Grammar may say that a Token "while" must be followed by an open parenthesis `(`. This is probably because the boolean condition of the while loop must be enclosed into a pair of parentheses, a common scenario in many languages.

Continuing with this simple example, if the sequence of Tokens does not contain an open parenthesis after the `while` Token, then **the Parser will reject the source code** (again, this is shown as a compilation error).

An easier way to think about what the Parser does (easier for me at least) is to compare it with your native language and its grammatical rules. Let's say it's English. Imagine that a sentence is a sequence of Tokens. It really just is a sequence of words, but let's call each word a Token. And let's say, while you read the sentence you see a token 'I', and right after that another token 'I'. Now, there's no possible sentence in English where you would repeat twice consecutively the pronoun 'I'. Therefore, you should "reject" this sentence and declare that it's wrongly typed. That's a compilation error!

So, to recap:

- Lexical Analysis catches errors at the character (alphabetic) level.
- Parsing catches errors at the token (grammatical) level.

What's the matter with Semantical Analysis then? Apart from the fancy name, it's simply another module of the front-end. And it's also the last one. This means that **the goal of Semantic Analysis is to catch all possible errors that went unnoticed through Lexical Analysis and Parsing**.

If the overall objective of the front-end is to reject ill-typed codes, then Semantic Analysis is the last soldier standing before the code is given to the back-end part. It's the last guard against dumb errors.

What these errors are, then? What could be possibly missed by the first two steps, Lexical Analysis and Parsing? Let's take a look.

## An Ocean of Errors

The first point I want to make is that writing one single giant software module that takes care of all types of error, thus merging in one single step the entire front-end compilation, is possible. However, **this is not how quality software engineering works**.

Not that it hasn't been tried. Each compiler is a unique system, and there have been many experiments on how to organize the several steps required by a Compiler. Nonetheless, the three front-end steps are to be separated, in order to have a **clean and well-designed system**.

To discuss why Semantic Analysis is necessary at all, I want to start with an example. Let's say we are given a piece of C (or Java) code, where a line is

    int x y = 0;

Now, let's put ourselves in the Lexical Analyzer shoes. We must read this line character after character, from left to right, and tokenize it in meaningful pieces. Meaningful according to the language specification. 

In this example, we should be able to create a Tokens sequence such as

```
<"int", type>
<" ", whitespace>
<"x", identifier>
<" ", whitespace>
<"y", identifier>
<" ", whitespace>
<"=", operator>
<" ", whitespace>
<"0", digit>
<";", semicolon>
```

Each Token is a pair made by the lexeme (the actual character sequence), and a logical type assigned by the Lexical Analysis. These types are usually members of an `enum structure` (or `Enum` class, in Java).

For sure you've noticed that something was wrong with that line of code. It should not compile. In fact, a comma is missing between the `x` and `y` identifiers.

We could possibly modify the Tokenizer and make it much more complex, so that it would also be able to spot errors like the one mentioned above. However, this approach doesn't make sense. **Modularity is a pillar of good software engineering**.

Let the Tokenizer tokenize, and let the Parser parse. To tokenize is "just" about splitting a stream of characters in groups, and output a sequence of Tokens. To parse is "just" about understanding if the sequence of Tokens is in the right order, and accept or reject it.

Thus, after the previous Tokens sequence is given to the Parser, the latter would understand that a comma is missing and reject the source code. How can it understand that? Because there must be a syntactic rule in the Grammar definition that clarify how as assignment statement (such as the one in the example) must be made in terms of Tokens.

Such a rule may be similar to the following:

    assignment  : type identifier (',' identifier)* '=' value ';'

Which you can read as:

_A valid assignment is composed by a "type" (such as "int", in the example), then the identifier name ("x"), then one can optionally add more identifiers separated by a comma (the "*" symbol means zero or more, and it refers to the subgroup in the parentheses), then an equal sign ("="), then the "value" to assign and finally a semicolon._

The Parser is a complex software module that understands such type of Grammars, and check that every rule is respected using advanced algorithms and data structures. Parsing is a theory in itself, in fact. I can't help but suggest to read more about it, including my previous articles.

Thus, the code in the example would pass the Lexical Analysis, but then would be rejected by the Parser. Which is the correct behavior.

Now let's consider a different example, this time in Java.

```
int x = 0;
Object y = null;
String z = x + y;
```

What about this three lines of code? When they are given to the Lexical Analysis module, it would be transformed in a long list of Tokens. No errors would be reported in this step, simply because all characters are valid, as well as all subgroups of them (e.g., `Object`, `int`, etc.).

Then the list of Tokens is given to the Parser. In this case, and you've got to trust me on this, a standard Parser would accept the list of Tokens, without reporting any error. The reason is that all three lines are **grammatically well-typed**.

What about the third line then? It must be wrong! We can't possibly add an integer to an Object, which is initialized as NULL!

That's right, we can't. And indeed this source code should result in a compilation error. However, while it's possible to expand the Parser so that it also check errors like this one (whose name, by the way, is "typing error"), this approach does not make sense. **Let the Parse parse only**.

The take-home message here is that **it's a good idea to divide a complex task such as source code compilation in multiple, well-defined steps**, rather than doing too many things at once.

If you try to compile that boilerplate code (you need to enclose it in a class definition, as per Java's requirement), here's the error you would get.

```
error: bad operand types for binary operator '+'
	String z = x + y;
	             ^
  first type:  int
  second type: Object
```

This type of errors are called **typing errors**.

Typing errors is just one example of things that neither the Lexical Analysis nor the Parser would catch. They must be caught by the Semantic Analysis. What else is there?

## Semantic Analysis: Catch Them All!

We have so far understood that the developer who wrote the source code to be compiled may have made a lot of different errors:

- Typos on the keyboard.
- Forgotten symbols, such as comma, semicolon, dot, parentheses, etc.
- ..., and many more!

While (1) is a typical example of errors caught by the Lexical Analysis, (2) is more often found by the Parser. The question I want to answer now is, what type of flaws in the source code are found in Semantic Analysis?

In fact, there's no definite answer to this question. Things can vary quite a lot based on the particular language specification. In other words, the Semantic Analysis module for a C compiler is definitely different from one made for a C++ compiler!

That said, there are some common patterns across many languages. For instance, Semantic Analysis pretty much always takes care of the following.

- Check that identifiers are declared before to be used in computations.
- Check that reserved keywords are not misused.
- Check that types are correctly declared, if the language is explicitly typed.
- Check that computations are type-consistent, wherever possible.

These are all good examples of nasty errors that would be very difficult to spot during Lexical Analysis or Parsing.

In addition to that, the most sophisticated programming languages support a handful of non-LL(1) constructs. But the Parser in their Compilers is almost always based on LL(1) algorithms. Therefore the task to analyze these more complex construct is delegated to Semantic Analysis.

The first two points above sound somewhat easy, don't they?

To check that variables and identifiers are well defined we can simply keep track of them in a table:

- We would add each of them into the table the first time we find it in the source code (which is read from top to bottom). 
- Then, when an identifier is used, we first check that is already in the table. If not, then... compilation error!

Things are not so simple, actually. But let's assume they are, just for a little while still. I will clarify this another article.

To check that reserved keywords are not misused seems like a fairly easy task, too. For example, the keyword `break` should only be used in a loop or in a if-else branch. How difficult can it be?

In fact, it's not too difficult as long as you make **clever choices in terms of data structure**. To decide, and to design the right data structure for your algorithms is a very important step. We are going to discuss about it in a minute.

The last two points in the list above deal with the subject of typing. This is a very important and complex topic, therefore I am going to dedicate to it an entire section in this article. Also, a subject that must be really studied, is **what data structure are to be used in Semantic Analysis**, in order to write good algorithms. I will also discuss this in a future article!

## References

Language Implementation Patterns. A book by [Prof. Terrence Parr](https://parrt.cs.usfca.edu/).

My article [Why There Are So Many Programming Languages?]({{< ref "/page/why-so-many-programming-languages" >}}) (Talks about front-end, back-end compilation).

My article [What is a Programming Language Parser?]({{< ref "/page/what-is-a-programming-language-parser" >}}) (If you don't know yet what a Parse Tree is).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

Wikipedia page about the [Builder Software Design Pattern](https://en.wikipedia.org/wiki/Builder_pattern).
