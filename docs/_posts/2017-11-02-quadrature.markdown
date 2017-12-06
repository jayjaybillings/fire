---
layout: post
title: Quadrature
permalink: /science/quadrature
category: science
---

Quadrature is the process of approximating an integral as a discrete summation of weighted function evaluations that can be obtained computationally. Fire contains a number of different quadrature routines in the quadrature/ directory for computing integrals in 1, 2, or 3 dimensions.

# Gaussian Quadrature for Polynomials

Gaussian Quadrature is special form of quadrature that is exact for polynomials. A n-point Gaussian Quadrature rule is exact for polynomial of degree 2n-1. They are especially useful in finite element or other analyses where polynomials are regularly used as basis and test functions in integrals. Gaussian Quadrature has a performance advantage over other functions since it only requires n function evaluations to exactly evaluate polynomials, whereas other methods may require a much larger number of function evaluations. 

## 1D Line Integrals

The LineQuadratureRule class provides a 1D Gaussian Quadrature method for integration functions along a line. This class uses a four point Gaussian Quadrature rule over the bounds x=[-1,1] and is exact for all polynomials of degree 7 or less, although in practice this is limited to about seven decimal places for most polynomials since the weights are not provided to that high of a precision. It requires four evaluations of the function f. Specifically,

$$
\int_{-1}^{1} f(x) dx \approx \sum_{i=0}^{i=4} \omega_{i}f(x_{i})
$$

with the weights

$$
\omega = [\frac{18-\sqrt{30}}{36},
			\frac{18+\sqrt{30}}{36},
			\frac{18+\sqrt{30}}{36},
			\frac{18-\sqrt{30}}{36}] 
$$

and quadrature points

$$
x = [-\sqrt{\frac{3}{7}+\frac{2}{7}\sqrt{\frac{6}{5}}},
			-\sqrt{\frac{3}{7}-\frac{2}{7}\sqrt{\frac{6}{5}}},
			\sqrt{\frac{3}{7}+\frac{2}{7}\sqrt{\frac{6}{5}}},
			\sqrt{\frac{3}{7}-\frac{2}{7}\sqrt{\frac{6}{5}}}]
$$

The bounds of the integration are from [-1,1] and functions not defined over this region will need to be converted. This is usually done by u-substitution. For example, to go from x = [-1,1] to t = [0,1], let

$$
t = \frac{x-1}{2}\\
\frac{dt}{dx} = \frac{1}{2}\\
dt = \frac{1}{2} dx\\
dx = 2 dt\\
$$

such that

$$
\int_{-1}^{1} f(x) dx = \int_{0}^{1} 2f(t) dt
$$

## 2D Triangular Integrals

The TriangularQuadratureRule class provides a 2D Gaussian Quadrature method for integrating functions defined on the surface of triangles. This class uses a four point Gaussian Quadrature rule in area coordinates that exactly integrates all polynomials of degree 7 or less. It requires four evaluations of the function f. Specifically,

$$
\iint_{A} f(L_{1},L_{2},L_{3}) dA \approx \sum_{i=0}^{i=4} \omega_{i}f(L_{1,i},L_{2,i},L_{3,i})
$$

with the weights

$$
\omega = [-9.0/32.0,25.0/96.0,25.0/96.0,25.0/96.0]
$$

and quadrature points

$$
p_{1} = (1.0/3.0,1.0/3.0,1.0/3.0)\\
p_{2} = (3.0/5.0,1.0/5.0,1.0/5.0)\\
p_{3} = (1.0/5.0,3.0/5.0,1.0/5.0)\\
p_{4} = (1.0/5.0,1.0/5.0,3.0/5.0)\\
$$	