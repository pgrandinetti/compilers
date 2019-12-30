---
title: "What Is A Programming Language Grammar?"
date: 2019-09-30
draft: false
---


If you study, or work on a subject that is IT-related, then you've heard of Programming Languages. And, as you know for sure, that's all about this series of articles is.

I bet every now and again you've heard or read the expression the Language's Grammar. What does it mean?

In this article, we are going to learn what a Formal Grammar is, in the context of programming languages and compiler's theory.

## Definition of Grammar

A Programming Language Grammar is **a set of instructions** about how to write statements that are valid for that programming language.

The instructions are given in the form of rules that specify how characters and words can be put one after the other, to form valid statements (also called sentences).

## Understanding the definition

Let's start by stating the obvious: in a programming language you cannot type characters randomly on your keyboard and hope something correct will come out of it. You must follow some rules.

Broadly speaking, **a Grammar is the definition of such rules**.

For example, you can imagine there's a rule in Java's Grammar called "the equality condition rule". This rule says 

_an equality condition is written in Java language by writing the name of an expression, followed by the `==` special symbol, followed by the name of another expression._

Then, there must be another rule that says what an expression is. And so on for each logical piece of the language (expressions, variables, keywords, etc.).

Looking deeper down, you can expect that **the rules of a grammar start by defining very high-level type of sentences, and then go to more and more bottom levels**. Let me use again the previous example to clarify this concept.

We know there is a rule about the equality condition. It says an equality statement must be expression, followed by `==`, followed by another expression. We can write is as

    eq_statement : expression '==' expression

This the highest level of sentences.

However, the rule is not complete in itself: the Grammar must also specify what an `expression` is.

Let's assume an expression can be a variable, or a string, or an integer. Of course, real programming languages have a more complex definition of expression, but let's continue with the simple one. So we would have

    expression : variable | int | string

You can see that the character `|` is used to express the inclusive logical condition (OR). The Grammar is not done yet, because it will also have to clarify what a `variable` is.

So, let's assume a variable is a sequence of characters in a-z, A-Z and digits in 0-9, without length limitations in the number of characters. You would express this with a _regular expression rule_, which is

    variable : [a-zA-Z0-9]*

The `*` symbol after the square brackets means you can repeat each of the symbols zero or more times.

Again, real programming languages have more complex definitions for variables, but I am hoping this simple example clarifies the concept.

There are still two pieces of a previous rule that haven't been specified yet: `int` and `string`.

Let's assume that an `int` is any non-empty sequence of digits, and string is any sequence of characters in `a-z`, `A-Z`. Of course in real applications you would have many more characters (for instance, `?!+-`, etc.), but let's keep it simple for the moment. So, you are going to have two more rules:

    int : [0-9]+
    string : [a-zA-Z]*

At this point we've gone the deepest you can in the definition of an equality condition. Why? Because you cannot expand further these rules. We say that single characters such as `a-z`, `A-Z` and digits `0-9` are the **terminal symbols** or, more formally, these are the characters of the Alphabet of the Grammar.

So, let's recap, from a different angle, which will help seeing things more clearly.

**You first decide on an Alphabet**. Alphabets are just set of characters, and in our case it would be `{a, b, …, z, A, B, …, Z, 0, 1, …, 9}`.

Then, you define the rules that state how you can concatenate characters in the alphabet to make words and sentences (statements) that are valid in your language. Eventually, **the whole set of rules defines the Grammar**, which generates the Language.

I want to go back to the example to discuss a few things worth mentioning.

If you count backwards, we had three levels to specify: equality statement, expression and variable/int/string (the last three are at the same level, in a hierarchical sense).

It is worth noting that you could express the entire condition in one-go, and you still can, by replacing the rules in the lower levels inside the rules in the upper levels. To replace literally means to substitute the symbols in the right-hand side of a rule wherever the left-hand side of the same rule appears.

So, when you replace the rules about variable, int, string in the rule about expression you get

    expression : [a-zA-Z0-9]* | [0-9]+ | [a-zA-Z]*

Then, when you replace the rule about expression in the equality statement rule, you get

```
eq_statement : ( [a-zA-Z0-9]* | [0-9]+ | [a-zA-Z]* )
               "=="
               ( [a-zA-Z0-9]* | [0-9]+ | [a-zA-Z]* )
```

It really is just replacing definition into definition, in a **recursive manner**.

When the recursion is over, that is all replacements have been done, you can see what an equality statement really must look like to be valid in language that is defined by the grammar. The difficulty though, is that **real Grammars have dozens of nested levels** and therefore is better to express them in small pieces, rather than in a whole, giant, rule.

Thus, **a full Grammar is a (rather big) list of rules that specify the syntactic constraints you must follow in order to write valid sentences in that language**. From the highest level constructs, to the tiniest detail, Grammars are very formally specified object.

Before moving on, I believe one thing is worth pointing out. **Grammars are only about syntax**. Like in a human natural language, also in programming languages, Grammars will tell you if what you are writing is acceptable from syntax point of view.

But the Grammar rules _will not_ tell you whether what you wrote makes sense or not!

And they _will not_ tell you if what you wrote will really do what you meant to, once it's executed!

So, to wrap up, watch out for the (very important) difference between syntactic and semantic rules! Formal Grammars are about syntactic rules only.

## A broader view on Formal Grammar

At this point in the article, I bet you have a question. Or, well, many questions. Some are along these lines:

- Are all Grammars the same?
- What differentiates one Grammar from the others?
- How can I make a new Grammar?

Let's take them one at a time.

First of all, no, Grammars are not all the same. Of course, I would add.

The way you mix the single characters in the Alphabet is obviously a feature of each language. Now, as it happens, many famous languages have a lot of rules in common, by design (because this way it's easier to learn them).

For instance, variable _identifiers_ are pretty much the same in C, C++, Java, Python, and many other languages: digits, letters, plus many other symbols, with the first character that has to be a letter. And they cannot be equal to some keywords.

Keywords, on the other hand, vary from language to language. Keywords are also defined as a rule in the grammar, something along these lines:

```
keyword : 'for' | 'if' | 'else' | 'while' | ...
```

This a very important first distinction about Grammars. Let's say it again:

_Since Grammars define how characters in the chosen Alphabet are used, the same Alphabet can easily generate two different languages, if the two Grammars are different._

Now, given that the way you write down the rules is so important, someone in the past has taken care of studying and grouping the sensible ways in which you can build a Grammar.

His name is **Noam Chomsky**, he's an american linguist and he defined the so-called Chomsky Hierarchy, which I am going to talk about next in this article.

At is turns out, the way you write down the rules defines also **the expressivity power of the Grammar**, and different Grammars can be grouped in 4 Types, each type with a different power.

Finally, yes, of course you can write your own Grammar. Just decide what Type you want to be in, and follow the rules stated in the Chomsky hierarchy. Good luck with that!

## References

The Dragon Book. Compilers: Principles, Techniques and Tools. [Wikipedia page](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools) (of the book).

The Chomsky Hierarchy, [Wikipedia Page](https://en.wikipedia.org/wiki/Chomsky_hierarchy), as starting point to follow many links inside it.
