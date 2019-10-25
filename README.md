# Compilers
_My open course on Compilers, Grammars and Programming Languages_

This repository contains the code I wrote for my _self-inflicted_ class about Compilers <https://plumebear.com/project/compilers/>.

**Use it at your risks**.

## About the project

As side project for a MOOC on Compilers I built a new (basic) programming language. Turing-complete, but without supports for many things.

This repo contains the Grammar's specifications, as well as the compiler for this new language, written in C and organized in 4 modules:

- Lexical Analysis
- Parsing
- Semantic Analysis
- Code Generation

## Compile!

1. Compile my Compiler! You'll need a C comiler, e.g. `gcc main.c cgen.c parser.c lexer.c`
2. Write your program in _my language_ and place it in a text file. An example is offered in the repo with the file `code.e`.
3. Now compile your program with `./a.out ./code.e`.

The result is a Python executable script, by default `./out.py`. You can optionally run `./a.out ./code.e /home/user/result.py`, to specify the path for the output file.

## Question?

- Why generated code is in Python?
- What is _my language_?
- How does the Compiler work?

Answers: <https://plumebear.com/courses/compilers/>
