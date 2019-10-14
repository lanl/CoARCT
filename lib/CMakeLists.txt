file(GLOB CORCT_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)

file(GLOB CORCT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cc )

add_library(corct ${CORCT_SRC} )

install(FILES ${CORCT_HEADERS} DESTINATION include)
install(TARGETS corct DESTINATION lib)

# End of file
