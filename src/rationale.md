# Rationale

This file describes why one or the other decision was made

Code style of this project is based on 'Google C++ stylebook'.
If there are no separate explanations in this file, you may refer to it.

## Modular project structure

To reduce compile time modular design is used. Each solver is independent.

## Formatting

No unformatted code is allowed.
Just use clang-format with given config file, it's automatically executed on all
source files and tests when project compiled.

## Naming conventions

The naming convention for variables has been changed
in favor of the ease of writing equations:

[] All equation variables and constants are named without any underscores
[] In other cases, naming convention is preserved

As those variables are used in different contexts, it shouldn't cause
any inconvenience.

## Files naming conventions

`.hpp` only. `.h` should be used only for C header files.

Snake-case is preferred for naming in project.

Header guards should be a capitalized version of a file name.

## CRTP

CRTP is used instead of virtual functions, since it would have required search in vtables
each time when any method is used.
