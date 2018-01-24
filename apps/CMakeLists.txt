include_directories ("${PROJECT_SOURCE_DIR}/lib")

add_library(corct-support summarize_command_line.cc)

set(APPS_LIBRARIES
  corct
  corct-support
  clangTooling
  ${TINFO_LIB}
  z
  c
)

add_executable(function-mover FunctionMover.cc )
target_link_libraries(function-mover ${APPS_LIBRARIES} )

add_executable(template-vars-report TemplateVarFinder.cc )
target_link_libraries(template-vars-report ${APPS_LIBRARIES} )

add_executable(temp-type-report TemplateType.cc )
target_link_libraries(temp-type-report ${APPS_LIBRARIES} )

add_executable(function-printer FunctionPrinter.cc )
target_link_libraries(function-printer ${APPS_LIBRARIES} )

add_executable(typedef-report TypedefFinder.cc )
target_link_libraries(typedef-report ${APPS_LIBRARIES} )

add_executable(global-replace GlobalReplace.cc )
target_link_libraries(global-replace ${APPS_LIBRARIES} )

add_executable(struct-field-use StructFieldUser.cc )
target_link_libraries(struct-field-use ${APPS_LIBRARIES} )

add_executable(func-decl-lister-rav FuncListerRAV.cc )
target_link_libraries(func-decl-lister-rav ${APPS_LIBRARIES} )

add_executable(func-decl-lister-am FuncListerAM.cc )
target_link_libraries(func-decl-lister-am ${APPS_LIBRARIES} )

# add_executable(while-loop-detect WhileLoopFinder.cc )
##target_link_libraries(while-loop-detect ${APPS_LIBRARIES} )
###  corct
###  ${CLANG_LIBRARIES}
###  ${LLVM_LIBRARIES}
###  ${TINFO_LIB}
###  z
### )
#
add_executable(global-detect GlobalDetect.cc )
target_link_libraries(global-detect ${APPS_LIBRARIES} )

###add_executable(loop-convert
###  LoopConvert.cpp
###  )
##target_link_libraries(loop-convert # ${APPS_LIBRARIES} )
###  ${CLANG_LIBRARIES}
###  ${LLVM_LIBRARIES}
###  ${TINFO_LIB}
###  z
### )
###
