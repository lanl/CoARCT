include_directories ("${PROJECT_SOURCE_DIR}/lib")


##add_executable(struct-field-use
##  StructFieldUser.cc
##  )
##target_link_libraries(struct-field-use
##  corct
##  ${CLANG_LIBRARIES}
##  ${LLVM_LIBRARIES}
##  ${TINFO_LIB}
##  z
## )

##add_executable(while-loop-detect
##  WhileLoopFinder.cc
##  )
##target_link_libraries(while-loop-detect
##  corct
##  ${CLANG_LIBRARIES}
##  ${LLVM_LIBRARIES}
##  ${TINFO_LIB}
##  z
## )

add_executable(global-detect
  GlobalDetect.cc
  )
target_link_libraries(global-detect
  corct
  ${CLANG_LIBRARIES}
  ${LLVM_LIBRARIES}
  ${TINFO_LIB}
  z
 )
##
##add_executable(global-replace
##  GlobalReplace.cc
##  )
##target_link_libraries(global-replace
##  corct
##  ${CLANG_LIBRARIES}
##  ${LLVM_LIBRARIES}
##  ${TINFO_LIB}
##  z
## )
##
##add_executable(loop-convert
##  LoopConvert.cpp
##  )
##target_link_libraries(loop-convert #
##  ${CLANG_LIBRARIES}
##  ${LLVM_LIBRARIES}
##  ${TINFO_LIB}
##  z
## )
##
