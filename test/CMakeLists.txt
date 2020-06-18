# CMakeLists.txt for unit tests
if (NOT DEFINED GTEST_INSTALL_ROOT)
	if (DEFINED ENV{GTEST_ROOT})
		set(GTEST_INSTALL_ROOT $ENV{GTEST_ROOT})
		message(STATUS "GTEST_INSTALL_ROOT is ${GTEST_INSTALL_ROOT}")
	else()
	   message(WARNING "Warning: environment variable GTEST_ROOT is not defined.")
	   message(WARNING "corct Unittests will not be made")
     message(WARNING "To make corct_unittests, set GTEST_ROOT to top level gtest directory.")
     message(SEND_ERROR "To skip corct_unittests, set 'build_gtests' to false.")
	   return()
	endif()
endif()

set(GTEST_CMAKE_PATH ${GTEST_INSTALL_ROOT}/lib/cmake)
message(STATUS "GTEST_CMAKE_PATH: " ${GTEST_CMAKE_PATH})

# Define helper functions and macros used by Google Test.
# include(${GTEST_ROOT}/cmake/internal_utils.cmake)
find_package(GTest REQUIRED
  CONFIG
  HINTS ${GTEST_CMAKE_PATH}
  )

# build from sources shown
set( CORCT_UNITTEST_SRC
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

add_executable(corct_unittests ${CORCT_UNITTEST_SRC} )

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
  ${CLANG_LIBRARIES} ${TINFO_LIBS} z)

add_test(NAME "corct_unittests" COMMAND "corct_unittests")

# End of file
