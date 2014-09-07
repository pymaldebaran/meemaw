meemaw
======

Compiler and tools for the Mee-Maw programming language.

Developpement tools and library
----------------------

The core tools used to implement the compiler chain is [LLVM](http://llvm.org/). That's why the project should be compiled using [clang++](http://clang.llvm.org/).

MeeMaw is developped according to *TDDAIYMT* method (Test Driven Developpement As If You Meant It) therefore it needs a quite good *automated test framework*. Currently we are using [Catch](https://github.com/philsquared/Catch).

Code quality is "assured" by the use of [uncrustify](http://uncrustify.sourceforge.net/) code formater/beautifier and [cppcheck](http://cppcheck.sourceforge.net/) static code analyser.
