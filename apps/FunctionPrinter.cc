// FunctionPrinter.cc
// Mar 17, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Report function declarations with a given name. */

#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <string>

using namespace clang::tooling;
using namespace llvm;
using clang::SourceLocation;
using clang::SourceManager;

void check_invalid(bool &invalid, const char*filename, int line);

SourceLocation
end_of_the_end(SourceLocation const & start_of_end, SourceManager & sm)
{
  using namespace clang;
  LangOptions lopt;
  return Lexer::getLocForEndOfToken(start_of_end, 0, sm, lopt);
}

struct Function_Printer
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  std::string const fd_bd_name_ = "f_decl";

  auto matcher(std::string const & fname)
  {
    using namespace clang::ast_matchers;
    // clang-format off
    return
    functionDecl(
      hasName(fname)
    ).bind(fd_bd_name_);
    // clang-format on
  }  // matcher

  virtual void run(corct::result_t const & result) override
  {
    using namespace clang;
    FunctionDecl * f_decl = const_cast<FunctionDecl *>(
        result.Nodes.getNodeAs<FunctionDecl>(fd_bd_name_));
    if(f_decl) {
      SourceManager &sm(result.Context->getSourceManager());
      SourceRange decl_range(f_decl->getSourceRange());
      SourceLocation decl_begin(decl_range.getBegin());
      SourceLocation decl_end_end( end_of_the_end( decl_range.getEnd(),sm));
      const char * buff_begin( sm.getCharacterData(decl_begin));
      const char * buff_end( sm.getCharacterData(decl_end_end));
      std::string const func_string(buff_begin,buff_end);
      std::cout << "Captured function " << f_decl->getNameAsString()
        << " declaration:\n'''\n" << func_string << "\n'''\n";

    }
    else {
      corct::check_ptr(f_decl, "f_decl");
    }
    return;
  }  // run
};   // struct Function_Printer

static llvm::cl::OptionCategory FPOpts("Common options for function-printer");

const char * addl_help = "Print function definitions with given name";

static llvm::cl::opt<std::string> function_name(
    "f",
    cl::desc("Function name to search for"),
    cl::value_desc("function-name"),
    cl::cat(FPOpts),
    cl::init("foo"));

int
main(int argc, const char ** argv)
{
  using namespace corct;
  CommonOptionsParser opt_prs(argc, argv, FPOpts, addl_help);
  RefactoringTool Tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());
  Function_Printer fp;
  finder_t finder;
  finder.addMatcher(fp.matcher(function_name), &fp);
  Tool.run(newFrontendActionFactory(&finder).get());
  return 0;
}  // main

void check_invalid(bool &invalid, const char*filename, int line){
  if(invalid){
    printf("%s:%i getCharacterData returned invalid\n",filename,line);
  }
  invalid = false;
}


// End of file
