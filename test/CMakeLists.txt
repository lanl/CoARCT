
if (NOT DEFINED GTEST_ROOT)
	if (DEFINED ENV{GTEST_DIR})
		set(GTEST_ROOT $ENV{GTEST_DIR})
		message(STATUS "GTEST_ROOT is ${GTEST_ROOT}")
	else()
	   message(WARNING "Warning: environment variable GTEST_DIR is not defined.")
	   message(WARNING "corct Unittests will not be made")
	   message(WARNING "To make corct_unittests, set GTEST_DIR to top level gtest directory.")
	   return()
	endif()
endif()

# Define helper functions and macros used by Google Test.
include(${GTEST_ROOT}/cmake/internal_utils.cmake)

config_compiler_and_linker()  # Defined in internal_utils.cmake

include_directories(
	${CMAKE_CURRENT_LIST_DIR}/../lib
	${GTEST_ROOT}
	${GTEST_ROOT}/include
)

# build gtest
cxx_library(gtest "${cxx_strict}" ${GTEST_ROOT}/src/gtest-all.cc)
cxx_library(gtest_main "${cxx_strict}" ${GTEST_ROOT}/src/gtest_main.cc)

# build tests
#---------------------------------------------------------
# build corct_unittest1 from sources shown
set( CORCT_UNITTESTS
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

add_executable(corct_unittests ${CORCT_UNITTESTS} )

target_link_libraries(corct_unittests gtest_main gtest corct ${CLANG_LIBRARIES} ${TINFO_LIBS} z)

# End of file
