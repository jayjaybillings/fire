---
layout: post
title: Solving Problems
permalink: /science/solving-problems
category: science
---

# THESE ARE NOTES

Nothing good here. Just trying to sketch some things out.

"Systems" are systems of partial or ordinary differential equations.

"Solvers" are routines for factorizing matrices.

"Methods" are methods used for solving partial differential equations including finite difference, finite element, particle and other types.

Most codes make no distinction between these three classes, but Fire does because the flexibility afforded by such a distinction is very valuable. Such a distinction makes it possible to modify each class separately without interfering with the others. This means that if the description of the PDEs change, it will not affect the method that solves those PDEs or the factorization routines used to solve the matrix.

### Systems

* RHS data
* LHS Operators
* Boundary Conditions - Can this work if the geometry/mesh is not known at this point? Do we need to have separate FE/FD System subclasses?
* Initial Conditions - Same.
* Method

**Problems**

* If my Method owns my discretization, what does a System really mean?
* How do boundary conditions work? They would typically be defined on the boundaries of the mesh, but as defined the system knows nothing about that. 

### Methods

* Geometry
* Mesh
* Grid

All this stuff is optional depending on the method type

# None of this is a great idea to me.