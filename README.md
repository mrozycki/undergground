# Undergground Solution Grader

Undergground is a solution grader for algorithmic problems.
It automatically downloads submissions from an accompanying
website (not available on GitHub at this point), runs them
on predefined test sets and uploads scores to the database,
where they can be accessed by users.

## Features

* grading solutions to multiple problems
* arbitrary number of test cases per problem
* soft memory and time limits set separately per test
* hard timeout, memory and process count limit

## History

This project was first created by me in 2012, when I was
barely into the first year of my undergraduate degree.
The code was messy, and it barely worked, but it worked
nonetheless. It was used by several high school students
to practice solving algorithmic problems.

The grader was taken offline in 2014, as the interest
in using it, as well as my availability to maintain it,
have both dropped significantly.

In 2018 I have discovered the source code on my hard drive,
and decided to immediately back it up on GitHub. The code
has been barely modified from the original 2012 version,
and was not touched at least since 2014. It was not up
to my quality standards anymore, but I thought I would
one day pick it back up, clean it up and continue developing it.

That time came in March 2020, when bored during the lockdown
I have started refactoring that old code. This process
is still going on, as I have since forgotten many of the
techniques used in the project, and have not included many
comments as to why certain things work in certain ways.
