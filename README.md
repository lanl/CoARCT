# CoARCT
Code Analysis and Refactoring with Clang Tools

CoARCT (pronounced like the word "corked") is a small set of tools built on the Clang tools. It demonstrates some more sustained examples of refactoring and analyzing code with AST Matchers and the clang Refactoring Tool.

It includes library code and command line drivers that go beyond some of the (excellent!) tutorials that are available. The CoARCT examples are drawn from refactoring legacy codes:
* Reporting which functions use which global variables;
* Replacing global variables with local variables, including threading variables through a call chain;
* Detecting which functions use which fields of a struct: this data can be used to analyze how to break up large structs;
* Finding code associated with a classic C-style linked list;
* Identifying struct fields defined with typedefs, reporting underlying types (apps/TypedefFinder.cc).

It also demonstrates a few useful things that were not immediately clear from the tutorials and examples I learned from, such as unit testing matchers and callbacks, and building out of the Clang/LLVM tree.

Our hope is that CoARCT will help demystify the Clang AST tools to users. If the CoARCT tools are directly useful in your work, let us know!

## Prerequisites:
1. CMake version 3+ (https://cmake.org/download/)
2. Clang and LLVM libraries and headers (http://releases.llvm.org/download.html)
3. libtinfo
4. Google test (https://github.com/google/googletest)

Currently known to work with Clang 3.9.0 and Clang 3.8.0.

## Build

1. Define these environment variables

    GTEST_DIR: Top level directory of google test installation
    TINFO_LIB_DIR: points to where libtinfo.a is installed.
    LLVM_LIB_DIR: points to where LLVM libraries are installed (e.g. ${HOME}/llvm/clang+llvm-3.9.0-x86_64-apple-darwin/lib)
    CLANG_LIB_DIR: points to where Clang libraries are installed. (e.g. ${HOME}/llvm/clang+llvm-3.9.0-x86_64-apple-darwin/lib)
    LibClang_INCLUDE_DIR: where Clang headers reside (e.g. ${HOME}/llvm/clang+llvm-3.9.0-x86_64-apple-darwin/include)

2. Clone the repository
3. Create a build directory
    /home/CoARCT $ mkdir build-clang-3.9.0
    /home/CoARCT $ cd build-clang-3.9.0
4. Run cmake, make
    /home/CoARCT/build-clang-3.9.0 $ cmake ..
    /home/CoARCT/build-clang-3.9.0 $ make
5. Run the unit tests
    /home/CoARCT/build-clang-3.9.0 $ ./test/corct-unittests
    ...
    [==========] 55 tests from 11 test cases ran. (139 ms total)
    [  PASSED  ] 55 tests.

Los Alamos National Security, LLC (LANS) owns the copyright to CoARCT, which it identifies internally as LA-CC-17-039. See the LICENSE file for license information.
