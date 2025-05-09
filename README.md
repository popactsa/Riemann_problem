# Riemann problem solver
A wrapper for a pack of solvers for hydrodynamics, solid state physics problems.

The main goal of this project is to create a parser with 
a relatively simple and easy embedding of new solvers 
written in seminars.

In many(most) ways it is a playground for mastering the language and development tools/techniques.

Installation
-

- Make sure that you have installed: Armadillo 14.4+, SuperLU, OpenBLAS
- Clone this repo: git clone ssh://git@github.com/popactsa/Riemann_problem.git
- mkdir build && cd build && cmake .. && make

How to use
-

- Exec `build/Riemann`
- Choose a scenario to solve. All scenarios are stored at `scenario`.
- Results are stored at `build/data`

How to create a scenario
-

There are few types of variables:
- Common -> "`name` `value`"
- Named -> "`name` : `key1` `value1` `key2` `value2` ..etc"
- Common array -> "`name` : `value1` `value2` ..etc"
- Named array -> "`name` `index`: `key1` `value1` `key2` `value2` ..etc"

Planned features:
-

- Solvers reworked and translated from Python: WENO5 1D/2D, Chapman-Jouquet, SIMPLE 1D/2D, Godunov 1D/2D
- C++20 multithreading
- MPI
- Interactive 1D/2D scene builder(something like Paint)

Made by avi🤕
