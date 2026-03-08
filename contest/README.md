# Programming contest

This contest is about creating the fastest FDM simulation for the poisson equation on a specified grid and the source term (rhs) given.

``` math
\begin{aligned}
\nabla^2 u(x, y) &= f(x, y), \quad [x, y] \in \langle 0, 1\rangle \times \langle 0, 1\rangle\\
u(x, y) &= 0, \quad (x, y) \in \partial\lbrace\langle 0, 1\rangle \times \langle 0, 1\rangle\rbrace,              
\end{aligned}
```
where $f(x, y) = 2  \pi^2  \sin(\pi x)  \sin( \pi y )$.

We obtain the numerical solution using the finite difference method on a Cartesian grid with a constant step size $h$. By discretizing the Laplace operator, we obtain a system of linear equations, which we will solve iteratively using the Jacobi method.

Let $u_{i,j}$ be the approximation of the solution $u(x_i,x_j)$, where $x_i=ih$ and $y_j=jh$. The Jacobi iteration for updating the value of an interior point $(i,j)$ is as follows:
``` math
  u_{i,j}^{(k+1)} = \frac{1}{4} \left( u_{i+1,j}^{(k)} + u_{i-1,j}^{(k)} +
  u_{i,j+1}^{(k)} + u_{i,j-1}^{(k)} \right) + \frac{h^2}{4} f_{i,j}           
```
Thus, one iteration of the Jacobi method involves updating all interior grid points using the values from the previous iteration.

For the convergence criterion the following formula will be used:
``` math
\varepsilon = \max_{i,j} |u_{i,j}^{new} - u_{i,j}|
```

NOTE: Multiple versions will be created along the progression of the course of Programming of Engineering Applications. I will describe each version briefly. At the end of the semester I'll evaluate all the scripts.

## Version 1
This version is a simple, a bit optimized, sequential programm. I used flattened arrays for faster data access. I also created a class FlatArray for easier handling and some useful member function for more readable code.
The class also features `swap()` method for data swaping between two arrays (mby simple pointer swap would be faster?).

## Version 2
This version is optimalized sequential code, featuring loop reordering, vectorization (intrinsics) and cache blocking.

## Version 3
Multithreading of the code from version 2 using OpenMP.

## Version 4


# Benchmarking and comparison
