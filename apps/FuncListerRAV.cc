// FuncListerRAV.cc
// Jan 25, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Uses RecrusiveASTVisitor to list all the functions in a translation unit,
 * skipping the ones that are defined in headers. Compare with FuncListerAM,
 * which uses AST Matchers to do the same thing.
 */

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>

uint32_t num_funcs = 0;
uint32_t num_skipped_funcs = 0;

void
print_func(clang::FunctionDecl * fdecl)
{
  std::cout << "Function '" << (fdecl->getNameAsString()) << "' defined\n";
  fdecl->dump();
  return;
}  // print_func

class FunctionLister : public clang::RecursiveASTVisitor<FunctionLister> {
public:
  clang::ASTContext * ast_ctx_;

  explicit FunctionLister(clang::CompilerInstance * ci)
      : ast_ctx_(&(ci->getASTContext()))
  {
  }

  bool VisitFunctionDecl(clang::FunctionDecl * fdecl)
  {
    clang::SourceManager & sm(ast_ctx_->getSourceManager());
    // cf cfe-3.9.0.src/include/clang/ASTMatchers/ASTMatcher.h:209-214
    bool const inMainFile(
        sm.isInMainFile(sm.getExpansionLoc(fdecl->getBeginLoc())));
    if(inMainFile) {
      num_funcs++;
      print_func(fdecl);
    }
    else {
      std::cout << "Skipping " << fdecl->getNameAsString()
                << " not from target file\n";
      num_skipped_funcs++;
    }
    return true;
  }  // VisitFunctionDecl
};   // FunctionLister

class FunctionListerConsumer : public clang::ASTConsumer {
public:
  virtual void HandleTranslationUnit(clang::ASTContext & ctx)
  {
    lister_.TraverseDecl(ctx.getTranslationUnitDecl());
  }

  explicit FunctionListerConsumer(clang::CompilerInstance * ci) : lister_(ci) {}

private:
  FunctionLister lister_;
};  // FunctionListerConsumer

class FuncListerAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance & ci,
      llvm::StringRef file)
  {
    return std::unique_ptr<clang::ASTConsumer>(new FunctionListerConsumer(&ci));
  }
};  // FuncListerAction

static llvm::cl::OptionCategory flt_cat("func-decl-list options");

int
main(int argc, const char ** argv)
{
  using namespace clang::tooling;
  CommonOptionsParser op(argc, argv, flt_cat);
  ClangTool tool(op.getCompilations(), op.getSourcePathList());
  int result = tool.run(newFrontendActionFactory<FuncListerAction>().get());
  std::cout << "Reported " << num_funcs << " functions\n";
  std::cout << "Skipped " << num_skipped_funcs << " functions\n";
  return result;
}
// End of file
