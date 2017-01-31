// FuncListerAM.cc
// T. M. Kelley
// Jan 26, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Uses AST Matcher to list all the functions in a translation unit,
 * skipping the ones that are defined in headers. Compare with FuncListerRAV,
 * which uses RecursiveASTVisitor. */

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

uint32_t n_matches = 0;
uint32_t n_skipped_matches = 0;

void print_func(clang::FunctionDecl const * const fdecl){
  std::cout << "Function '" << (fdecl->getNameAsString()) << "' defined\n";
  fdecl->dump();
  return;
} // print_func

auto mk_fn_decl_matcher(){
  return functionDecl(isExpansionInMainFile()).bind("fdecl");
}

struct FuncPrinter : public MatchFinder::MatchCallback{
  void run(MatchFinder::MatchResult const & result) override {
    using namespace clang;
    FunctionDecl const * fdecl =
        result.Nodes.getNodeAs<FunctionDecl>("fdecl");
    if(fdecl){
      n_matches++;
      print_func(fdecl);
    }
    else { std::cerr << "Invalid fdecl\n";}
    return;
  } // run
}; // FuncPrinter

auto mk_fn_skipper_matcher(){
  return functionDecl(unless(isExpansionInMainFile())).bind("fdecl");
}
struct FuncSkipper : public MatchFinder::MatchCallback{
  void run(MatchFinder::MatchResult const & result) override {
    using namespace clang;
    FunctionDecl const * fdecl =
        result.Nodes.getNodeAs<FunctionDecl>("fdecl");
    if(fdecl){
      std::cout << "Skipping " << fdecl->getNameAsString()
                << " not from target file\n";
      n_skipped_matches++;
    }
    else { std::cerr << "Invalid fdecl\n";}
    return;
  } // run
}; // FuncSkipper

static llvm::cl::OptionCategory flt_cat("func-decl-list-am options");

int main(int argc, const char **argv){
  CommonOptionsParser OptionsParser(argc, argv, flt_cat);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  FuncPrinter fp;
  FuncSkipper fs;
  MatchFinder finder;
  finder.addMatcher(mk_fn_decl_matcher(),&fp);
  finder.addMatcher(mk_fn_skipper_matcher(),&fs);
  int rslt = Tool.run(newFrontendActionFactory(&finder).get());
  std::cout << "Reported " << n_matches << " functions\n";
  std::cout << "Skipped " << n_skipped_matches << " functions\n";
  return rslt;
}

// End of file
