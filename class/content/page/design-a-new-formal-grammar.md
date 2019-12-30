---
title: "Design of a New Context-Free Grammar"
date: 2019-10-13
draft: false
---

We studied Formal Grammars in some of the previous articles in this course, _What is a Grammar in programming languages_ and _How to design formal Grammars_. The most important concepts we learned are:

- There are 4 types of Grammar in the Chomsky hierarchy.
- Type-2 Grammars, also said Context-Free Grammars, are the ones used to design new programming languages.
- One must avoid many pitfalls while creating a new Grammar, for example left-recursion and ambiguity.

In this article we will take all those concepts and put them into practice: I will show how I designed a new Context-Free Grammar. This Grammar is the basis of the new Programming Language I created as a learning tool for this course.

## Preliminaries

The first baby-step in the design of a Formal Grammar is to decide on the Alphabet, and the set of characters allowed. If such terminology sounds a bit confusing, I can’t help but suggest you to read a bit via Wikipedia, or the previous articles in this course (see the references at the end).

Now, one thing I always had as objective while working on the design part was the simplicity of the programming language. **I wanted to create a very direct programming language**, almost “explicit” if you will, and without overloading it with fancy statements or complex expressions. This has been the main guideline during the whole process.

Also, to start off, I decided to focus on a small set of instructions. That means the initial version of the language (that’s what I will show in this article) does not allow for OOP, routines, coroutines, etc.. Of course, I understand that routines are an extremely important part of a language, and **I will definitely add them at a later stage**.

Also, I wanted the language to be explicit but not too verbose. Therefore I decided to make it non-explicitly typed. So, for example, you wouldn’t write `int x = 5`, you would just write `x = 5`.

However, I do want the language to be aware of types, also at compilation time, therefore **it will be a language with static and implicit typing system**. Based on that, the compiler will also check for typing-errors, such as adding a string to an integer (more on this in a future article).

At the theoretical level, I wanted to design a LL(1) grammar, so that I can also write my own implementation of a LL(1) recursive descent parser.

If you need to check what these naming conventions mean, start from my previous article _What are the differences between programming languages_, or google them!

Wherever possible, I tried to add some small nuances into the formal grammar, to give a personal touch to the whole. Having fun was one of the goals too!

## Characters, Numbers and Strings

To start with, I knew I would need all basic blocks such as digits and letters in the Alphabet. Thus, I defined the following regular expressions as part of the Grammar.

```
digit0      : 0-9
digit       : 1-9
letter      : a-z | A-Z
otherChar   : |!”%&/()=+-*#><;,^.][\n \t
char        : letter | otherChar | digit
```

Let’s take a moment and think, because the choices I made have already some implications. First of all, the distinction between all digits, and the subset of them without the zero, is a bit redundant, but I found it useful (as I will tell you later).

In second place, the underscore (_) and the single quote (‘) are missing from the set of allowed characters! Why is that?

Regarding the underscore, things are quite simple: I don’t think it’s needed. The CamelCase convention (where variables are named like _thisIsAVariable_) is good enough in my opinion, and also very clear.

What happened to the single quote, instead? It will be clear in a moment.

With that (almost) clarified, let’s continue from the simpler to the tougher, and take a look at how I defined the numeric objects -- integer and float types.

```
num         :   ('+' | '-')? float
float       :   (frac exp?) | (int frac? exp?)
int         :   '0' | digit+ digit0*
frac        :   '.' int
exp         :   '^' ('+' | '-')? int
```

Just one word before a full explanation: I used all over the grammar the single quotation mark to delimit parts of the grammar. So, if I write `‘+’` it means that there must be a plus symbol for that token to be valid. That’s partially why the single quote is not a valid character in the Grammar, so there’s no confusion about it.

Okay, what is happening in the last few definitions? First, I defined a `num` (numeric) grammatical token. This is made by a sign (either plus or minus) followed by a `float` token. Notice also that the sign is optional, and, as you would guess, the number is positive by default when the signed is omitted.

What’s the `float` token? I defined it as a choice between two cases: it can either be a `frac` (fractional part), followed by an optional exponent part, or it can be an integer token, optionally followed by a fraction and also an optional exponent.

Remember that the pipe `|` means OR (either of two), and the question mark `?` after a parenthesis means that the group of characters in parentheses is optional. Also, the star `*` after a parenthesis means that the group of characters in parentheses can be repeated zero or more times, while the plus `+` means the group can be repeated one or more times. Don’t get confused between `‘+’` and just `+`. The former means the symbol ‘+’, whereas the latter is a notation in the regular expression syntax.

My definition of float is not extremely elegant, but it’s very explicit. In particular, it makes clear that you don’t need to start with an integer if you want to write a floating point number. You can just start with the dot, like in the number `.25`, which is interpreted as `0.25`.

What about the integer token? I very simply defined it either as a single `‘0’`, or as a sequence of digits that does not start with zero. So, in my language you could write `x = 0` to define a new integer, and also `x = 1`, but you could not write `x = 01`. Quite standard, isn’t it?

I defined the fractional part also simply as a dot followed by an integer. So far, so good. You could write `x = 1.25`, and `‘1’` would be the integer part, while `‘.25’` would be the fractional part, made by a dot and another integer.

Lastly, the exponent. Here I introduced a small novelty. The vast majority of programming languages uses the exponential notation with the `E` (or `e`) letter. I decided to use the caret `^`, like scientists do on the calculator. A small nuance, nothing more. After the caret you have an optional sign (plus by default) followed by an integer. So, `x = 1.25^5` would be a valid `num` token in my language. And its meaning it’s the same as in languages where you use the `E` notation: `1.25` times `10` at the power `5`.

The other building block a language truly needs are strings, or characters array at least. I defined string in the following way.

```
str         :   quotedStr ('+' quotedStr)*
quotedStr   :   ' " ' (char)* ' " ' (',' obj)*
```

The basic unit of a string is a “quoted string”. This is what you would actually call string in other languages, that is a sequence of characters in between double quotes.

So, for example, `"abc def”` is a valid `quotedStr` token in my Grammar. However, my definition is a bit more complex, and let you create strings the C (or Python) way. Basically, each such sequence of characters can be followed by a comma `,` and an object `obj`, possibly repeated. This is to allow string interpolation. Here’s one example:

```
“Abc %d defg”, 5
```

As you can see, there a single pair (comma, object) after the closing double quote. So, if you are familiar with other programming language, you already know that the `“5”` will be interpolated where the `“%d”` is in the string.

The following string, with two pairs of (comma, obj), is also valid according to Grammar:

```
“The number %d is greater than %d”, 5, 4
```

And this explains why I wrote `(‘,’ obj)*` in the Grammar: to allow for multiple interpolations (zero or more).

Having said that, the definition of string in the Grammar is just a `quotedStr`, optionally followed by a plus sign `+` and another `quotedStr`, etc. This way you can concatenate strings as in the following example:

```
“first “ + “second is %d”, 2 + “hello, third”
```

You have probably noticed that I didn’t define explicitly the `obj` token used in the definition of quotedStr. We’ll do it soon.

## Other basic types

The choice of what basic types are to be supported in a language is an important one. It will affect forever how people write code in your language. Can you imagine what would Python be without lists? What if C had built-in dictionaries?

I decided that **I wanted to support lists**, in this new programming language. The Python way, to be clear. I find lists really explicit and handy to use. However, I also want to encourage good programming patterns, therefore I will force lists to contain only objects of the same type.

I have seen enough of bad programming practice that mixes different object types in the same container, and want to avoid that to ever happen in my language. This last constraint, however, is not a syntactical one, rather is a semantic constraint, so the Formal Grammar is not concerned by it.

I also wanted to add a boolean type, and the `NULL` type. Finally, I defined the “variable” type to be any sequence of characters that is not a reserved keyword.

```
obj         :   var | num | str | bool | null | list
bool        :   'True' | 'False'
null        :   'NULL'
var         :   letter (digit | letter)*
list        :   '[' (listExpr)? ']'
listExpr    :   obj (',' obj)*
listElem    :   var '[' int ']'
```

Now the `obj` definition is clear. A `obj` token is either a variable, a numeric, a string, a boolean, a list or even `NULL`.

A detail to note is that variables (names) can only be made of letters and digits. As I already said, no underscore or other strange things!

The grammatical definition of list is more interesting. First of all, you must have a open square bracket `[`. Then you have an optional token, called `listExpr` (list-expression), followed by a closed square bracket `]`, that is not optional.

First remark: the definition implies that you can write empty lists: `[ ]` is a valid expression according to the grammar.

The `listExpr` token is not very surprising. It’s made by a `obj` token, optionally followed by a sequence of pairs `(comma, obj)`. This definition allows to stack up in a list as many obj tokens as you wish.

A second remark is worth noting here. My definition of list forbids to end a `listExpr` token with a trailing comma. As you may already know, in Python you can have a trailing comma in a list, like in `[1, 2, 3, 4,]`, and the Python interpreter will take care of that final comma for you. In my language, instead, if you have a trailing comma like that you will receive a compilation error.

After some thought I decided to include another “type”, among the basic one. I called it `listElem` (list-element), and it simply identifies the natural (and explicit!) way you would index an array, or a list in this case. It’s made by a var token, which should always be the name of a list object, and a pair of square brackets with an integer token between them.

Lots of words just to say that `x[1]` is a valid expression in the Grammar, and it just means to take the element in position `1` in the list identified by the variable `x`. Standard stuff. Are we sure?

## My first mistake

I must be honest and say that as far as numeric, strings, boolean and other basic types were concerned, my work went smoothly. Surely because these are very standard grammatical constructs, and I studied them both in the MOOC and on my reference book.

With lists things got a bit trickier. The list definition itself was not complicated, but **I had to think about all its possible implications**.

That’s when I made my first mistake. It’s was even more than “just” a mistake, it was a misunderstanding on an important concept. And that’s what makes it even more important: I think to go over my (wrong) reasoning, and see how I corrected it, **can be very beneficial to other students**.

I initially included the listElem token among the choices for the obj token. So, the definition was:

```
obj   :   var | num | str | bool | null | list | listElem
```

I wanted it to be that way because I thought having the `listElem` like one of the basic types would be useful. However, that led to a very subtle issue. Let me illustrate it.

As we will see later in this article, the Grammar will also include a “output statement”, that has to be interpreted like the “print” function in Python, or the “printf” in C. In my Grammar, I defined it as made by the keyword `writeOut` followed by a `obj` token.

So, for example, if `x` is a token `obj` (that means, a `var`, or a `string`, etc.) you could write `writeOut x`, and that will compile correctly, and will print at screen the value stored into `x`. Formally, the definition is

```
output     : ‘writeOut’ obj
```

Here’s where the problem comes. Or rather, where I thought it came.

If `listElem` was part of the `obj` token choices, then there would be two cases where the keyword `‘writeOut’` could be followed by a `var` token:

1. When it’s really just a `var` token.
2. When it’s the var token that starts a `listElem`.

This means **the Grammar would not be LL(1)**, because after the parser grabs the token `‘writeOut’` and looks ahead, and sees a `var` token, it wouldn’t know whether it’s just a `var` or a `listElem`. It would actually need to look further ahead, but **I didn’t want to design a LL(2) Grammar**.

If you are wondering what happens in similar cases in widely used languages, such as Python, just know that common Programming Languages have several constructs that are not LL(1), and are handled ad-hoc by the compiler.

I spent some time brainstorming about this issue, and eventually decided to exclude the `listElem token` from the obj token types.

So, I thought I had solved the issue. Instead I was wrong. In fact, **I was wrong twice!**

I got the whole (and right) picture only much (much) later, while I was working on the Semantic Analysis module of the language. This is why I am telling you about the entire process -- I think it can be a very helpful learning experience.

While working on a later module of the Compiler, I realized what was wrong in my reasoning. Let’s put ourselves in the position of the Parser, that is currently parsing an Output statement, such as

```
writeOut x[1]
```

where `x` was previously defined as a list, for example

```
x = [1, 2, 3]
```

What will the parser do? I was wrong (again!) in saying that when it gets the `'writeOut'` token, it will have to look ahead two tokens to understand whether it’s just `'x'` (list) or it’s actually `'x[1]'` (listelem).

**Why was I (doubly) wrong?**

The reason is that after getting the `“writeOut”` token the Parser will only need to know that a `obj` token is after that. Therefore it won’t look ahead at all, not even 1 step. It will simply delegate the task to some sub-module that has to parse a `obj` token.

What will happen next, then? This sub-module (that I will show later), will now point at the `'x'` token. Only now it will look ahead 1 token, and just 1 token, to figure out if it’s a `var` or a `listElem`. At this point looking ahead one token only is more than enough. Therefore **the Grammar is still LL(1)**, and the final `obj` definition is:

```
obj     :   var | num | str | bool | null | list | listElem
```

## Operators

After the somewhat intricate definitions above, I needed something easier to work on! So I defined, in a “almost standard” way, the logical operators allowed in the Grammar. Here they are.

```
operator  :   '+' | '-' | '*' | '/' | '/.' | '%' | condOp
condOp    :   '&&' | '||' | '!=' | '==' | '<=' | '>=' | '>' | '<'
```

These are very normal definition, with the exception of the token `'/.'` -- this is something I added because I wanted to distinguish between the standard division operator, that truncates the result when the operands are integer numbers. So, `3/4` will yield `0` (integer division), but you can write `3/.4` and it will yield `0.75`.

Wait, **there’s actually a blind spot here**! What if I write `3/.4` but what I really mean is `3` divided by `0.4`? Well, in this case you will need to add one space, because the characters are read from left to right, so if the dot `.` is found immediately after the slash `/`, then it will be interpreted as a floating division `/.`.

You would write `3/.4` if you mean “3 divided by 4, without truncating the result”, while you’d write `3/ .4`, if you mean “3 divided by 0.4, with integer truncation” (which is, by the way, quite a strange thing to do).

At any rate, I do recommend the Python convention to add spaces around the operators. So, just write `3 /. 4` and everything will be more readable. Or also `3 /. .4` if you want to compute the floating point division between 3 and 0.4.

Speaking about this, for a long time I considered to forbid floating point to start with a dot. I wanted to force the usage of zero, so you’d never write `.4`, and always `0.4`. I decided not to, eventually, for consistency with many (many) other languages. Nonetheless, I do encourage programmers to type that zero on their keyboard!

## Expressions

Expressions are a fundamental block of any programming languages. Actually, as you should know if you read my previous articles, the ability to write balanced expressions (with balanced parentheses) is exactly what makes Context Free Grammars more powerful than Regular Expressions, and give them the power to design entire languages.

This is also why Expressions, and in particular Arithmetic Expressions, are the single most used example in textbook about Formal Grammars. Therefore, I pretty much already knew what to do here. My only challenge was to make sure the definition included the other basic types of my Grammar, and not just numeric (like in arithmetic expressions).

```
expr        :   term ( ('+' | '-') expr)?
term        :   baseExpr ( ('*' | '/' | '/.' | '%') term)?
baseExpr    :   obj   |   '(' expr ')'
```

I suggest you pay attention to the patterns in the definition above, because they are standard and thus **very important to know**. Let me illustrate them.

There are **three basic blocks**, which you can think of as three rings in a chain: the general expression `expr`, the inner term `term`, and the basic expression `baseExpr`. They are somehow ordered, in the sense that the definition of `expr` involves the definition of `term`, and the definition of `term` involves the definition of `baseExpr`. Finally, `baseExpr` can either be a basic `obj` token, or a whole new expression, but only if it’s enclosed in parentheses.

This way of defining expressions ensures a very important thing: **the Grammas is not ambiguous**. Ambiguity is really bad for Formal Grammars, and is to avoid with all means.

In the case of expressions, ambiguity can come from the operator precedence. If you write `5 + 3 * 2`, there must be one and only one way this expression is interpreted by the Grammar (and hopefully is the one that prioritizes the star `*` operator over the plus `+`).

The recursive definition with three basic blocks (Expr, Term, baseExpr) is guaranteed to be unambiguous, so it’s worth sticking it to our mind.

## Program and Statements

Expressions were the last among the building blocks. With that done, I decided to jump right at the top of the Grammar and clarify what the whole Program will look like.

Once again, I took a very explicit approach to this design challenge, and I told myself: _A Program will simply be a sequence of statements, that I will call Lines, and each Line must be followed by a Endline token_.

The concept of Line/statement includes really all types of statement. So, a simple assignment (e.g., `x = 5`) will be a `line`, and also a long and complex if-then-else will be just one `line`. That also implies that a Line must be able to contain more Lines.

Here’s the definition of Program I came up with. You can think of the Program token as if it’s used to represent the entire source code.

```
program   :   (line ENDLINE)*
ENDLINE   :   ';' '\n'
line      :   assign
          |   input
          |   output
          |   ifLine
          |   loopLine
          |   breakLine
          |   contLine
```


The first grammatical rule says a `program` is a (possibly empty) sequence of pairs `(line, Endline)`. As already said, a `line` can be anything from an assignment operation `assign`, to a complex loop instruction `loopLine`. You should really think of a Line token like a “logical line”.

The `Endline` token instead is a very simple token meant to clarify the end of a logical line, and it’s made of two characters: a semicolon and a newline. I believe this a small novelty that I introduced with respect to other languages. Let me explain why.

In all languages I know, you can put two logical statements on the same physical line, for example:

```
x = 5; y = x + 2;
```

is a valid string in Python, C, Java, etc., and it consists of two assignment operations.

In my Grammar instead, **physical lines and logical lines are terminated together***, because the Endline token contains a `‘\n’`. Thus, the previous example would give a compilation error, and you must instead write

```
x = 5;
y = x + 2;
```

I believe this improves the readability of the code, and in fact programmers are supposed to follow this practice even if it’s not enforced by the language… just that sometimes they do not!

It is worth noting that the `Endline` token is the only one where the semicolon appears. Therefore, you can’t really write a semicolon in my language if it’s not followed by a `\n` character.

Now, let’s stop one more minute here to think about the implications brought by the concept of logical Lines. Let’s start with an example of valid Program:

```
<Program>
  <Line>
    <Assign>
  <Endline>
  <Line>
    <ifLine>
  <Endline>
```

This `program` corresponds to a source file made of just two lines, the first being an assignment and the second being a `ifLine`. It could look like the following excerpt:

```
a = “a valid string”;
if (a != NULL)
  inner-Line1;
  inner-Line2;
;
```

It’s really important to understand that the first `line` ends at the second double quote (`...string”`) and it’s correctly followed by a `Endline` token. Then, the second `line` starts and it’s a `ifLine`. As we will see in a moment, the `ifLine` can contain multiple Lines, each terminated by a `Endline`.

What I want to underline is that **the ifLine must be terminated by a Endline token too**. This is the semicolon that you see as the last line in the example.

Let me repeat the concept again. The last semicolon in the example is (part of) the `Endline` that terminates the `ifLine` started with `if (a != NULL)`.

This way to terminate complex instructions, such as a `if` block, is slightly uncommon, because in most languages you open a curly bracket at the beginning of the `if`, and then “close” such a bracket when the if-body is done, as in the following code

```
a = “a valid string”;
if (a != NULL) {
  inner-Line1;
  inner-Line2;
}
```

On the other hand, and to be absolutely honest, my idea of “logical lines” is not a new one. There are already a few languages using it. Python has the same concepts, and implements it with an INDENT token. Indeed, if you know Python then you know that indentation plays an important role in it.

Other languages terminate the if statements with the `end` keyword, which is the closest to my choice that you can think of. Matlab is an example of such languages.

I must say **I am very happy with this design choice of mines**. To terminate complex blocks with a semicolon feels really natural once you grasp the concept of logical lines, and I think the simple, lonely, semicolon at the end of the block it’s quite nice looking. And don’t forget there’s an invisible `\n` next to it!

At this point, let’s see the grammatical definition for each of the possible Lines, starting with the ifLine.

## Conditional statement - if

The conditional line (statement, if you will) is meant to represent branches in the code, such as the if-then-else statement. Thus, my definition for it is quite simple.

```
ifLine   :   'if' ifCond ifBody
```

Basically, the `ifLine is a sequence of three elements.

- First, a constant token, `“if”`.
- Then a token called `ifCond`. This is meant to represent the logical condition that, if True, will let the flow “enter” the if.
- Then, a token called `ifBody`, which hopefully is superfluous to explain.

Now, it’s time to recursively define the `ifCond` and `ifBody` token, to complete the definition of `ifLine`. Let’s start with the ifCond token.

### My second (bigger, and most useful) mistake

To define a sound syntax for the `if` statement was a tricky affair. It wasn’t all clear to me until I wrote the code for the parser. I do understand the details now though, so this is also a very good lesson to share.

In my mission to keep things explicit, I initially defined the ifCond as it follows

```
ifCond      :   '(' obj condOp obj ')'
```

It made perfect sense to me, at the beginning. Open a parenthesis, create a conditional expression with two `obj` token (that is, two primitive types of my Grammar) and then close the parenthesis.

Unfortunately, I soon realized this was way too restricted. With this syntax you can’t even write

```
if (x + 1 == y)
```

because the first part `x+1` is not just a token `obj`, it’s a whole expression. Therefore, I understood that I could not keep things too restricted in the `ifCond` and decided to change it to the following.

```
ifCond      :   '(' expr condOp expr ')'
```

This sounds right, isn’t it? The `expr` token gives a lot of flexibility and all you have to do is to write two expressions with a conditional operator (`==`, `!=`, `&&`, `<`, etc.) in between them.

For example, you could now write

```
if (x + 1 == y + z)
```

and the Grammar would be fine with it. I thought I nailed it.

Not quite.

As I kept digging deeper (again, I understood all this later, while I was writing the parser) I realized the latest definition of `ifCond` didn’t work well.

To start with, I couldn’t use more than two conditions, such as in

```
if (boolean1 && boolean2 && boolean3)
```

This could be solved by rewriting it as

```
ifCond  :  '(' expr (condOp expr)+ ')'
```

so that we could use as many boolean expressions as we wanted. But then, when I thought about this, I realized there was **a much bigger issue in my design**. 

What about the following line:

```
if (x + 1 == y && boolean1)
```

This is a perfectly fine, intuitive way to write a piece of code and it should really be supported by a good Grammar. However, notice that in the parentheses there is one conditional operator `==` used as part of the expression `x+1==y`, and another one `&&` used to link and evaluate two expressions.

This clarifies that there was **a major flow** in my design, at this point. But luckily, **I also figured out the source of the problem**.

The problem lied in a simple fact: I was not able to use conditional operators in the logical expressions!

After I realized this single thought, everything became clearer soon. In fact, in many languages, **expressions have a value**. In C, for example, you can write `int x = a && b;` and the result of the expression `a && b` is assigned to the variable `x`.

This also allowed me to understand that I didn’t need to design the `ifCond` as a condition between two expressions. One expression alone is enough, because it’s evaluated no matter how complex it is. And, as a convention, there is a defined set of basic values that evaluate to “false” (e.g., `0`, `NULL`, `False`, and a possibly empty list).

Thus, I was finally clear about the way to go. I had to redesign the `expr` token to include conditional operators and then the ifCond will simply be

```
ifCond  :  '(' expr ')'
```

That single expression will be evaluated, and its value will be the condition to enter the if branch or not.

In hindsight this all looks pretty obvious now, but it totally wasn’t for me!

Okay, so, **how to modify the grammatical definition of expression token?** We understood in a previous paragraph that operators priority is implemented with a chain of tokens, such as the chain made by `expr`, `term`, `baseExpr`.

Now, if I want to add conditional operators and assign them a different priority then **I need to add a new ring to the chain**.

I decided this was the case, because in general conditional operators have the lowest priority, at least in my intuition. Let’s take an example. When I write

```
if (x + y <= 3)
```

I really mean that I first want to compute the sum `x+y` and then check if it’s smaller or equal than `3`. This means that the conditional operator `<=` has a lower priority than the plus `+`. And plus is already at the lowest priority (less than multiplication and division, for instance).

Sure, I could use more parentheses. If I wrote

```
if ((x + y) <= 3)
```

then it would be all good, but it definitely seems unnecessary to do so.

So, all considered, I added one more element in the recursive rule for the `expr` token and called it `pred` (as for logical predicate). Since this is the lowest in the priorities, it will be at the top of the chain.

Here’s the new grammatical rule for the `expr` token.

```
expr        :   pred (condOp expr)?
pred        :   term ( ('+' | '-') pred)?
term        :   baseExpr ( ('*' | '/' | '/.' | '%') term)?
baseExpr    :   obj    |   '(' expr ')'
```

By comparing this with the previous (flawed) rule, the design pattern is very clear and easy to understand.

One last thing worth noticing. I didn’t differentiate between conditional operators. It means that, for example, `&&` and `<=` have the same priority. As a consequence, if you write

```
if (x <= y && boolean1)
```

it’s not guaranteed which of the two logical expressions will be executed before. It could either be `(x <= y) && boolean1`, or it could be `x <= (y && boolean1)`. This is not a case of ambiguity, it’s a design choice. I know the first case is far more common, yet all logical operators are the same to me, so in similar case I want you use parentheses.

Alright! With this troublesome issue solved, now it’s time to breath again!

### Back to the if statement

Let’s complete the definition of ifLine by specifying what the ifBody is.

```
ifBody   :   (line ENDLINE)+ (optElse)?
optElse  :   'else' (line ENDLINE)+
```

At this point, the above two definitions should be very easy to understand. The `ifBody` token is defined as a non-empty sequence of `(line, Endline)`. The last of these pairs may be followed by a `optElse` token (optional Else).

The `optElse` token is made of the keyword `“else”` followed by a non-empty sequence of `(line, Endline)`.

I think this is quite intuitive. Basically, both the if and the else branch are sub-Program tokens.

Here is how a simple and valid if-then-else would look like in my Grammar.

```
if (expr1)
  line1;
  line2;
else
  line3;
;
```

Mind the final semicolon!

Nested if statements would be written like this one:

```
if (expr1)
    line1-1;
    line1-2;
else
    if (expr2)
        line2-1;
        line2-2;
    else
        line2-3;
        line2-4;
    ;
;
```

Writing a sound grammatical rule for the conditional statement (ifLine) was definitely the piece of work **I learnt the most from!**

## Loop statement - while

With the `ifLine` out of our way, I believe the loop statement will not require any comment!

Here’s the definition.

```
loopLine    :   'while' ifCond loopBody
loopBody    :   program
```

Just notice that I am using again the `ifCond`, as well as the whole program token. Pretty useful, isn’t it?

## Input, Output, Assignment

The `line` definitions that we are still missing to complete the `program` token are those related to I/O and assignment. Luckily, I kept these very simple and, once again, explicit.

```
input    :   ‘readIn’ var
output   :   'writeOut' obj
assign   :   var '=' expr
```

Notice that `“readIn”` and `“writeOut”` are reserved keyword. I will leave the understanding of these three simple definitions as exercise for you!

### My third (but not last!) mistake

Here’s another thing I got wrong about the Grammar. It’s going to be the last mistake for this article, but definitely not my last one in the whole Compiler’s studies!

As I said at the beginning, my objective is to build an implicitly typed programming language. This means that the Compiler knows about types, but it’s able to understand them all by itself. The programmer does not need to declare types explicitly.

For example, you would simply write

```
x = 5;
y = NULL;
```

And the Compiler would understand that `x` is integer and `y` is a null type. You do not need to write

```
int x = 5;
Object y =  NULL;
```

like in Java, C++, etc. What’s even more important is that, if later in the program you write

```
z = x + y;
```

The Compiler will remember the types `x` and `y` were assigned to, and will tell you that the operation `+` is not allowed between these two types. And the compilation will stop with an error.

So, my language is statically typed, but implicitly, which saves some time in typing on the keyboard.

It’s worth underlining that this is a very different behavior from what we see, for example, in Python, Perl, and other scripting language. There, when an operation is not possible between two types, the error is generated at runtime, not during the compilation (which often does not even exist, in scripting languages).

Type-checking is part of the Semantic Analysis, and this is why I didn’t immediately realize that I made a mistake at this point. The problem is with the Input Line.

If all we declare is `readIn var`, how could the Compiler possibly know what’s the type of the newly assigned variable? It just can’t.

That’s true in many languages. For example in Python3, you would do

```
x = input()
```

And `x` would automatically given a `str` class (because Python is Object Oriented), regardless of what you enter. Even if enter just digits, such as `1234`, and wanted to have an integer variable, `x` will still be a string.

You are required to do an explicit type cast, if you want an integer. Something like

```
x = int(input())
```

Type cast is a very weird beast in my opinion. It’s totally unsafe, and can potentially lead to crashes at runtime. Still, it’s very common especially in interpreted languages.

At any rate, since types are implicit in my language, it didn’t make sense to support type casting. I would also have to introduce all keywords reserved to types.

I therefore decided to modify the Input Line. The new definition is

```
input   :   readIn var
```

where `readIn` is not a keyword anymore, it’s another token type, defined as it follows.

```
readIn  :   'readInt' | 'readFloat' | 'readBool' | 'readStr'
```

Like that, when you write an Input Line you will also have to declare what’s the input type, for example

```
readInt x;
```

and the Compiler will assign type int to the identifier `x`. Obviously, it does not make sense to have a `readNull` keyword.

## Break and Continue

In the `program` token, as you’ve already seen, I also added the `break` and `continue` instructions, that are quite useful when managing control flow. I kept them as last ones because they are the easiest!

```
breakLine   :   'break'
contLine     :   'continue'
```

## Summary

At this point all pieces of the Grammar have taken their places. Here’s how the entire thing looks like.

```
digit0     : 0-9
digit      : 1-9
letter     : a-z | A-Z
otherChar  : |!”%&/()=+-*#><;,^.][\n \t
char       : letter | otherChar | digit

num         :   ('+' | '-')? float
float       :   (frac exp?) | (int frac? exp?)
int         :   '0' | digit+ digit0*
frac        :   '.' int
exp         :   '^' ('+' | '-')? int

str         :   quotedStr ('+' quotedStr)*
quotedStr   :   ' " ' (char)* ' " ' (',' obj)*

bool        :   'True' | 'False'
null        :   'NULL'
var         :   letter (digit | letter)*
list        :   '[' (listExpr)? ']'
listElem    :   var '[' int ']'
obj         :   var | num | str | bool | null | list | listElem
listExpr    :   obj (',' obj)*

program     :   (line ENDLINE)*
ENDLINE     :   ';' '\n'
line        :   assign
            |   input
            |   output
            |   ifLine
            |   loopLine
            |   breakLine
            |   contLine

input       :   readIn var
readIn      :   'readInt' | 'readFloat' | 'readBool' | 'readStr'
output      :   'writeOut' obj
assign      :   var '=' expr

expr        :   pred (condOp expr)?
pred        :   term ( ('+' | '-') pred)?
term        :   baseExpr ( ('*' | '/' | '/.' | '%') term)?
baseExpr    :   obj   |   '(' expr ')'

operator    :   '+' | '-' | '*' | '/' | '/.' | '%' | condOp
condOp      :   '&&' | '||' | '!=' | '==' | '<=' | '>=' | '>' | '<'

ifLine      :   'if' ifCond ifBody
ifCond      :   '(' expr ')'
ifBody      :   (line ENDLINE)+ (optElse)?
optElse     :   'else' (line ENDLINE)+

loopLine    :   'while' ifCond loopBody
loopBody    :   program

breakLine   :   'break'
contLine    :   'continue'
```

I must say I do like the result. First of all because is a neat and quite simple Grammar. Sure, at the moment it does not allow for many important things, such as OOP and routines. I will definitely add routines at some point down the road.

Despite that, this Grammar is LL(1), a feature I really wanted so that I can further learn by implementing a LL(1) recursive descent parser. Moreover, the Grammar does allow for constructs such as input, output, conditional branching and loops, therefore the generated Language is Turing-complete.

## A Complete Example

Although this work is still far away to be a new Programming Language, I want to show you how a meaningful source code would look like.

As an example, let’s consider every student’s favorite exercise: _read an integer number N and compute the total sum of the first N prime numbers_.

Here’s how I would implement this in Python (> 3.6) and in my own language.

```python
# Python3.6
N = int(input())
total = 0
for i in range(2, N + 1):
    isPrime = True
    for j in range(2, int(i/2) + 1):
        if i % j == 0:
          print(f'{i} is not prime')
          isPrime = False
        if not isPrime:
            break
    if isPrime:
        total += i
print(f'Total primes sum until {N} is {total}')
```

```
# My language
readInt N;
totSum = 0;
i = 2;
while (i <= N)
    j = 2;
    isPrime = True;
    while ((j <= i / 2) && isPrime)
        if (i % j == 0)
            writeOut "%d is not prime", i;
            isPrime = False;
        ;
        j = j + 1;
    ;
    if (isPrime)
        totSum = totSum + i;
    ;
    i = i + 1;
;
writeOut "Total primes sum until %d is %d", N, totSum;
```

## Conclusions and Outlook

The design of a formal, LL(1) Grammar for a new Programming Language is not only a fun task, but it’s also very instructive.

A Grammar itself though, while fun to design, is not very useful. To take this one step further and build a new Programming Language out of it, as we already know, all the other main modules of a Compiler must be built: Parsing, Semantic Analysis and Code Generation.

## References

Language Implementation Patterns. A book by Prof. [Terrence Parr](https://parrt.cs.usfca.edu/).

Alex Aiken's class Compilers at Stanford University. Available as [free MOOC online](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/).

My article [What Is A Programming Language Grammar?]({{< ref "/page/what-is-a-programming-language-grammar" >}}).

My article [How to design Formal Grammars]({{< ref "/page/how-to-design-grammars" >}}).

My article [What are the differences between programming languages?](({{< ref "/page/what-are-the-differences-in-programming-languages" >}})).

Python Lexical Analysis [documentation page](https://docs.python.org/3/reference/lexical_analysis.html).

My [GitHub repository](https://github.com/pgrandinetti/compilers) for this course.
