# CoARCT
Code Analysis and Refactoring with Clang Tools

This is a placeholder. When LANL approves release of CoARCT example code, it will go here.

## What is it?

CoARCT (pronounced like the word "corked") is a set of small tools built on the Clang tools. It is a chance to show some more sustained examples of refactoring and analyzing code with AST Matchers and the clang Refactoring Tool.

## Prerequisites:
CMake, probably at least version 3.
Clang and LLVM libraries and headers.
Currently known to work with Clang 3.9.0 and Clang 3.8.0.

## Build

### Define these environment variables
GTEST_DIR: Top level directory of google test installation
TINFO: points to where your libtinfo.a is installed.
LLVM_LIB_DIR: points to where LLVM libraries are installed.
CLANG_LIB_DIR: points to where Clang libraries are installed.
