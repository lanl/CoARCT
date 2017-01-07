# CoARCT
Code Analysis and Refactoring with Clang Tools

CoARCT (pronounced like the word "corked") is a set of small tools built on the Clang tools. It is a chance to show some more sustained examples of refactoring and analyzing code with AST Matchers and the clang Refactoring Tool.

It includes library code and command line drivers that go beyond some of the (excellent!) tutorials. These examples are drawn from refactoring legacy codes:
* Reporting which functions use which global variables;
* Replacing global variables with local variables, including threading variables through a call chain;
* Detecting which functions use which fields of a struct: this data can be used to analyze how to break up large structs.
* Finding code associated with a classic C-style linked list.

Our hope is that CoARCT will help demystify these tools for more developers; if the tools are useful, so much the better! Either way, let us know.

It also demonstrates a few useful things that were not immediately clear from the tutorials and examples I saw, such as unit testing matchers and callbacks, and building out of the Clang/LLVM tree.

## Prerequisites:
1. CMake, probably at least version 3.
2. Clang and LLVM libraries and headers.
3. libtinfo
Currently known to work with Clang 3.9.0 and Clang 3.8.0.

## Build

1. Define these environment variables
```
  GTEST_DIR: Top level directory of google test installation
  TINFO: points to where your libtinfo.a is installed.
  LLVM_LIB_DIR: points to where LLVM libraries are installed.
  CLANG_LIB_DIR: points to where Clang libraries are installed.
```
2. Clone the repository
3. Create a build directory
```
CoARCT $ mkdir build-clang-3.9.0
CoARCT $ cd build-clang-3.9.0
```
4. Run cmake, make
```
CoARCT/build-clang-3.9.0 $ cmake ..
CoARCT/build-clang-3.9.0 $ make
```
5. Run the unit tests
```
CoARCT/build-clang-3.9.0 $ ./test/corct-unittests
...
[==========] 36 tests from 7 test cases ran. (44 ms total)
[  PASSED  ] 36 tests.
```
