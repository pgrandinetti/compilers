---
title: "Why There Are So Many Programming Languages?"
date: 2019-09-28
draft: false
---

Why hasn't anyone invented yet a Programming Language that would **catch them all**?

Such question has stuck in my brain for quite some time, and none of the answers I found online were convincing. Then, I found an essay/video by Prof. Aiken, who teaches a class on Compilers at Stanford's Computer Science faculty.

His point was so interesting that I want to elaborate it further.


## The "cost" of a Programming Language

Let's start with a question that apparently hasn't got anything to do with the original question: What's the most expensive thing about a Programming Language?

Let's say you were to create a new Programming Language today, and finishing up its development by tomorrow (a bit optimistic, I know). Now, what would the most expensive, and most difficult thing you have to do?

Prof. Aiken makes a very interesting point here. He says that:

_The most expensive thing of a Programming Language is the time it takes to programmers to learn it._

If you work in more than one programming language, then you know it's a lot of time indeed. However, **when a language is young** and there are only few programmers using it, **it can evolve very quickly** because it's easy to keep just a few developers updated with the novelties. Also, young languages are backed by small but extremely tight communities. This is another big boost to its development.

![Language Complexity vs Time to Learn](/images/timeToLearn.png)

Think about well established languages, say Java and Python. For each of them there are hundreds of thousands of programmers using it, and therefore these languages evolve much more slowly. How much time there is between two consecutive Java, or Python, releases?

Thus, famous and solid programming languages are **slower to adapt** to new needs that arise from new technologies.

Therefore, quite paradoxically, **creating a new language is a faster way** to support such new needs, as well as gathering new users and creating a new community of programmers around the new language.

I'd like to share two examples from my own daily practice at work.

## A new language, just for statistician?

Let's consider R, the "language for statisticians". It's an extension of an old language, called S, created at the Bell Labs. R, instead, was originally implemented at the University of Auckland, New Zealand.

Would you say it is not possible to support statistics computation in Python, Java or C? Of course it is. And actually there's a well known package for Python, `scipy.stats`, oriented towards statistics.

However, adapting Java or C in order to make them easy-to-use for statisticians would require a lot of time. In this sense, the scipy.stats package is a perfect example: even though it works well (and let's assume it's well designed internally), its interface for the programmer (that is, the syntax and the libraries) is nowhere as neat as R's interface. I personally find `scipy.stats` really clumsy to use for statistics. R is almost perfect.

This is why R got a big traction (and quite some hype around it). All well deserved in my opinion. For example, it's the only programming language used in the fantastic Data Science specialization offered on Coursera by the Johns Hopkins University.

So, all good for a long while. What happened then? In my opinion, things went a bit out of control.

R's aficionados thought - Hey why don't we get out of the statisticians world? We can use R for a lot of other things!

The result was a bunch of pseudo- software products written in R. Today, you can really find anything in R: web servers, http and websocket clients, maybe even some GUI library.

The undeniable fact is that R wasn't designed for these applications. This, along with the fact that they weren't built by professional software developers, makes these software products not very well designed, again in my modest opinion.

At any rate, the simple evidence is that R has evolved into a **full-fledged programming language**, regardless of the quality of the libraries for things outside of the statistic scope. Hence, we have a new general-purpose programming language alongside the more classic C++, Java, etc.

If you think R's story is an exception, then think again! Lots of other Programming Languages have seen the same happening to them.

## A new language, just for automation?

A language I've worked with for a long time is Matlab. It's a proprietary programming language, owned by MathWorks, that was designed to seamlessly integrate with a set of other software tools developed by the same company. Main use cases: dynamic system simulations, automotive and feedback control.

Even though these applications are not too common within the Computer Science world, and even though Matlab is not too common of a language either, I think it's a good example in this context.

Matlab is, once more, in my opinion, not the best programming language out there, if used for general-purpose software development. Memory management, OOP, and many other features are almost senseless therein.

However, **it's just perfect for dynamic systems simulations and control applications**. And that's why is the de-facto tool in this industry.

If you think that numpy + scipy is a good competitor, then, well, I bet you've never really used Matlab.

Matlab's story is quite similar to R's. A great product, enormously useful to people working in the related field. MathWorks built something really great.

But then people started doing the weirdest thing with it, and things went, ironically, out of control!

## Conclusions

Programming Languages history is a fascinating topic! From FORTRAN, for which arguably the first real Compiler was built, to C - simply put, a milestone in the Computer Science history.

Modern languages history is also very interesting, and also heavily influenced by the new trends and technologies that become public every day.

Ultimately, the constant rise of new needs for such technologies, and the big amount of time it would take for "old" languages to adapt and address these needs, are the reasons why we have so many programming languages out there.


## References

R programming language: [A Brief History](https://en.wikipedia.org/wiki/R_(programming_language)#History) (Wikipedia page).

MATLAB programming language: [A Brief History](https://en.wikipedia.org/wiki/MATLAB#History) (Wikipedia page).

Prof. Aiken [personal page](http://theory.stanford.edu/~aiken/) at Stanford University.
