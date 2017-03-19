// FunctionMover.cc
// Mar 17, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Find all C struct fields declared with a typedef; report the typedef and
 * underlying (desugared) type. Ignores fields declared as builtin types. */

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

struct Function_Mover
    : public clang::ast_matchers::MatchFinder::MatchCallback {

  using Replacements = clang::tooling::Replacements;

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
      SourceLocation decl_start_end(decl_range.getEnd());
      SourceLocation decl_end_end( end_of_the_end( decl_start_end,sm));
      const char * buff_begin( sm.getCharacterData(decl_begin));
      const char * buff_end( sm.getCharacterData(decl_end_end));
      std::string const func_string(buff_begin,buff_end);
      // now you have original source of declaration, output to new file etc.
      std::cout << "Captured function " << f_decl->getNameAsString()
        << " declaration:\n'''\n" << func_string << "\n'''\n";
      // Generate a replacement to eliminate the function declaration in
      // the original source
      uint32_t const decl_length =
        sm.getFileOffset(decl_end_end) - sm.getFileOffset(decl_begin);
      Replacement repl(sm,decl_begin,decl_length,"");
      repls_.insert(repl);
    }
    else {
      corct::check_ptr(f_decl, "f_decl");
    }
    return;
  }  // run

  explicit Function_Mover(Replacements & repls):repls_(repls){}

  Replacements & repls_;
};   // struct Function_Mover

static llvm::cl::OptionCategory TROpts("Common options for typedef-report");

const char * addl_help = "Find structs with fields declared via typedef";

int
main(int argc, const char ** argv)
{
  using namespace corct;
  CommonOptionsParser opt_prs(argc, argv, TROpts, addl_help);
  RefactoringTool tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());
  Function_Mover fm(tool.getReplacements());
  finder_t finder;
  std::string const function_name("foo"); // could get from CL options
  finder.addMatcher(fm.matcher(function_name), &fm);
  // runs, reports, does not overwrite:
  tool.run(newFrontendActionFactory(&finder).get());
  // invoke instead to overwrite original source:
  // tool.runAndSave(newFrontendActionFactory(&finder).get());

  std::cout << "Replacements collected"
    << " (not applied)"
    << ":\n";
  for(auto & r : tool.getReplacements()){
    std::cout << r.toString() << "\n";
  }
  return 0;
}  // main

void check_invalid(bool &invalid, const char*filename, int line){
  if(invalid){
    printf("%s:%i getCharacterData returned invalid\n",filename,line);
  }
  invalid = false;
}


// End of file
