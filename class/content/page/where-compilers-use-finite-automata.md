---
title: "Where Do Compilers Use Finite Automata?"
date: 2019-10-11
draft: false
---

Finite Automata... What a fancy name!

Formally, **they are computational machines with a finite number of states**.

For instance, a digital watch counting only hours and minutes is a finite state machine, because it can only in one out of 1440 states (24 "hour state" times 60 "minute state" for each hour).

A washing machine can be seen as a finite state machine too. It starts in the "Ready" state, then goes onto the "Washing" state, followed by the "Drying" state. There may be more intermediate states, but the total number is certainly finite!

The more important question to me, now, is what's the role of Finite Automata in the process of building a new programming Language? **Where are Finite Automata used within Compilers?**

## Background

In the previous article in this course we learned a great deal about Regular Expressions. I warmly recommend you to read that article again (see the references at the end). The key concept, however, are the following:

- The second step of the compilation is called Lexical Analysis.
- Lexical Analysis takes care of transforming the original source code (that's a sequence of characters), into a sequence of substrings (called lexemes), each with its own class (such as keyword, operator, etc.).
- Regular Expressions are used to match each lexeme with one class.

I also showed examples regarding the last point. In summary, matching a lexeme to a class consists in the classic pattern-matching operation. For instance, you'd want to match the lexeme

```
"if"
```

With the pattern `keyword`, given by

```
"if | else | while | for | return"
```

In Python, you would do:

```python
import re
pattern = 'r^(if|else|for|while|return)$'
st = 'if'
print(bool(re.match(pattern, st)))  # >> True
```

The point where Finite Automata enter the Compiler's game is **the execution of Regular Expression**. Or you can say, in the implementation of regular expressions. Let's see it more in detail.

## How to implement a Regular Expression

First of all, what does it mean to "implement" a regular expression? It means finding a procedure to decide whether a given sentence is valid for a given regular expression.

Remember that **the overall objective is to accept or reject a sentence**. A sentence is just a string (a sequence of characters). Given a regular language defined by a set of regular expressions, for each new sentence we must decide whether it is valid (accept) or not valid (reject) for this language.

**Finite Automata is the most commonly used tool to implement regular languages**.

It's nearly impossible to give a detailed discussion about Finite Automata in this article (and it'd probably be confusing too), so I simply want to discuss the core of it.

A Deterministic Finite Automata (DFA) is defined by the following elements:

- An input Alphabet (set of characters).
- A finite set of states, $S$.
- A start state, $n$ that belongs to $S$.
- A set of terminal states, $F$ subset of $S$.
- A set of transitions $T$, that are rules about going from one state to another one.

A transition is a rule of the form "when in state s1, if you read character c1 move to state s2. If you read c2 then move to state s3." Additionally, and very importantly, a DFA can have **only one transition for each state for the same character**. This is what it makes it deterministic.

It's very common to use graphs like the one below to represent DFA.

![Example of Deterministic Finite Automata](/images/DFA.png)

The key element of this DFA are the following ones:

- The Alphabet is ${"0", "1"}$ (you must know it in advance).
- The start state is $n$, and it is indicated by the arrow entering it from nowhere.
- The terminal states set is ${f}$. These are indicated by the double circled notations.
- When in state $n$, if the char "0" is read then the machine goes into state $f$.
- When in state $n$, if the character "1" is read then the machine stays in the same state.

How does this DFA work? Let's assume we have the sentence "110" and must decide to accept or reject it. Let's see what happens step by step.

- The DFA starts in state $n$ and reads character "1" (the first in the sentence).
- It stays in the same state, as prescribed by the transition.
- It reads character "1" (the second in the sentence).
- It stays in the same state.
- It reads character "0".
- It switches to state $f$.

At this point the input sentence has been read and the machine is in a terminal state. So the sentence is accepted.

To be more formal:

- If the input sentence is entirely read and the machine ends in a terminal state, then the sentence is accepted.
- If the sentence is entirely read and the machine ends in a non terminal state, then the sentence is rejected.
- If the sentence is not entirely read because the machine is stuck at any point, then the sentence is rejected.

Let's consider the sentence "120".

1. The DFA starts in state $n$ and reads character "1" (the first in the sentence).
2. It stays in the same state, as prescribed by the transition.
3. It reads character "2" (the second in the sentence).
4. There is no transition for this character from the current state, so the machine is stuck and the input is rejected.

Let's consider "101".

1. The DFA starts in state $n$ and reads character "1" (the first in the sentence).
2. It stays in the same state.
3. It reads character "0".
4. It switches to state $f$.
5. It reads character "1"
6. There is no transition for this character from the current state, so the machine is stuck and the input is rejected.

So, the question is: **what does this DFA do in plain English?**

This DFA accept sentences of any length that are made by "1"s characters and have a "0" as last character only. So for example "10", "110", "1110" are all valid, but "101" and "1010" are not.

The key thing to know is that **Deterministic Finite Automata and Regular Languages are equivalent formalisms**. This means that:

- Given a DFA there will always be at least one Regular Language that accept the same set of sentences.
- Given a Regular Language there always is at least one DFA that accept the same set of sentences.

On the other hand, regular expressions have weaker expressivity than what you need to describe a full programming language. To handle that you need Context-Free Grammars.

Also, by construction of DFA is relatively easy to build a procedure that checks for acceptance / rejection of a sentence. It is basically as simple as one iterative loop over the characters of the sentence:

```C
int i = 0;
int curr_state = n;
char [] input = ... // some sentence
while (input[i]){
    curr_state = lookup_transitions_table(curr_state, input[i++]);
}
if (is_terminal(curr_state))
    ACCEPT;
REJECT;
```

This pseudo-code also lets you guess that the fastest way to implement a DFA in the memory of a real computer is not with a fancy-looking graph, but with a lookup table that maps transitions to states... but this is a story for another article.

## Wait! What about Grammars?

Right. I think I threw in a cryptic sentence a bit above: "Regular expressions have weaker expressivity than what you need to describe a full programming language. To handle that you need Context-Free Grammars."

What the heck does that mean?!

Like for Finite Automata, I am not going to go into a detailed analysis of Context-Free Grammars right now, so I will just leave you with a challenge.

Take a look at the following, simple, arithmetic expression.

```
(5 + (2 * (1+3)))
```

This is an example of a well-formed expression, meaning that it's correctly written and, in particular, parentheses are correctly closed.

To express well-formed arithmetic expressions that use only sum and products with a Context-Free Grammar does not require much effort:

```
expression : sum
sum        : product ('+' product)*
product    : value ('*' value)*
value      : digit | ('(' expression ')')
digit      : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
```

So, my challenge for you is the following. Given just two of the basic arithmetic operations, sum `+` and product `*`, write a regular expression that catches all well-formed arithmetic expressions. Let me know when you are done with it.

## References

My article [Where do Compilers Use Regular Expressions?]({{< ref "/page/where-compilers-use-regular-expressions" >}}).
