// CallLister.cc
// Feb 2, 2018

/* List all the places where a target function is called, and the calling
 * function in a translation unit.*/

#include "callsite_lister.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory csl_cat("callsite-list options");

static llvm::cl::opt<bool> verbose_compiler(
    "vc",
    llvm::cl::desc("pass -v to compiler instance (default false)"),
    llvm::cl::cat(csl_cat),
    llvm::cl::init(false));

static cl::opt<std::string> target_func_string(
    "tf",
    cl::desc("target function(s), separated by commas if nec. E.g. "
             "-tf=\"foo,bar,baz\""),
    cl::value_desc("target-function-string"),
    cl::cat(csl_cat));

const char * addl_help =
    "List all calls to target functions and where they are called (excluding "
    "functions defined in system headers).";

void
add_include_paths(ClangTool & tool);

int
main(int argc, const char ** argv)
{
  CommonOptionsParser OptionsParser(argc, argv, csl_cat);
  ClangTool tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  add_include_paths(tool);
  // process target functions
  corct::vec_str targ_fns(corct::split(target_func_string, ','));

  // instantiate callback and matcher
  corct::callsite_lister csl(targ_fns);
  MatchFinder finder;
  auto matchers = csl.matchers();
  for(auto & m : matchers) { finder.addMatcher(m, &csl); }
  // go!
  int rslt = tool.run(newFrontendActionFactory(&finder).get());
  std::cout << "Reported " << csl.m_num_calls << " calls\n";
  return rslt;
}

void
add_include_paths(ClangTool & tool)
{
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
  return;
}  // add_include_paths

// End of file
