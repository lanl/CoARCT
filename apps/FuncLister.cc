// FuncLister.cc
// Jan 26, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* List all the functions defined in a translation unit, with other enlightening
 * information
 .*/

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "function_definition_lister.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory flt_cat("func-decl-list-am options");

static llvm::cl::opt<bool> verbose_compiler(
    "vc",
    llvm::cl::desc("pass -v to compiler instance (default false)"),
    llvm::cl::cat(flt_cat),
    llvm::cl::init(false));

const char * addl_help =
    "List all functions that are defined in this translation unit, excluding "
    "functions defined in system headers.";

int
main(int argc, const char ** argv)
{
  CommonOptionsParser OptionsParser(argc, argv, flt_cat);
  ClangTool tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  // add header search paths to compiler
  ArgumentsAdjuster ardj1 =
      getInsertArgumentAdjuster(corct::clang_inc_dir1.c_str());
  ArgumentsAdjuster ardj2 =
      getInsertArgumentAdjuster(corct::clang_inc_dir2.c_str());
  tool.appendArgumentsAdjuster(ardj1);
  tool.appendArgumentsAdjuster(ardj2);
  if(verbose_compiler) {
    ArgumentsAdjuster ardj3 = getInsertArgumentAdjuster("-v");
    tool.appendArgumentsAdjuster(ardj3);
  }
  // instantiate callback and matcher
  corct::FunctionDefLister fl("f_decl");
  MatchFinder finder;
  finder.addMatcher(fl.matcher(), &fl);
  // go!
  int rslt = tool.run(newFrontendActionFactory(&finder).get());
  std::cout << "Reported " << fl.m_num_funcs << " functions\n";
  return rslt;
}

// End of file
