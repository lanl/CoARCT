# CoARCT

[![Build Status](https://travis-ci.org/lanl/CoARCT.svg?branch=clang-10.0)](https://travis-ci.org/lanl/CoARCT)

Code Analysis and Refactoring with Clang Tools

CoARCT (pronounced like the word "corked") is a small set of tools built on Clang's LibTooling. CoARCT demonstrates some more sustained examples of refactoring and analyzing code with AST Matchers and the clang Refactoring Tool. The goal is not to supply tools, but rather to show how one can create one's own tools.

It includes library code and command line drivers that go beyond some of the (excellent! but short) tutorials that are available. The CoARCT examples are drawn from refactoring legacy codes:
* Reporting which functions use which global variables;
* Replacing global variables with local variables, including threading variables through a call chain;
* Detecting which functions use which fields of a struct: this data can be used to analyze how to break up large structs;
* Finding code associated with a classic C-style linked list;
* Identifying struct fields defined with typedefs, reporting underlying types (apps/TypedefFinder.cc);
* Identifying typedef;
* Identify uses of a class template, such as std::vector<T>.

Explanatory blog posts can be found at Various Burglarious: https://variousburglarious.com/2017/01/18/getting-started-with-clang-refactoring-tools/

It also demonstrates a few useful things that were not immediately clear from the tutorials and examples I learned from, such as unit testing matchers and callbacks, and building out of the Clang/LLVM tree.

Our hope is that CoARCT will help demystify the Clang AST tools to developers. If the CoARCT tools are directly useful in your work, let us know!

## Prerequisites:
1. CMake version 3+ (https://cmake.org/download/)
2. Clang and LLVM 10.0 libraries and headers (http://releases.llvm.org/download.html)
3. libtinfo
4. Boost (currently using 1.61, just needs boost/type_index in one spot)
4. Google test (currently using 1.8.0 https://github.com/google/googletest)

Default branch is Clang 10.0 (older branches: 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.9, 3.8). These branches track changes in the Clang API's, so be sure to match the branch against the version of Clang you are using.

## Build

1. Make sure clang++ is in your path
1. Define these environment variables
    ```
    CXX: Your clang++ version 10.0.x
    GTEST_DIR: Top level directory of google test installation
    BOOST_DIR: Top level of Boost (#include "boost/type_index.hpp" needs to work)
    TINFO_LIB_DIR: points to where libtinfo.a is installed.
    ```
2. Clone the repository
3. Create a build directory

    ```
    /home/CoARCT $ mkdir build-clang-10.0.0
    /home/CoARCT $ cd build-clang-10.0.0
    ```

4. Run cmake, make

    ```
    /home/CoARCT/build-clang-10.0.0 $ cmake ..
    /home/CoARCT/build-clang-10.0.0 $ make
    ```

5. Run the unit tests

    ```
    /home/CoARCT/build-clang-10.0.0 $ ./test/corct-unittests
    ...
    [==========] 63 tests from 16 test cases ran. (438 ms total)
    [  PASSED  ] 63 tests.
    ```

## Changes for Clang 10.0

(Still) None!

## Changes for Clang 9.0

None! (This seems too easy...)

## Changes for Clang 8.0

Renamed ```getLoc(Start|End)``` to ```get(Start|End)Loc```, following Clang API change. Version bumps for Docker/Travis.

## Changes for Clang 7.0

None! (Hmmm.)

## Changes for Clang 6.0

Added logic to match desugared types in template variable matcher. This doesn't affect anything in Clang 5, and it keeps the Clang 6 behavior the same as before (i.e. it still "sees through" type aliases).

## Changes for Clang 5.0

Minor tweaks. Hopefully CMake configuration is improved. Also added ability to configure compiler instances in unit tests; this should permit more complex test inputs.

## Known issues

These issues did not arise this time with my standard build on a Mac. But I'll mention them again, in case they come up.

### futimens on OSX

Building CoARCT failed on OSX with pre-built binaries from llvm.org: the function `futimens` was undefined. Workaround: build Clang and LLVM from source as described at http://clang.llvm.org/get_started.html.

### No std::is_final

Building CoARCT on Linux failed with errors about `no member is_final in namespace std`. Diagnosis: That installation of Clang seems to be finding headers with an older GCC (4.8.5). Workarounds

1. set (or append) `--gcc-toolchain=/path/to/newer/gcc` to the `CXXFLAGS` environment variable when running CMake.

1. The above solution did not work on one system. In that case, overriding cxx-isystem was necessary. Pass:
    ```
    -cxx-isystem /path/to/newer/gcc/include/c++/version -cxx-isystem /path/to/newer/gcc/include/c++/<version>/x86_64-pc-linux-gnu
    ```

## Copyright

Los Alamos National Security, LLC (LANS) owns the copyright to CoARCT, which it identifies internally as LA-CC-17-039. See the LICENSE file for license information.
