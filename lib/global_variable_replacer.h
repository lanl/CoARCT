// global_variable_replacer.h
// T. M. Kelley
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef GLOBAL_VARIABLE_REPLACER_H
#define GLOBAL_VARIABLE_REPLACER_H

#include "make_replacement.h"
#include "signature_insert.h"
#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include <iostream>

namespace corct
{
// clang-format off
/** \brief Match reference ("globalReference") to a
  global variable named g_var_name ("varName") in a function
  ("function"). */
auto mk_global_var_matcher = [](std::string const & g_var_name){
  using namespace clang::ast_matchers;
  return declRefExpr(
    to(
      varDecl(
        hasGlobalStorage()
       ,hasName(g_var_name)
      ).bind("varName")
       )
    ,hasAncestor(functionDecl().bind("function"))
    ).bind("globalReference")
  ;
}; // mk_decl_matcher
// clang-format on

/** \brief Replace use of a global variable with uses of a
    pre-determined code fragment. */
class global_variable_replacer :
  public clang::ast_matchers::MatchFinder::MatchCallback {

public :

  using matcher_t = clang::ast_matchers::StatementMatcher;

  using matchers_t = std::vector<matcher_t>;

  using replacements_t = clang::tooling::Replacements;

  /** \brief Constcorctor
    \param reps: pointer to clang::Replacements object, as in tool.getReplacements()
    \param new_var: new variable text
     */
  global_variable_replacer(
    replacements_t * reps,
    vec_str const & old_globals,
    vec_str const & new_vars,
    bool const dry_run)
    : reps_(reps), old_globals_(old_globals), new_vars_(new_vars),
    dry_run_(dry_run)
  {
    assert(old_globals_.size() == new_vars_.size());
  } // ctor


  /** Process a variable that matches the criteria. */
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
    using namespace clang;
    ASTContext *Context = Result.Context;
    SourceManager & src_manager(
      const_cast<SourceManager &>(Context->getSourceManager()));

    const DeclRefExpr * g_var = Result.Nodes.getNodeAs<DeclRefExpr>("globalReference");
    if(g_var)
    {
      string_t const gvar_name = g_var -> getNameInfo().getAsString();
      if(!in_vec(old_globals_, gvar_name))
      {
        return;
      }
      uint32_t idx = 0;
      while(gvar_name != old_globals_[idx]) idx++;

      clang::tooling::Replacement rep =
        replace_source_range(src_manager,g_var->getSourceRange(),new_vars_[idx]);
      if(!dry_run_)
      {
        reps_->insert(rep);
      }
      else
      {
        llvm::outs() << "global_var_replacer: replacement "
          << rep.toString() << "\n";
      }
    }
    return;
  } // global_replacer::run

  /** Generate a set of matchers */
  matchers_t
  matchers() const {
    matchers_t ms;
    for(auto & o : old_globals_){
      ms.push_back(mk_global_var_matcher(o));
    }
    return ms;
  }

private:
  replacements_t * reps_;
  vec_str const old_globals_;
  vec_str const new_vars_;
  bool dry_run_;
}; // class global_variable_replacer

} // corct::

#endif // include guard


// End of file
