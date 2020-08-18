# -*- Makefile -*-
cmake_minimum_required(VERSION 3.3)

# Compiler-driven Refactoring with Clang Tools
project(CoARCT LANGUAGES CXX)

# global C++ properties

# Guess cmake paths for Clang and LLVM
get_filename_component(CLANG_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
set(CLANG_BASE ${CLANG_BIN_DIR}/..)
message(STATUS "CLANG_BASE:" ${CLANG_BASE})
message(STATUS "CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})
if(NOT DEFINED ENV{ECLANG_CMAKE_DIR})
  set(CLANG_CMAKE_DIR ${CLANG_BASE}/lib/cmake/clang)
else()
  set(CLANG_CMAKE_DIR $ENV{ECLANG_CMAKE_DIR})
endif()
if(NOT DEFINED ENV{ELLVM_CMAKE_DIR})
  set(LLVM_CMAKE_DIR ${CLANG_BASE}lib/cmake/llvm)
else()
  set(LLVM_CMAKE_DIR $ENV{ELLVM_CMAKE_DIR})
endif()
include("${CLANG_CMAKE_DIR}/ClangConfig.cmake")
include("${LLVM_CMAKE_DIR}/LLVMConfig.cmake")

# Guess include paths for the standard C++ library.
# These paths are used from CoARCT tests and applications to configure the
# compilers instantiated in those programs.
set(CLANG_INCLUDE_DIR1 ${CLANG_BASE}/include/c++/v1)
set(CLANG_INCLUDE_DIR2 ${CLANG_BASE}/lib/clang/9.0.0/include)
message(STATUS "CLANG_INCLUDE_DIR1: " ${CLANG_INCLUDE_DIR1})
add_definitions(-DCLANG_INC_DIR1="${CLANG_INCLUDE_DIR1}")
add_definitions(-DCLANG_INC_DIR2="${CLANG_INCLUDE_DIR2}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# 1.  ------------ Clang/LLVM configurata  ------------
set(CLANG_LIBRARIES clangTooling clangASTMatchers)

# derived from looking at clang++ -v
# To do: get from llvm-config
set(LocalLibClang_DEFINITIONS -fPIC -fvisibility-inlines-hidden
  -Wall -Wno-unused-parameter -Wwrite-strings
  -Wcast-qual -Wmissing-field-initializers
  -pedantic -Wno-long-long -Wcovered-switch-default
  -Wnon-virtual-dtor -Wdelete-non-virtual-dtor
  -Werror=date-time -fno-exceptions -fno-rtti
  -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
add_definitions(${LocalLibClang_DEFINITIONS})

# defined in ClangConfig.cmake
include_directories(${CLANG_INCLUDE_DIRS})
include_directories(${LLVM_INCLUDE_DIRS})

# terminal info
message(STATUS "TINFO_LIB_DIR: " $ENV{TINFO_LIB_DIR})
find_library(TINFO_LIB tinfo PATHS $ENV{TINFO_LIB_DIR})
if(${TINFO_LIB-NOTFOUND})
  message(FATAL_ERROR "Could not find tinfo library!")
else()
  set(TINFO_LIBS ${TINFO_LIBS} ${TINFO_LIB})
endif()

# 1. ----------- Boost -----------
if (NOT DEFINED BOOST_INCLUDE_DIR)
  if (DEFINED ENV{BOOST_DIR})
    set(BOOST_INCLUDE_DIR $ENV{BOOST_DIR})
    message(STATUS "BOOST_INCLUDE_DIR is ${BOOST_INCLUDE_DIR}")
  elseif( DEFINED ENV{BOOST_INCLUDES})
    set(BOOST_INCLUDE_DIR $ENV{BOOST_INCLUDES})
    message(STATUS "BOOST_INCLUDE_DIR is ${BOOST_INCLUDE_DIR}")
  else()
    message(ERROR "Error: environment variable BOOST_DIR is not defined.")
    return()
  endif()
endif()
include_directories(${BOOST_INCLUDE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# 2. ---------- Library ----------
add_subdirectory(lib)

# 3. ---------- Unit Tests ------------
# default behaviour is to build gtests
if (NOT DEFINED build_gtests)
   set(build_gtests "yes")
endif()
# test for option "-D build_gtests=yes"  (-D variable is retained from prior runs)
if (${build_gtests} )
   set(CORCT_GTEST_DIR ${CMAKE_SOURCE_DIR}/test)
   if (IS_DIRECTORY ${CORCT_GTEST_DIR})
      add_subdirectory(${CORCT_GTEST_DIR})
   else()
      message("Warning: Test dir ${CORCT_GTEST_DIR} not found.")
   endif()
endif()

# 4. ----------- Appz ------------

add_subdirectory(apps)

# 5. ----------- Doxygen -----------

if (NOT DEFINED build_api_doc)
   set(build_api_doc "no")
endif()
if (${build_api_doc} )#
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
     message("Configuring Doxygen")
     configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
     # doxygen can't make a sub-dir on the fly, so we're pre-making it here
     #   this should correspond to the OUTPUT_DIRECTORY in the Doxyfile.in
     #   grep OUTPUT_DIR Doxyfile | grep -v ^# | cut -d '=' -f 2
     file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxygen)
     add_custom_target(doc ALL
       COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
       COMMENT "Generating API documentation with Doxygen" VERBATIM
     )
  endif(DOXYGEN_FOUND)
endif(${build_api_doc})

# End of file
