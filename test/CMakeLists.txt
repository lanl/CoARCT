
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

# add compile flags for CORCT tests if running cmake directly on testG directory
if (NOT DEFINED CORCT_COMPILE_FLAGS)
	set(CORCT_COMPILE_FLAGS "-std=c++14 -g")
	# append CMAKE_CXX_FLAGS w/ above variable
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CORCT_COMPILE_FLAGS}")
endif()

include_directories(
	${CMAKE_CURRENT_LIST_DIR}/../lib
	${GTEST_ROOT}
	${GTEST_ROOT}/include
)

# build gtest
cxx_library(gtest "${cxx_strict}" ${GTEST_ROOT}/src/gtest-all.cc)
cxx_library(gtest_main "${cxx_strict}" ${GTEST_ROOT}/src/gtest_main.cc)
target_link_libraries(gtest_main gtest corct)

# build tests
#---------------------------------------------------------
# build corct_unittest1 from sources shown
file(GLOB CORCT_UNITTESTS
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cc
)

add_executable(corct_unittests ${CORCT_UNITTESTS} )

target_link_libraries(corct_unittests gtest_main ${CLANG_LIBRARIES} ${TINFO_LIBS} z)

# End of file