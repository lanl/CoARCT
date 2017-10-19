// TemplateType.cc
// June 7, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* For variables that are constructor expressions, and with type that is
 * a class template specialization, list its template arguments. */

#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;

struct TemplateType_Reporter
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  std::string const ctor_bd_name_ = "ctor_expr";
  std::string const var_bd_name_ = "var_decl";
  std::string const sp_dcl_bd_name_ = "spec_decl";

  auto matcher()
  {
    using namespace clang::ast_matchers;
    // clang-format off
    return
    varDecl(
      has(
        cxxConstructExpr().bind(ctor_bd_name_)
      )
     ,hasType(
        classTemplateSpecializationDecl().bind(sp_dcl_bd_name_)
      )
    ).bind(var_bd_name_);
    // clang-format on
  }  // matcher

  virtual
  void run(corct::result_t const & result) override {
    using namespace clang;
    using corct::check_ptr;
    using CTSD = ClassTemplateSpecializationDecl;
    SourceManager & sm(result.Context->getSourceManager());
    CTSD * spec_decl =
        const_cast<CTSD *>(result.Nodes.getNodeAs<CTSD>(sp_dcl_bd_name_));
    VarDecl * var_decl =
        const_cast<VarDecl *>(result.Nodes.getNodeAs<VarDecl>(var_bd_name_));
    if(spec_decl && var_decl) {
      // get the template arguments
      TemplateArgumentList const &tal(spec_decl->getTemplateArgs());
      for(unsigned i = 0; i < tal.size(); ++i){
        TemplateArgument const &ta(tal[i]);
        // If this arg is a type arg, get its name
        TemplateArgument::ArgKind k(ta.getKind());
        std::string argName = "";
        if(k==TemplateArgument::ArgKind::Type){
          QualType t = ta.getAsType();
          argName = t.getAsString();
        }
        // Could do similar for integral args, etc...
        std::cout << "For variable declared at "
          << corct::sourceRangeAsString(var_decl->getSourceRange(),&sm) << ":"
          << spec_decl->getNameAsString()
          << ": template arg " << (i+1) << ": " << argName << std::endl;
      }
    }
    else {
      check_ptr(spec_decl, "spec_decl");
      check_ptr(var_decl, "var_decl");
    }
    return;
  }  // run
};  // struct Typedef_Reporter

static llvm::cl::OptionCategory TROpts("Common options for temp-type-report");

const char * addl_help = "Report on variables that are template specializations";

int
main(int argc, const char ** argv)
{
  using namespace corct;
  CommonOptionsParser opt_prs(argc, argv, TROpts, addl_help);
  RefactoringTool Tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());
  TemplateType_Reporter tr;
  finder_t finder;
  finder.addMatcher(tr.matcher(), &tr);
  Tool.run(newFrontendActionFactory(&finder).get());
  return 0;
}  // main

// End of file
