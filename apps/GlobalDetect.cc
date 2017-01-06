// GlobalDetect.cc
// T. M. Kelley
// Oct. 6, 2016
// (c) Copyright 2016-7 LANSLLC, all rights reserved

#include "global_matchers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

static cl::extrahelp MoreHelp(
    "\nReport all functions that use global variable, or all sites in which "
    "global variables  are used");

// command line options:
static llvm::cl::OptionCategory GDOpts("global-detect options");

static cl::opt<std::string> old_var_string(
    "old",
    cl::desc("global variable to seek (default: detect all globals)"),
    cl::value_desc("old-var-string"),
    cl::cat(GDOpts),
    cl::init(""));

static cl::opt<bool> dry_run("d",
                             cl::desc("dry run"),
                             cl::cat(GDOpts),
                             cl::init(false));

static cl::opt<bool> report_functions(
    "f",
    cl::desc("just report functions that use the target global"),
    cl::cat(GDOpts),
    cl::init(false));

int
main(int argc, const char ** argv)
{
  using namespace corct;
  CommonOptionsParser OptionsParser(argc, argv, GDOpts);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  Global_Printer printer(std::cout);
  StatementMatcher global_var_matcher =
      (old_var_string == "")
          ? all_global_var_matcher()
          : mk_global_var_matcher(old_var_string);
  DeclarationMatcher global_func_matcher =
      (old_var_string == "")
          ? all_global_fn_matcher()
          : mk_global_fn_matcher(old_var_string);

  clang::ast_matchers::MatchFinder finder;
  if(report_functions) {
    finder.addMatcher(global_func_matcher, &printer);
  }
  else {
    finder.addMatcher(global_var_matcher, &printer);
  }
  return Tool.run(newFrontendActionFactory(&finder).get());
} // main

// End of file
