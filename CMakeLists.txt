# -*- Makefile -*-
cmake_minimum_required(VERSION 3.3)

# Compiler-driven Refactoring with Clang Tools
project (CoARCT LANGUAGES CXX)


# 1.  ------------ Clang/LLVM configurata  ------------

# This builds up the CLANG_LIBRARIES flag which can be used in various places.
MACRO(APPEND_CLANG_LIB _libname_)
find_library(CLANG_${_libname_}_LIB ${_libname_} PATHS $ENV{LLVM_LIB_DIR} $ENV{CLANG_LIB_DIR})
if (CLANG_${_libname_}_LIB)
   set(CLANG_LIBRARIES ${CLANG_LIBRARIES} ${CLANG_${_libname_}_LIB})
endif()
ENDMACRO()

# This set of libs may vary with LLVM/Clang version

APPEND_CLANG_LIB(clangAST)
APPEND_CLANG_LIB(clangAnalysis)
APPEND_CLANG_LIB(clangBasic)
APPEND_CLANG_LIB(clangDriver)
APPEND_CLANG_LIB(clangFormat)
APPEND_CLANG_LIB(clangFrontend)
APPEND_CLANG_LIB(clangFrontendTool)
APPEND_CLANG_LIB(clangLex)
APPEND_CLANG_LIB(clangParse)
APPEND_CLANG_LIB(clangSema)
APPEND_CLANG_LIB(clangEdit)
APPEND_CLANG_LIB(clangASTMatchers)
APPEND_CLANG_LIB(clangRewrite)
APPEND_CLANG_LIB(clangRewriteFrontend)
APPEND_CLANG_LIB(clangStaticAnalyzerFrontend)
APPEND_CLANG_LIB(clangStaticAnalyzerCheckers)
APPEND_CLANG_LIB(clangStaticAnalyzerCore)
APPEND_CLANG_LIB(clangSerialization)
APPEND_CLANG_LIB(clangToolingCore)
APPEND_CLANG_LIB(clangTooling)

APPEND_CLANG_LIB(LLVMAnalysis)
APPEND_CLANG_LIB(LLVMMCParser)
APPEND_CLANG_LIB(LLVMMC)
APPEND_CLANG_LIB(LLVMBitReader)
APPEND_CLANG_LIB(LLVMProfileData)
APPEND_CLANG_LIB(LLVMOption)
APPEND_CLANG_LIB(LLVMSupport)
APPEND_CLANG_LIB(LLVMCore)

# derived from looking at clang++ -v

set(LocalLibClang_DEFINITIONS -fPIC -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers -pedantic -Wno-long-long -Wcovered-switch-default -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Werror=date-time -std=c++14 -O3 -DNDEBUG  -fno-exceptions -fno-rtti -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
add_definitions(${LocalLibClang_DEFINITIONS})


include_directories($ENV{LibClang_INCLUDE_DIR})

set(LLVM_LDFLAGS "-Wl,-search_paths_first -Wl,-headerpad_max_install_names")
set(CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS})

# term info
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
