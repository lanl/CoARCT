# CoARCT
Code Analysis and Refactoring with Clang Tools

CoARCT (pronounced like the word "corked") is a small set of tools built on Clang's LibTooling. CoARCT demonstrates some more sustained examples of refactoring and analyzing code with AST Matchers and the clang Refactoring Tool.

It includes library code and command line drivers that go beyond some of the (excellent!) tutorials that are available. The CoARCT examples are drawn from refactoring legacy codes:
* Reporting which functions use which global variables;
* Replacing global variables with local variables, including threading variables through a call chain;
* Detecting which functions use which fields of a struct: this data can be used to analyze how to break up large structs;
* Finding code associated with a classic C-style linked list;
* Identifying struct fields defined with typedefs, reporting underlying types (apps/TypedefFinder.cc).

Explanatory blog posts can be found at Various Burglarious: https://variousburglarious.com/2017/01/18/getting-started-with-clang-refactoring-tools/

It also demonstrates a few useful things that were not immediately clear from the tutorials and examples I learned from, such as unit testing matchers and callbacks, and building out of the Clang/LLVM tree.

Our hope is that CoARCT will help demystify the Clang AST tools to developers. If the CoARCT tools are directly useful in your work, let us know!

## Prerequisites:
1. CMake version 3+ (https://cmake.org/download/)
2. Clang and LLVM 5.0 libraries and headers (http://releases.llvm.org/download.html)
3. libtinfo
4. Boost (currently using 1.61, just needs boost/type_index in one spot)
4. Google test (currently using 1.7.0 https://github.com/google/googletest)

Default branch is Clang 5.0 (older branches: 4.0, 3.9, 3.8).

## Build

1. Make sure clang++ is in your path
1. Define these environment variables
    ```
    CXX: Your clang++ version 5.0.x
    GTEST_DIR: Top level directory of google test installation
    BOOST_DIR: Top level of Boost (#include "boost/type_index.hpp" needs to work)
    TINFO_LIB_DIR: points to where libtinfo.a is installed.
    ```
2. Clone the repository
3. Create a build directory

    ```
    /home/CoARCT $ mkdir build-clang-5.0.0
    /home/CoARCT $ cd build-clang-5.0.0
    ```

4. Run cmake, make

    ```
    /home/CoARCT/build-clang-5.0.0 $ cmake ..
    /home/CoARCT/build-clang-5.0.0 $ make
    ```

5. Run the unit tests

    ```
    /home/CoARCT/build-clang-5.0.0 $ ./test/corct-unittests
    ...
    [==========] 60 tests from 13 test cases ran. (157 ms total)
    [  PASSED  ] 60 tests.
    ```

## Changes for Clang 5.0

Minor tweaks. Hopefully CMake configuration is improved. Also added ability to configure compiler instances in unit tests; this should permit more complex test inputs.

## Known issues

### futimens on OSX

Building CoARCT failed on OSX with pre-built binaries from llvm.org: the function `futimens` was undefined. Workaround: build Clang and LLVM from source as described at http://clang.llvm.org/get_started.html.

### No std::is_final

Building CoARCT on Linux failed with errors about `no member is_final in namespace std`. Diagnosis: That installation of Clang seems to be finding headers with an older GCC (4.8.5). Workarounds

1. set (or append) `--gcc-toolchain=/path/to/newer/gcc` to the `CXXFLAGS` environment variable when running CMake.

1. The above solution did not work on one system. In that case, overriding cxx-isystem was necessary. Pass:
    ```
    -cxx-isystem /path/to/newer/gcc/include/c++/version -cxx-isystem /path/to/newer/gcc/include/c++/<version>/x86_64-pc-linux-gnu
    ```

### Building on Ubuntu

Prajjwald reported some solutions to problems with building on Ubuntu. Please see [issue #1](https://github.com/lanl/CoARCT/issues/1).

These issues may be mitigated in the clang-5.0 branch.

## Copyright

Los Alamos National Security, LLC (LANS) owns the copyright to CoARCT, which it identifies internally as LA-CC-17-039. See the LICENSE file for license information.
