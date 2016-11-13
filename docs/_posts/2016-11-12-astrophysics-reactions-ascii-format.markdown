---
layout: post
title: Astrophysics Reactions ASCII File Format
permalink: /science/astrophysics/reactions-ascii-format
category: science
---

Simulations of thermonuclear networks in astrophysics require lists of reactions that describe how the species evolve over time. Fire supports a legacy ASCII format with a 8-line specification for a reaction. Each entry
in each line is separated by a space. See Reaction.h for full details on each required value.

The first line for each reaction contains:

* Name/Label - A string of the form "he4+he4+he4-->c12" that describes the 
reaction.
* Reaction Group Class - an integer
* Reaction Group Index - an integer
* REACLIB Class - an integer
* Number of reacting species - an integer
* Number of resulting products - an integer
* Electron capture flag - a bool
* Reverse reaction flag - a bool
* Statistical factor - a double
* Energy release - a double

The second line contains the seven rate coefficients from the REACLIB library, all doubles. The third line contains the array of atomic numbers for the reactants in this reaction, which are integers. The fourth, fifth, and sixth lines are also integers for the reactant neutron number and product atomic and neutron numbers. Each of these four lines has one integer for each reactant and product in the
system, but with no more than four entries per line. (See example below for more details.)

The seventh and eighth lines contain quantities that are used for *Partial Equilibrium* approximations.
Each line contains up to three integers.

```
he4+he4+he4-->c12 3 0 8 3 1 0 0 0.16666667 7.27500
-24.99350000 -4.29702000 -6.69304000 15.59030000 -1.57387000 0.17058800 -9.02800000
2 2 2 // Since numReactants = 3 and he4 has Z=2, there are three values on this line equal to 2.
2 2 2 // Same, but for he4's neutron number
6 // There is only one product and Z=6 for C12.
6 // N=6 for C12
0 0 0 
1 
ne20-->he4+o16 2 3 2 1 2 0 1 1.00000000 -4.73400
109.31000000 -72.75840000 293.66400000 -384.97400000 20.23800000 -1.00379000 201.19300000
10 // Z for ne20
10 // N for ne20
2 8 // Z for he4 and o16
2 8 // N for he4 and o16
3 
0 2 
```