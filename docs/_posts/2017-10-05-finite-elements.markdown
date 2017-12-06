---
layout: post
title: Finite Elements
permalink: /science/fem
category: science
---

2D finite element support.

# Constant Strain Triangle Elements

The design of this triangular element follows the description in _The Finite Element Method_ by A J Davies, (Oxford, 2011),
which provides an excellent treatment of the subject.

The Constant Strain Triangle (CST) element is an element in which the strain (or any similar quantity) is constant across
the triangle. This element contains three nodes at positions 

$$ (x_1,y_1)\\
(x_2,y_2)\\
(x_2,y_3) $$ 

that form the corners of the traingle. The area of the CST is

$$ A = \frac{1}{2}\begin{vmatrix}1,x_1,y_1\\1,x_2,y_2\\1,x_3,y_3\end{vmatrix}\\
= \frac{1}{2}[(x_{2}y_{3} - x_{3}y_{2}) - x_{1}(y_{3} - y_{2}) + y_{1}(x_{3} - x_{2})]$$

A point inside the element is defined locally using Area Coordinates for local coordinates $$ (L_1,L_2,L_3) $$ as

$$ L_1 = \frac{A_1}{A} , L_2 = \frac{A_2}{A}, L_3 = \frac{A_3}{A} $$ 

The "subareas" $$ A_1, A_2, A_3$$ are defined as the areas of the subtriangles formed between a point in the triangle 
and two of the three nodes. For a point at $$ (L_1,L_2,L_3) $$, $$ A_1 $$ is the area of the triangle formed between nodes 
2, 3 and the point, and so on for both $$ A_2 $$ and $$ A_3 $$.

The global position of a point in the triangle, (x,y), is defined by a combination of the local coordinates of the point
in the triangle and the global coordinates of the nodes as follows

$$ x = L_{1}x_{1} + L_{2}x_{2} + L_{3}x_{3} \\
y = L_{1}y_{1} + L_{2}y_{2} + L_{3}y_{3} $$

Likewise, the local coordinates $$ L_{i} $$ can be defined in terms of the global position (x,y)

$$ L_{i} = \frac{(a_{i} + b_{i}x + c_{i}y)}{2A} $$

The constants a, b, and c are the factors in the determinant of the matrix A, cyclicly permutated to go from node to
node

$$ a_1 = x_{2}y_{3} - x_{3}y_{2}\\
b_1 = y_{2} - y_{3}\\
c_1 = x_{3} - x_{2}\\
$$

$$ a_2 = x_{3}y_{1} - x_{1}y_{3}\\
b_2 = y_{3} - y_{1}\\
c_2 = x_{1} - x_{3}\\
$$

$$ a_3 = x_{1}y_{2} - x_{2}y_{1}\\
b_3 = y_{1} - y_{2}\\
c_3 = x_{2} - x_{1}\\
$$

## Shape Functions

The shape functions for CSTs are defined simply in terms of $$L_i$$,

$$ \mathbf{N}^{e} = \begin{bmatrix} L_1, L_2, L_3 \end{bmatrix} $$

## Stiff Matrix and Force Vector Contributions

The contributions to the stiffness matrix for a CST with globally defined node indices $$i,j$$ at point $$x,y$$ are defined by integrating over the area of the triangle. These contributions are dependent on the form of the stiffness kernel. If that kernel is defined by the Laplacian, then the elements of the stiffness matrix are given by

$$
k_{ij} = \iint_A k (\frac{b_{i}b_{j}}{4A^2} + \frac{c_{i}c_{j}}{4A^2}) dx dy
$$

The contributions to the i-th element of the global force vector on a CST are 

$$
f_{i} = \iint_A L_{i}f(x,y) dx dy
$$

### Boundary Values

The boundary values contributed to both $$k$$ and $$f$$ by providing additive updates if a side of the triangle is on the boundary. The contribution to the stiffness matrix due to the boundary conditions is

$$
\bar{k} = \int_{C^{e}_{2}} \sigma(s) \mathbf{N}^{e^{T}}\mathbf{N}^{e} ds
$$

The contributions to the force vector are

$$
\bar{f} = \int_{C^{e}_{2}} h(s) \mathbf{N}^{e^{T}} ds
$$

These integrals are presented in matrix and vector form above, but for a CST they can be simplified to $$k_{ij}$$ and $$f_{ij}$$ forms. The arc length s for a boundary side $$\bar{ij}$$ of a triangle with nodes i, j, k is

$$
s = (b_{k}^{2} + c_{k}^{2})^{1/2} L_{j}\\
ds = (b_{k}^{2} + c_{k}^{2})^{1/2} dL_{j}
$$

The matrix $$\mathbf{N}^{e^{T}}\mathbf{N}^{e}$$ is symmetric with nonzero elements for (i,j), (j,i), (i, i), and (j, j) along the side $$\bar{ij}$$ because, along that side, $$L_{k} = 0$$. Thus $$\bar{k}_{ij} = \bar{k}_{ji}$$ and only three integrations are needed for the nonzero elements. 

$$
\bar{k}_{\bar{ij}} = \bar{k}_{\bar{ji}} =  \int_{0}^{1} \sigma(L_j) (b_{k}^{2} + c_{k}^{2})^{1/2} (L - L_{j}^{2}) dL_j\\
\bar{k}_{\bar{ii}} =  \int_{0}^{1} \sigma(L_j) (b_{k}^{2} + c_{k}^{2})^{1/2} (1 - L_{j})^{2} dL_j\\
\bar{k}_{\bar{jj}} =  \int_{0}^{1} \sigma(L_j) (b_{k}^{2} + c_{k}^{2})^{1/2} L_{j}^{2} dL_j\\
\bar{k}_{\bar{ik}} = \bar{k}_{\bar{ki}} = \bar{k}_{\bar{kj}} = \bar{k}_{\bar{jk}} = \bar{k}_{\bar{kk}} = 0 
$$

The vector $$\mathbf{N}^{e^{T}}$$ has one nonzero entry such that

$$
\bar{f}_{i} = \int_{0}^{1} h(L_i) L_{i} dL_i\\
\bar{f}_{j} = \int_{0}^{1} h(L_i) (1 - L_{i}) dL_i\\
\bar{f}_{k} = 0
$$

Note that both $$\bar{f}_{i}$$ and $$\bar{f}_{j}$$ are integrated over $$L_{i}$$ because the contribution is from side $$\bar{ij}$$.