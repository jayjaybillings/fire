---
layout: post
title: Astrophysics Species ASCII File Format
permalink: /science/astrophysics/species-ascii-format
category: science
---

Simulations of thermonuclear networks in astrophysics require lists of
species that react with each other. Fire supports a legacy ASCII format with a 4-line specification for a species. See Species.h for ful details on each required value. 

The first line specifies the

* species name - a string
* mass number - an integer
* atomic number - an integer
* neutron number - an integer 
* mass fraction - a double
* mass excess - a double

The second through fourth lines specify parameters for the partition function. Each line contains eight double precision numbers separated by spaces.

Here is a sample for 4He and 12C.

```
4He 4 2 2 0.0 2.425
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
12C 12 6 6 0.0416666 0.0
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0
```