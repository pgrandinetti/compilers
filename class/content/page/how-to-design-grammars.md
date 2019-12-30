---
title: "How to Design Formal Grammars"
date: 2019-10-10
draft: false
---

In a previous article of this series we answered basic questions about Formal Grammars:

- What are they?
- How are they related to Programming Languages?
- What are they made of?

Then, we quickly got a glimpse of more profound questions, such as:

- Are all Grammars the same?
- What differentiates one Grammar from the others?
- How can we build a new Grammar?

I did answer them, but in a brief way. To summarize the main points:

- Grammars are not all the same. Of course.
- The way you design the grammatical rules is also what determines the expressivity power of a Formal Grammar. So, ultimately, expressivity power is what differentiates one Grammar from another.
- To build a Grammar one must first decide the Alphabet (the set of allowed characters), and then write the production rules.

Now, it's time to go deeper into the details and build a real knowledge around these concepts.

Given that the way you write down the rules is so important, someone in the past has taken care of studying and grouping the sensible ways in which you can build a Grammar.

His name is **Noam Chomsky**, he's an american linguist and he defined the so-called Chomsky Hierarchy, which I am going to talk about next in this article.

At is turns out, the way you write down the rules defines also the **expressivity power of the Grammar**, and different Grammars can be grouped in 4 Types, each type with a different power.

## The Chomsky Hierarchy

In my opinion, the easiest way to think about the Chomsky Hierarchy is like a Venn diagram.

Venn diagrams express relations between sets. For example, the set of natural numbers is a subset of (included in) the set of real numbers.

**The Chomsky Hierarchy is a Venn diagram between Formal Grammars**. More precisely, between four Types of Formal Grammars. These are simply called

- Type-0 Grammars,
- Type-1 Grammars,
- Type-2 Grammars,
- Type-3 Grammars,

although they also have some nicknames (which we are going to see).

One point I really want to remark is that the "subset"-like relation means expressivity, in the case of Grammars. Let me try to clarify this point, with the following example.

Consider the alphabet given by digits and lowercase letters: `{a, b, …, z, 0, 1, …, 9}`. Then, consider all possible strings that you can make with characters drawn from this alphabet, of any length. They are a lot! Infinite, actually. Let's call the set of all such strings `S1`.

Now, consider the alphabet given by digits, lowercase and uppercase letters: `{a, b, …, z, 0, 1, …, 9, A, B, …, Z}`. Then, consider all possible strings that you can make with this alphabet. They are infinite too, like the previous case. Let's call this set `S2`.

Let's think more deeply for a moment. The set `S2` ought to be larger than `S1`! They are both infinite in number, but we can definitely say that `S2` contains all strings that are in `S1`, plus many more.

Thus, in a Venn diagram we would put `S1` within `S2`, to say that `S1` is contained in `S2`. We can also say that **S1 is a subset of S2**.

With Formal Grammars the logic is similar. In fact, Grammars are basically constraints over the strings that you can make with the given Alphabet. Instead of having "all possible strings you can form given a alphabet", you will have some limitations on how you can form valid strings.

Therefore, two Grammars specify different sets of limitations. And hence, the sets of sentences you can make with them can be in any of the Venn-type relations.

In particular, the Chomsky Hierarchy is made by **4 Types of Grammars that are recursively included each into another**.

1. Type-0 Grammars are the most general. In other words, they have the least constraints.
2. Type-1 Grammars are a bit more constrained than Type-0, and are a subset of them.
3. Type-2 are even more restricted, and are a subset of Type-1 (and therefore also of Type-0).
4. Type-3 have the lowest expressivity power. Or, that's the same, the most constrained form of rules. They are included into Type-2 (and therefore into Type-1 and Type-0 too).

Let's now take a closer look at each of these types. Before that though, let me add two pieces of notation that are worth remembering:

- A _terminal_ token (or terminal symbol) in a grammar G is a word that cannot be expanded in further levels. For instance, the word `for` is a terminal token in many programming languages.
- A _nonterminal_ token (or nonterminal symbol) is a part of the grammar that can be expanded further. For instance, the `keyword` in the previous example is nonterminal, because it can be expanded by replacing it with `for`, or `if`, or `else`, or `while` (and many others, in fact). Same for the equality statement.

Also, let me introduce the general concept of Grammar, before going into each type of grammar.

_A Grammar G consists of:_

1. _A finite set N of nonterminal symbols, disjoint with the set of strings that G can form._
2. _A finite set T of terminal symbols, disjoint with N._
3. _A finite set P of production rules, dictating how symbols are replaced to form a valid string._
4. _A start symbol s that belongs to N._

Fix in your mind the name **production rules**, which is the correct name for "replacement rules" I improperly used before.

At this point I believe we have all the pieces of knowledge we need for a deeper study of Grammars!

## Type-0 Grammars

Let's start with the most general Grammars. Remember that "most general" also means "less constrained", and also means **higher expressivity power**.

In other words, **Type-0 Grammars are the largest set in the Chomsky Hierarchy**, and they can express the largest set of languages. Such a set is a superset of the languages that you can build with the other types of Grammar.

What characterizes the Grammars of Type-0? Answer: _The way production rules are written_. In a Type-0 Grammar, each production rule has the form

```
p : q
```

where `p`, `q` are sequences of either terminal or nonterminal symbols, and `p` contains at least one nonterminal.

As you can see, there is **basically no restriction** on how you can write a production rule for a Type-0 Grammar. This is why such grammars are **also called Unrestricted Grammars** (which, in my opinion, is a much easier name to remember!).

### Turing-digression

Feel free to skip this small section and jump to Type-1 Grammar!

A well-known and beautiful result states that **Type-0 Grammars are equivalent to Turing Machines**. In different words, given a Type-0 Grammar `G` and the language it generates `L(G)`, there exists some Turing machine capable of recognizing `L(G)`, and vice versa.

To me, that seemed like a surprising result! In fact, the Turing machine that accomplishes this task is quite easy to imagine. Let `w` be the input sentence that we have to test (that is, accept or reject). Let's imagine a 2-tapes non-deterministic Turing machine where the first tape contains `w` and is not modified, and the second tape also contains `w` but is used for computations. The machine does the following steps:

1. Start at the beginning (left side) of the second tape
2. Choose (non-deterministically) a production rule `p : q` from the grammar `G`.
3. If `p` appears somewhere in the second tape then replace it with `q`, possibly filling empty space by shifting the other characters on the tape.
4. Compare the sentence on tape 2 with `w`. If they are equal then accept `w`. Otherwise, go back to step 1.

It's quite easy to see that the non-deterministic step 2 will branch many times, so producing all possible sentences that the grammar `G` can generate. The machine will accept the input if any of its branches accepts it, therefore it will accept it if any sequential application of production rules produces a valid string.

On the other hand, if you think about this Turing Machine for a minute, you will see that it's possible it never stops. This is exactly what happens for so-called **undecidable problems**: the input is accepted if the Turing machine stops, which makes the problem itself recursive enumerable. But the machine might never stops, which also makes the problem "undecidable".

Famous examples of undecidable problems are the halting problem and the Post correspondence problem. Just to throw in a formal definition: a formal language is said _recursively enumerable_ if it is a recursively enumerable subset in the set of all possible sentences over the alphabet of the language or, equivalently, if there exists a Turing machines which will enumerate all valid sentences of the language.

If the above sentence sounds mysterious, don't worry, it is not critical for understanding this article and programming language Grammars.

At this point we know that **Turing machines, undecidable problems and Type-0 Grammars express (more or less) the same languages**.

## Type-1 Grammars

This type of Grammars is also called **Context-Sensitive Grammars**. The reason for this name is that the way you are allowed to write production rules is to specify what symbols are around the symbol you want to replace. These additional symbols are seen like the "context" around the symbol to be replaced.

The proper form of the production rules is the following:

```
pAq : pgq
```

where `A` is a nonterminal symbol, `p, q, g` are strings of terminals and/or non terminals, and `g` must be non-empty.

When you think about it, the above rule says that the symbol `A` can be replaced with the string of symbols `g`, but only if `A` appears between the `p` and `q` strings of symbols. That's the "context" of `A`.

The most famous example of language that can be generated by a context-sensitive grammar (and so it's said _context-sensitive language_) is

$$
L = { a^nb^nc^n \, |\, n \geq 1}
$$

which is the language that contains all strings made in the following way:

- the character `a` repeated any number of times (at least 1 time),
- followed by the character `b` repeated the same number of times,
- followed by the character `c` repeated the same number of times.

Valid sentences in such language are

```
abc
aabbcc
aaabbbccc
```

At this point, the natural question is what can Type-0 Grammars do, that Type-1 can't?

The answer is relatively easy: there exists some language that you cannot express with a Type-1 Grammar, whereas you can with a Type-0 one. 

For example, if you take any undecidable problem (such as the Post correspondence problem), and write the formal grammar that produces valid sentences for that problem, then you would see that such a grammar is Type-0. And **you would not find any way to do the same with a Type-1 Grammar**.

**This restriction is reflected in the form of the production rules**, too. In the Unrestricted Grammars, productions can have basically any form `p : q`, where `p` and `q` are strings of terminals and/or nonterminals symbols and `p` contains at least one non-terminal. On the other hand, in Type-1 grammars, the productions are restricted to the form `pAq : pgq`.

Once again, **more constraints means more restrictions**, which means less expressivity power. And therefore, the whole set of languages that can be built with all Type-1 Grammars is just a subset of the one generated by Type-0 Grammars.

## Type-2 Grammars

This is a very important class of Grammars, from practical point of view. In fact, **this set is where most programming languages grammars belong to**.

Type-2 Grammars are **also said Context-Free Grammars**. As opposed to Context-Sensitive Grammars, in the Context-Free ones the context around a symbol does not play a role in the production rules.

This is a big limitation, and in fact **Context-Free are less powerful than Context-Sensitive Grammars**, yet they are the most used in programming languages definition.

The general form that production rules can take in Context-Free Grammars is

```
A : g
```

where `A` is a nonterminal symbol and `g` is a string of terminal and nonterminal symbols. Moreover, `g` cannot be empty.

You can almost "see" that Context-Free Grammars are more restricted than the previous two types, because of how production rules look like: you must have a unique symbol on the right-hand side of the rule, and it must be a nonterminal.

The classical example of language that is formed by a Context-Free Grammar (which is said _Context-Free Language_) is

$$
L = { a^nb^n\, |\, n \geq 1}
$$

Valid sentences in this language are

```
ab
aabb
aaabbb
```

The question is why this language is Context-Free, whereas I earlier said that $$ L = { a^nb^nc^n | n \geq 1} $$ is Context-Sensitive?

Again, the answer lies in the restrictions and the constraints applied to the production rules.

To express a language with three "balanced" characters (such as "aabbcc") you will need to write down production rules that use context. You will NOT be able to write a Context-Free grammar that does it. Therefore, the language is context-sensitive, but NOT context-free.

To make this point clearer, remember that, instead, **all Context-Free Grammars are also Context-Sensitive**, whereas the vice versa is not true.

## Type-3 Grammars

This is **the last layer in the Chomsky Hierarchy**, and therefore the most constrained class. It is also the least powerful class. And also the class that can express the least number of languages. Just repeating the concept in different wordings!

The production rules in these grammars have either of the following forms:

```
A : aB
A : Ba
```

where `A, B` are nonterminal symbols and `a` is a terminal symbol. A Type-3 Grammar must have all production rules either in the first form, or in the second form. The two forms cannot be mixed in the same Grammar: it wouldn't be a Type-3 Grammar then.

In fact, Grammars of this type are very famous: they are **also called Regular Grammars**. The languages they can express are called _Regular Languages_, and a very well-known formalism to handle them is via **Regular Expressions**, which you have surely used before.

A classic example of regular grammar is

```
S : aS | bA
A : cA | eps
```

where `eps` is the empty string. This grammar is equivalent to the regular expression

```
a*bc*
```

Indeed, the language formed by such a grammar consists of all strings with any number of `a`'s, followed by one `b`, followed by any number of `c`'s.

This grammar is said _left-regular_. Regular grammars can also be right-regular, for instance

```
S : Sa | Ab
A : Ac | eps
```

which is very similar to the previous grammar, but inverts the order of the characters and is equivalent to the regular expression

```
c*ba*
```

## Take-home messages

At this point, after many technical details and strange notations, I feel like it is worth summing up a bit of everything and clearly stating the most important things to remember.

We studied what Formal Grammars are and what they are used for in programming languages:

- Formal Grammars are set of rules. These rules define how symbols (words, strings, etc.) can be mixed to form longer sentences that are "valid" in each Grammar.
- A Grammar also defines a Language. The Language L(G) of a Grammar G is the set of all valid sentences.
- The most critical part of a Grammar is the set of production rules. These rules say how to replace nonterminal symbols in a sentence, with either terminal or other nonterminal symbols. This is done recursively, until the sentence is made by only terminal symbols (in which case the initial sentence is accepted), or the procedure is stuck (in which case the initial sentence is grammatically wrong, and hence rejected).

Then, we studied in detail the Chomsky Hierarchy:

- There are 4 Types of Grammars in this hierarchy, from the most powerful one (Type-0, also said Unrestricted), to the least powerful (Type-3, also said Regular Grammars). The other two are called Context-Sensitive (Type-1) and Context-Free (Type-2).
- These four types are subset each to another: Type-1 is a subset of Type-0; Type-2 is a subset of Type-1 (and hence of Type-0); Type-3 is a subset of Type-2 (and hence of Type-0 and Type-1).
- In the Grammars world, "more powerful" means less constrained and therefore capable of expressing more languages. The constraints are about how production rules can be written.
- Type-2 Grammars, the Context-Free Grammars, are the ones used to create new programming languages. The reason is that there are efficient algorithms to parse grammars of this type.
- Type-3 Grammars, the Regular Grammars, have application in a very wide range of subjects, and are very often implemented as Regular Expressions.

## Common issues in Formal Grammars

If everything went smoothly so far, then I might have given you the wrong impression. To **properly define a useful Grammar is quite a task**.

The problem is that, unless you are very careful at how you define each production rule, chances are you will end up with a unparsable Grammar, for example a Type-0 Grammar. It would certainly be powerful.. in theory! In practice, you could not write a general parser for it, and therefore nobody could write a program in its language. Or maybe you could write one (e.g., the Earley algorithm), but it would be so inefficient that nobody would not use it.

There are many tricks and pitfalls that one must be aware of, when defining a Grammar. I am going to discuss with you two common issues.

### Ambiguity and Operators Precedence

Let me start by making one point clear: **ambiguity is BAD**.

A Grammar is ambiguous when the same input sentence can result in two (or more) different parse trees. This is just too much for a compiler, because it would have to guess which of the two parse trees must be translated into executable code.

In fact, ambiguity is a much more common issue than you would think. Let's consider a simple grammar that defines addition and multiplication between integer numbers, where `int` is the only terminal token.

```
E  : E + E
   | E * E
   | (E)
   | int
```

Basically, this Grammar says:

_an expression is either the sum of two expressions, or a multiplication of two expressions, or an open parenthesis followed by an expression followed by a closed parenthesis, or an integer number._

So far, so good. Now let's consider the input sentence

```
9 * 5 + 2
```

This simple input has two parse trees. One where the first rule applied is `E*E`, and one where it's `E+E`.

It's worth noticing that the problem is not having two different sequence of derivations that lead to the same parse tree. This would not be a problem at all. The issue is that there are two completely different parse trees for this input, and both are valid.

In fact, there are many tricks that can help solve this problem in a Grammar. For example, in this case, one must add a production rule in order to clarify whether the `+` or the `*` has the priority. The new Grammar would be

```
E  : E' + E
   | E'
E' : int * E'
   | int
   | (E) * E'
   | (E)
```

The new Grammar is **ambiguous-free**, and does what we intuitively want: prioritize multiplications over additions.

### Left Recursion

Guess what: **left recursion is quite BAD**. Perhaps less bad than ambiguity, but you should really avoid left recursion too.

Let me explain what it is with an example.

Suppose you want to define a simple grammar for all sentences that start with the character `b` and continue with as many characters a as desired. You could write the following.

```
S : Sa | b
```

In theory, this works. A possible derivation could be:

```
S → Sa → Saa → Saaa → ... → Saa...a → baa...a
```

However, in practice, if you want to parse the input sentence and build the parse tree from the leftmost character to the rightmost one, then you will have an infinite loop. Why? Because you start by calling some procedure that parses according to the definition of `S`, and the first thing it does is to search for one more `S` (hence, calling the same procedure).

With a bit of general notation, the grammar

```
S : S a1 | … | S an | b1 | ... | bm
```

is a left-recursive grammar that produces all strings starting with one character among `b1, ..., bm` and then follows with any number of characters among `a1, ..., an`.

To make it non-left-recursive, one can rewrite it as

```
S : b_1 S' | ... | b_m S'
S' : a_1 S' | ... | a_n S' | eps
```

which produces exactly the same set of strings (can be easily proved) and is not left-recursive.

There is a trickier case that can lead to left-recursion, when the recursion is not immediate. For example, the following grammar has a non-immediate left-recursion issue.

```
S : Aa | b
A : Ac | Sd | eps
```

Why non-immediate? Because the left-recursion becomes evident only after more than one derivation, in this case `S → Aa → Sda`.

There are ways to get rid of this left-recursion type too. But for this, I will just refer you to the Dragon Book (see references at the end) and the Algorithm 4.19: Eliminating Left-Recursion therein.

For the sake of clarity, I also want to mention that left-recursion is an issue only in top-down parsing algorithms, whereas it's not a big deal in bottom-up parsing. The article about parsing algorithm is for the next time, though!

## What's next

When I first decided to dive deep into the Formal Grammars world, my motivation was because I wanted to build my own programming language. There are a lot of tools out there that allows you to build a new programming language in a semi-automatic way, but I wanted to go down deep till the bones of the subject.

Of course, a complete knowledge of everything I said in this article is not needed for the most part of your daily practical tasks (unless you work on Domain Specific Language application).

Nonetheless, I do believe that having a broader knowledge about programming languages help coders, software engineers and architects and, why not, well-rounded scientists in making a better job.

Plus, it's truly a fascinating subject!

## Rerefences

The Dragon Book. Compilers: Principles, Techniques and Tools. [Wikipedia page](https://en.wikipedia.org/wiki/Compilers:_Principles,_Techniques,_and_Tools) (of the book).

The Chomsky Hierarchy, [Wikipedia Page](https://en.wikipedia.org/wiki/Chomsky_hierarchy), as starting point to follow many links inside it.

