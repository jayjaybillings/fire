---
layout: post
title: Notes on Getting Eclipse to Work
permalink: /about/eclipse-bugs
category: about
---

It should go without saying that I think Fire should be developed in Eclipse,
especially given my role in the Eclipse community. But... that does not mean that I am above acknowledging that Eclipse can have some extremely annoying
errors.

### Getting the indexer to work properly

If the indexer does not work for your project, make sure that you are opening
the project from the subdirectory in the FIRE-Debug@fire-build or FIRE-Release@fire-build projects created by CMake instead of the straight FIRE 
project. The difference is that the default project does not properly
configure the builders and toolset, but the CMake project does.

### Autocompletion doesn't work

Autocompletion in C++ projects often breaks in Eclipse. One trick that works
is to follow edit "Window->Preferences->C/C++->Editor->Content Assist->Advanced" and make sure that "Parsing Based Proposals" is checked in both
menus.

### Autocompletion with variables declared using the auto keyword

Sorry, you're out of luck! Eclipse CDT is not currently able to discover what
the type is of a variable declared with auto in C++11. The CDT project lead,
Doug Schaefer, says that there is a lot of work going into the new features.


