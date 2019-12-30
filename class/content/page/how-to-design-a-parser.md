---
title: "How to Design A Programming Language Parser"
date: 2019-10-11
draft: false
---

In a previous article of this series we've studied Programming Language Parsers and answered the basic question: what are they?

We learnt that:

- Parsers are algorithms that decide whether or not a source code is correct with respect to the rules of a Grammar.
- Parsers get in input the list of Tokens produced by the Lexical Analysis, and output the yes/no decision, while building in memory a Parse-Tree.

And we also learnt what are the main features that distinguish parsers:

- Top-down or Bottom-up.
- Leftmost vs Rightmost.
- With or without backtracking.
- With or without recursion.

In order to successfully build a Parser, it's worth taking as much time as we need for the design part, all while keeping the previous information on the top of our heads.

The goal is to make **informed decisions** about each of the above features. So, let's see how to design a Parser.

## Choosing The Parsing Algorithm

Before to start, remember that my ultimate objective is to learn so much about this subject that I can confidently build my own Programming Language, with its Compiler and hence its Parser.

In fact, **I want to guide you through the decisional process I went through myself** when I designed the Parser for my language.

Let me start with a bold sentence: _writing a parser is a scientific task as much as a coding task; therefore, unless you're exploring new research horizons, stick to the state of the art algorithms and don't make a home-made design._

I am not saying that you should dry out the creative effort, not at all. What I am saying is: _let scientific theory guide your efforts_. Let me give you a few concrete examples.

To start the design of my parser, I could jump straight into writing utterly complex regular expressions and a gigantic loop that reads the source code, until it reaches the End Of File, and works on a case by case design, like in a huge if-then-else. And, maybe, it will work. Maybe it won't.

Instead, I am going to stop for a second and ask myself: What kind of parser do I want to build, top-down or bottom-up?

I know that bottom-up parsers are a bit more powerful, but they also are much more complex. My objective is to fully understand a parsing algorithm in order to make my own, and then integrate it into my own compiler. Therefore **I am going to choose a top-down design**.

Then, I could open my text editor and start typing some code and... see what comes out of it. Instead, I am going to think for one more second and ask myself: Do I want to build a parser based on backtracking, or on lookahead?

Backtracking is a very interesting approach, but I am already familiar with it, since I studied it in the past in reasonable depth. Moreover, as I told you, backtracking parsers are a less common choice because their runtime can be very high. Therefore, **I am going to design a LL(1) parser**. Make no mistake about it: this choice implies that the parser will only work with LL(1) grammars.

I also decided to take a leftmost approach, as this is the most common choice.

Finally, there is the question about recursion. This is another question you should ask yourself at the start, because it influences all the upcoming development.

In fact, this was the toughest choice for me, because my objective was to learn as much as I could and both approaches (recursive and iterative) are really worth exploring. Thus, I chose both! **I am going to start with a recursive design**, and later learn how to implement the iterative equivalent of it.

Let's recap. My choice is to build a top-down, recursive, LL(1) parser that uses leftmost derivations.

## A Short Tour In The Definitions

Remember that _the parser parses the grammar_. NOT the language. This means that you must look for the grammar definition and analyze it in quite some depth, in order to build your parser.

Since I want a LL(1) parser, we can start reasoning about how to implement the lookahead. Theoreticians like to define two sets, called **First** and **Follow**. For a given symbol (or a sequence of symbols) $X$, here's the definition of First set:

$$
\text{First}(X) = \\{ t\, |\, t\, \text{is terminal}, X \rightarrow^* ta\\} \bigcup \\{ \epsilon | X \rightarrow^* \epsilon \\}
$$

Quick bit of notation: the symbol $\rightarrow^*$ means that what's on right side of it can be achieved from what's on the left, in any number of derivations. "Derivation" is to apply one production rule of the Grammar.

What does the definition say? It says that $\text{First}(X)$ is the set of all terminal symbols that appear at the beginning of the string after an arbitrary number of derivation. Also $\epsilon$ belongs to $\text{First}(X)$ if $X$ derives the null string.

The Follow set is defined only for a terminal symbol. So, here's the definition for a terminal $A$:

$$
\text{Follow}(A) = \\{\, t\, |\, t\, \text{is terminal}, S \rightarrow^+ bAtd \\} \bigcup \\{t\, |\, t=EOF, S\rightarrow^* aA \\}
$$

It says that Follow($A$) is the set of terminals that can appear immediately to the right of $A$ in some partial derivation (with at least one derivation step - that's what $\rightarrow^+$ means). Furthermore, if $A$ can be the rightmost symbol in a derivation, then EOF is in Follow($A$).

Now that we have seen these definitions, I can also give you a **more concrete view** about why LL(1) grammars are a restricted subset of all grammars. In fact, a Grammar is LL(1) if, for every production rule `A : p | g`, the following three conditions hold true:

1. There isn't any terminal symbol a such that both `p` and `g` derive strings starting with `A`.
2. At most one between `p` and `g` can derive the empty string $\epsilon$.
3. If `g` derives $\epsilon$, then `p` does not derive any string that starts with a terminal in Follow($A$). Likewise, if `p` derives $\epsilon$, then `g` does not derive any string that starts with a terminal in Follow($A$).

I hope you made it till this paragraph, despite the odd-looking definitions above!

The truth is that those definitions are really important from a conceptual point of view, but here is what I think:

_In practice, though, it's easier to simply ask ourselves, What tokens can possibly start phrases beginning at this alternative? The exact definition of FIRST is a bit much to chew on for our purposes here[...]_ (cit. Prof. Terrence Parr in his book about language implementation, see the references at the end).

## Prototyping The Parser

Finally, we are going to see how to implement a parser algorithm!

Bear in mind that I am only going to discuss the strategic, high-level aspects of the implementation. Basically, the implementation design.

Regarding the actual code, it will have its space in the D section of my Compiler's class: **Do it**.

To start off, remember that we are going to build a top-down, LL(1) (thus, with lookahead and without backtracking), recursive Parser.

Also, I think that it is worth looking at one example that clarifies the concept of LL(1), versus LL(k). In practice, LL(1) parser are those that can always take deterministic decision by just looking ahead one single token.

This feature, clearly depends on the grammar. So here's an example of LL(1) grammar:

```
exp : ID
    | '-' ID
    | '+' ID
```

This grammar can easily be parsed by a LL(1) parser, because when the parser needs to decide what to do with an `expr` (expression) token, it can look ahead one token: if it's `ID` then only the first production rule can be right; if it's `-` then only the second choice is valid; and if it's `+` then only the third one makes sense. If the lookahead token is none of the three, then a parsing error will be returned.

Let's consider instead the following grammar:

```
expr : ID '++'
     | ID '--'
```

A LL(1) will not be able to parse strings in this grammar, because knowing that the next token is `ID` simply does not suffice to the goal. The parser would need to know also the token after that one. In fact, this is an example of LL(2) grammar.

Notice, however, that the grammar in the example could be left-factored and made LL(1), by rewriting it as:

```
expr : ID ( '++' | '--' )
```

Left-factoring Grammars is not something I am going to talk about in this article, therefore we can assume that we want to build a LL(1) parser because _we know that the grammar is LL(1)_.

At this point, all information are in your brain, hopefully in your working memory, and we can get started.

The first thing to know, which helped me a lot, it to understand the overall structure of the software that we are going to write. In fact, a parser like the one we want to build should respect a very well known structure, that is basically a **software design pattern**. Let me give more details.

As we know, all types of Tokens in a Grammar can be classified into two sets: terminal tokens and nonterminal tokens. The key concepts to know are the following ones:

- For each Token we are going to call a procedure (call it "function", or "method", if you like).
- Procedures for the nonterminal tokens simply call the procedures for the tokens in the production rule to be applied.
- There is only one procedure for all terminal tokens, that simply matches the current input character(s) with the token type.

An example will make everything easier to figure out. Let's consider this grammar:

```
expr : ID ('=' rhs)?
     | NUM
rhs : ID | NUM | expr
```

It says that an expression can either be a `NUM` (a number), or it can be an `ID` (identifier) optionally followed by a `=` symbol and a `rhs` (right-hand side of the expression). Such `rhs` token must be one between `ID`, `NUM` and `expr`.

For instance the following strings are all valid expressions in this grammar.

```
5 // a single NUM is an expression
var1 // a single ID is also a valid expression
var = 5
var1 = var2
```

Due to the recursive nature of the definition, even the following string is valid

```
var1 = var2 = var3 = var4 = 5
```

Here's an example of non-valid string, that must be rejected:

```
var1 = var2 =
```

That one is non-valid because the `=` symbol MUST be followed by a `rhs`.

The grammar is clearly LL(1), because when we are parsing an `expr` token, knowing whether the next token is `ID` or `NUM` is enough knowledge to pick the correct production rule.

So, let's start by considering the terminal tokens. As I said, there's only one procedure for them; let's call this procedure `match`

```C
void match(int type_enum)
{
    ...
}
```

What is the argument `type_enum`? It's an integer, that in a Object-Oriented Language like Java would be instance of `Enum`. Basically, a finite set of integers is used to identify one-to-one all terminal token types.

In the current example, we would have that

- `int 0` matches the token type `ID`
- `int 1` matches the token type `NUM`
- `int 2` matches the symbol `=`. Let's call it `EQUAL`

and these are the only terminal tokens that we have.

The procedure match is really simple. Let's assume that we have a global pointer to the next token in the input sequence, call it `lookahead`. Let's also say that the sequence of tokens is an array of pairs (`token_type`, `token_chars`), so that we can access the two elements of the pairs for each token in the array.

```C
void match(int type_enum)
{
    if (input_sequence[lookahead].token_type == type_enum)
        consume();
    else throw_error("Some message");
}

void consume() {
    lookahead++;
    if (lookahead >= input_sequence.length)
        accept(); // input sequence is accepted
}
```

What happens in the code above? When `match` is called, it checks that the next token is indeed of the type that we expect to see. If it's not, then a parsing error is generated (with a meaningful message, hopefully). If it is, then we call the `consume` procedure, that just advances the global pointer to the next location.

Keep in mind that **in addition to advancing the pointer you may also want to build the parse tree in memory**. This is why I suggest to have a separate procedure, `consume`, so that you can change its behavior without touching ever again the `match` procedure. For the time being, let's keep consume very simple and just incrementing the pointer.

What about nonterminal token types? As I said, for each of these types we are going to have a separate procedure. In the example, the only nonterminal is `expr`, so we only need to write one function.

How do procedures for nonterminals work? For LL(1) parsers this is going to be a piece of cake:

- Write a long `if-else` for each of the possible lookahead tokens that's possible to have, or a `case-switch`, if you prefer.
- Each of these cases corresponds to a production rule. So, for each of them recursively call the procedures defined for the single pieces of that production rule. This will be clear with the example below.
- As "default" case in the `if-else`, generate an error, because it means the lookahead token is not valid in the grammar.

Alright, now let's see the procedure for the `expr` token type. As a convention, it makes sense to call the procedures for the nonterminals with the same name of the token type.

```C
void expr(){
    if (input_sequence[lookahead].token_type == NUM)
        match(1); // 1 is the integer for NUM
    else if (input_sequence[lookahead].token_type == ID){
        match(0); // 0 is the integer for ID
        if (input_sequence[lookahead].token_type == EQUAL){
            // now call procedures for each piece
            // of this production rule: expr ← ID '=' rhs
            match(2); // 2 is the integer for EQUAL
            rhs();
        }
    }
    else generate_error("Helpful message");
}

void rhs(){
    if (input_sequence[lookahead].token_type == NUM)
        match(0);
    else if (input_sequence[lookahead].token_type == ID)
        match(1);
    else if (input_sequence[lookahead].token_type == expr)
        expr();
    else
        generate_error("Message");
}
```

There is one crucial factor into play: **the lookahead point is a global variable** that is updated (globally) each time `match`, and therefore `consume`, is called. This is extremely important for the overall strategy to work. Even if you don't like global variables (I don't either), all serious programming languages have constructs that allow this behavior.

To see why this is so important, just take a look at the nested if in the `expr` procedure, the one that checks

```C
input_sequence[lookahead].token_type == EQUAL
```

This `if` instruction is called right after we called `match(0)`, that did update the `lookahead` pointer. As I said, it's crucially important.

The second most important thing to notice is that the procedures for nonterminals invoke each other. So, the `expr` procedure might call the `rhs` procedure, if it matches successfully an `ID` token first, and an `EQUAL` token in second place. And the `rhs` procedure might even call again `expr`, if that's what the lookahead suggests.

How can this work? How do we make sure that we are advancing in the input sequence while doing all these recursive calls?

Again, it's because before calling any nonterminal procedure we always first consume the current token by invoking match, which advances the global lookahead pointer. Did I say that it is very important?

With some adjustment to this boilerplate code, and some more code to handle global pointers, input sequence and error messages, you'd have a pretty much complete parser for the grammar I wrote as an example!

Obviously, for any real grammar, one that makes sense, there would be many more procedures to create for nonterminal, and many more if-else to write.

As you can imagine, this task can be quite tedious and error-prone. Nonetheless, I believe this is the type of task that one has to do "once in a lifetime", to understand the concepts in the deepest possible details. After that unique time, it makes sense to use tools that automate it (such as ANTLR).

## What's next

A Parser is the second step in the process of compiling a source code. It comes after the Lexical Analysis, and before the Semantic Analysis.

Building parsers is a complex task, that has been heavily studied from theoretical point of view, along with Formal Grammars.

In this article I showed you how to design a parser, and, perhaps more importantly, what are the strategic decisions that must be taken before going into the coding effort. In particular, we also saw the template code for a LL(1), top-down, recursive-descent parser.

The other top-down, LL(1) alternative is an iterative approach, that uses a "parsing table" and a stack-like data structure to imitate recursion. Such alternative approach is more involved but can have some advantages in terms of memory management, and I will talk about it in a companion article.

If you're not too interested in the theoretical details of parsing algorithms, then the choice is clear: jump onto the next step -- Semantic Analysis!

## Resources

My article [What is a Programming Language Grammar?]({{< ref "/page/what-is-a-programming-language-grammar" >}}).

Language Implementation Patterns. A book by Prof. [Terrence Parr](https://parrt.cs.usfca.edu/).

The Dragon Book. Compilers: Principles, Techniques and Tools. [Wikipedia page](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools) (of the book).
