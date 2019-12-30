---
title: "Can I Get a Job After I Study Compilers?"
date: 2019-09-27
draft: false
---


The answer is a resounding **absolutely yes**, even though it will depend on the type of job you're looking for, of course.

Funnily enough, if you are a scientist and specialize in some niche topic, you might find that there are a lot of jobs available to those who are fluent in Compilers, Parsers and general Programming Language theory.

## But… Why? And How?

The reason is that basic Parsing and Grammar theory is ubiquitous, and it's hidden in places you would never imagine.

The "big name" in this scenario is **Domain Specific Languages**.

From Wikipedia:

_A *domain-specific language (DSL)* is a computer language specialized to a particular application domain. This is in contrast to a general-purpose language (GPL), which is broadly applicable across domains. There are a wide variety of DSLs, ranging from widely used languages for common domains [...] down to languages used by only one or a few pieces of software [...]._

In other words, a lot of applications out there have some component that is based on a DSL. The classic example is a web application that exposes an API where users can perform queries to the server's database.

## A typical startup job

Let's make this same example more concrete. Say, you have collected a lot of data about temperature and weather in a given city, for the past 5 years. There was no such a database available on the internet, and it took you a lot of work to build it.

Now, you want to sell access to this database, maybe as a small monthly fee. In what form should you grant access to the data, to your paying customers?

You could tell them - Hey, write your own SQL query, send it over and my web app will send you data in CSV format. This would be a **huge mistake**:

* You can't really assume your customers know the SQL language.
* You can't trust them! They could send over *malicious query*, that would corrupt your data.

A better alternative, sound and implemented by many web services, is to ask the user to send a JSON, where the fields in the JSON correspond to fields in your DB. This is fine, although it requires a POST request, and additional data sent over the network.

A third alternative, that **I personally implemented for a paid project**, is to design the web server such that it supports URLs like the following

`domain-name.com/weather?q=city is NYC and (temp gt 45 or month is Oct)`

Bear in mind that spaces in the URL must be encoded correctly (usually "%20"). I didn't do it in the example for the sake of clarity.

So, what is happening within that URL? Your customer is sending a standard GET request, where the `q` (aka "query") part is written in... something that looks like English! They are looking for entries in your DB where the city is NYC and the temperature is higher than 45 (gt, normally stands for greater than) or the month is October.

Does it remind you of anything? Yes, Google's search engine API works very similarly to this.

![Draft Design of a DSL-base Web API](/images/WebApiDsl.png)

But how does your web app work underneath? How does it know what the query part means? There's only one possibility: *you created a Domain Specific Language* for your application.

- You defined a small Grammar, with keywords such as "is", "gt", "and", "or".
- You implemented a lightweight Parser, that checks that the queries sent by your customers are correctly typed (and otherwise inform them about their mistakes).
- You built a small translation module from your DSL to raw SQL, **inside your web server**, so that it's safe.

More realistically, you hired someone with experience in Compilers, Languages and DSL to do this for you.

## What about enterprise jobs?

Sure, if you are really ambitious you can drop an email to someone at Oracle, and ask if they have open positions in the team that works on Java. Be sure to calibrate your wordings: you don't want a position in some team that works _with_ Java!

I like to be more realistic though. The truth is that, today, **most Programming Languages are open-source**. Therefore, you can't get a job directly to work on that.

It's more about what you are ready to give, and *how you want to contribute*. For example, you could contribute to the development of the Python Language, if that's something you like:

1. Check out Python's code from GitHub.
2. Study it (good luck!). You'll need the source code as much as the official documentation.
3. Choose one module, or even just one thing in the language (e.g., list), and go deeper in your studies about that part.
4. Identify optimization spots, read the Developer's guide and submit a Merge Request.

If you think this would not be a "job", and therefore just a big waste of time, then think again. If you build yourself a reputation as an outstanding developer, so good that you made a serious contribution (not just some documentation file) to a BIG project such as Python, *you won't have any trouble in finding a very well paid job*.

## Do you care about Computer Science?

I would like to drive the focus on the importance of studying Programming Languages and Compilers Theory, **regardless of your job status**.

_As a well-known fact, there are two types of code developers out there._

The one who likes to get some quick and dirty result, and then see what happens. Or the one who wants to acquire the most solid knowledge she can, about a certain topic, and is confident that things will roll in her favor. I call the first ones programmers, the second ones scientists.

If you are scared about the many possibilities and choices that are offered in everyone's professional life, then you should not study Compilers.

Just learn how to code in a couple of Programming Languages of your choice. **Learn how to use** Programming Languages.

You'll get tested about your skills, in the very standard way that we call "job application" today. And if you are convincing enough, *you might get a very good position as a programmer*.

But if you get excited when there are many roads, and some of them are unclear, then you belong to the second group. That's where I belong to as well.

Today's competition for a job position is brutal. This has the very sad side-effect that most people content themselves with the minimal necessary knowledge to get stuff done. I wish them the best, but I doubt they can go very far this way.

**In a world of fierce competition, knowledge is your best and only weapon.**

Even if you don't see in your professional horizons how or when a complex topic like Compilers could be useful, I recommend you to dedicate time to your *professional growth*. This is a subject that touches in depth many areas of Computer Science, from advanced algorithms and data structures, automata, computer's architecture and even AI (in the classical meaning of AI, that is greedy and heuristic search). If you master it, you'll be a better Computer Scientist. If you master it, it will help you. *It did help me*.

Learn how to build programming languages, not just how to use a couple of them. It will get you ahead of the competition.

From a Quora discussion:

_If you can truthfully claim that you have written a compiler with your own hands, it is likely that there will be no questions asked after that in any interview._

## Conclusions

I know my view on this subject may sound odd to some, but it's my firm belief that **knowledge is never a waste**.

When I studied Compilers and Programming Languages in depth I already was working as a software engineer. Nonetheless, I decided to spend many months studying that, and also self-assigning myself a tough project (more about it in the next articles).

I gained so much from those months. Invaluable knowledge and insights about algorithms, data structures and the inner "secret" of Programming Languages.

I did get more job opportunities thanks to this, and you can too.

## References

Quora discussion on the benefits of studying Compilers: <https://www.quora.com/What-is-the-importance-of-studying-Compiler-Design>.

Stack Overflow thread on the benefits of studying Compilers: <https://stackoverflow.com/questions/733093/when-should-i-learn-compilers>.

Source code for the Python Programming Language: <https://github.com/python/cpython>.

Startup jobs listing filtered with the keyword "Compiler", on [angel.co](https://angel.co/jobs#find/f!%7B%22skills%22%3A%5B%22Compiler%20Design%22%2C%22Compilers%22%5D%7D) (might need log-in. No affiliations!).
