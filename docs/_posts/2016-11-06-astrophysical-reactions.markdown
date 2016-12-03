---
layout: post
title: Nuclear Reactions in Astrophysical Systems
permalink: /science/astrophysics/reactions
category: science
---

# Overview

The astrophysics module of Fire contains a set of classes for managing
thermonuclear reactions in astrophysical problems. The ReactionNetwork
class is responsible for organizing the species and rates that drive the
evolution of the network. 

# Configuring the Network

# Reaction Rates

Reaction rates are computed using parameterized rates from the REACLIB rate
library. The REACLIB rates are parameterized by seven coefficients and can
be computed as 

The rate is computed as

$$ R = p_s*\sum_k R_k $$

where $$p_s$$ is the prefactor (based on the statistical prefactor) and

$$ R_k = \exp(p_1 + \frac{p_2}{T_9} + \frac{p_3}{T_9^{1/3}} + p_{4}T_9^{1/3}
 + p_{5}T_9 + p_{6}T_9^{5/3} + p_{7}\ln T_9) $$

$$T_9$$ is the the temperature in units of $$10^9$$ Kelvin. The prefactor is
given by

$$ p_s = s\rho^{(n_R -1)}. $$

where $$s$$ is the statistical factor (an input from REACLIB), $$\rho$$ is the
density in units of $$10^8 \frac{g}{m^3}$$ and $$n_R$$ is the number of
reactants in the reaction (species on the left hand side that start the 
reaction).

In general k may be greater than 1 in the summation for the rate, but in this work k = 1 and $$R = R_k$$. 

More information on how this library is parsed is available in the documentation for [the Reaction ASCII format]({{ site.baseurl }}{% post_url 2016-11-12-astrophysics-reactions-ascii-format %}).

### Some Design Problems

Some of the code in the astrophysics module was adapted from an older code
called the Fast Explicit Reaction Network (FERN) solver. FERN is a research
code that was developed by students and others at the University of Tennessee
to explore issues around efficiently solving reaction networks using
advanced methods and hardware.

FERN has a very low-level design with significant flaws. The routines in 
FERN that initialize the network pack everything into arrays with little
regard for treating the memory right, among other problems. When this
code was ported to Fire, the memory errors were fixed, but the larger
considerations for a cleaner design were not addressed. These problems are
discussed in-line in the classes themselves and will be addressed in a
future release.

# References

"Stars and Stellar Processes", Mike Guidry, to be published Cambridge University Press.