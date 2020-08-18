# CMakeLists.txt for unit tests
if (NOT DEFINED GTEST_ROOT)
  if (DEFINED ENV{GTEST_DIR})
    set(GTEST_ROOT $ENV{GTEST_DIR})
    message(STATUS "GTEST_ROOT is ${GTEST_ROOT}")
  elseif (DEFINED ENV{GTEST_ROOT})
    set(GTEST_ROOT $ENV{GTEST_ROOT})
    message(STATUS "GTEST_ROOT is ${GTEST_ROOT}")
	else()
	   message(WARNING "Warning: environment variable GTEST_ROOT is not defined.")
	   message(WARNING "corct Unittests will not be made")
	   message(WARNING "To make corct_unittests, set GTEST_DIR to top level gtest directory.")
	   return()
	endif()
endif()

set(GTEST_CMAKE_PATH ${GTEST_ROOT}/lib/cmake )

find_package(GTest REQUIRED
  CONFIG
  HINTS ${GTEST_CMAKE_PATH}
  )

set( CORCT_UNITTESTS_SRC
  lib/callsite_expander_test.cc
  lib/callsite_lister_test.cc
  lib/clang_utilities_test.cc
  lib/function_common_test.cc
  lib/function_def_lister_test.cc
  lib/function_sig_exp_test.cc
  # lib/function_sig_matchers_test.cc   ## not working on Linux??
  lib/global_matchers_test.cc
  lib/small_matchers_test.cc
  lib/struct_field_users_test.cc
  lib/template_var_matchers_test.cc
  lib/utilities_test.cc
)

add_executable(corct_unittests ${CORCT_UNITTESTS_SRC})

target_include_directories(corct_unittests
  PUBLIC
    ${GTEST_INSTALL_ROOT}/include
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/../lib
)

target_link_libraries(corct_unittests
  GTest::gtest
  GTest::gtest_main
  corct
  ${CLANG_LIBRARIES} 
  ${TINFO_LIBS} 
  z
  )

# End of file
