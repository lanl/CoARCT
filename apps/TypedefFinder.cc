// TypedefFinder.cc
// Feb 14, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Find all C struct fields declared with a typedef; report the typedef and
 * underlying (desugared) type. Ignores fields declared as builtin types. */

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

struct Typedef_Reporter
    : public clang::ast_matchers::MatchFinder::MatchCallback {
  std::string const ty_bd_name_ = "type_decl";
  std::string const fd_bd_name_ = "fld_decl";

  auto matcher()
  {
    using namespace clang::ast_matchers;
    // clang-format off
    return
    fieldDecl(
      hasType(
        typedefType().bind(ty_bd_name_)
      )//hasType
    ).bind(fd_bd_name_);
    // clang-format on
  }  // matcher

  virtual void run(corct::result_t const & result) override
  {
    using namespace clang;
    FieldDecl * f_decl =
        const_cast<FieldDecl *>(result.Nodes.getNodeAs<FieldDecl>(fd_bd_name_));
    TypedefType * tt = const_cast<TypedefType *>(
        result.Nodes.getNodeAs<TypedefType>(ty_bd_name_));
    if(f_decl && tt) {
      // QualType qt = tt->desugar();   // good to see
      QualType ut = tt->getDecl()->getUnderlyingType();
      TypedefNameDecl * tnd = tt->getDecl();
      std::string const struct_name = f_decl->getParent()->getNameAsString();
      std::string const fld_name = f_decl->getNameAsString();
      // std::string const ty_name = qt.getAsString();
      std::string ut_name = ut.getAsString();
      std::string tnd_name = tnd->getNameAsString();
      std::cout << "Struct '" << struct_name << "' declares field '" << fld_name
                << " with typedef name = '" << tnd_name << "'"
                << ", underlying type = '" << ut_name << "'" << std::endl;
    }
    else {
      corct::check_ptr(f_decl, "f_decl");
      corct::check_ptr(tt, "tt");
    }
    return;
  }  // run
};   // struct Typedef_Reporter

static llvm::cl::OptionCategory TROpts("Common options for typedef-report");

const char * addl_help = "Find structs with fields declared via typedef";

int
main(int argc, const char ** argv)
{
  using namespace corct;
  CommonOptionsParser opt_prs(argc, argv, TROpts, addl_help);
  RefactoringTool Tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());
  Typedef_Reporter tr;
  finder_t finder;
  finder.addMatcher(tr.matcher(), &tr);
  Tool.run(newFrontendActionFactory(&finder).get());
  return 0;
}  // main

// End of file
