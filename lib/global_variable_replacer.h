// global_variable_replacer.h
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef GLOBAL_VARIABLE_REPLACER_H
#define GLOBAL_VARIABLE_REPLACER_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include "make_replacement.h"
#include "signature_insert.h"
#include "types.h"
#include "utilities.h"
#include <iostream>

namespace corct {
// clang-format off
/** \brief Match reference (bound to gref_bind_name) to a
  global variable named g_var_name in a function. */
auto mk_global_var_matcher(str_t_cr g_var_name, str_t_cr gref_bind_name){
  using namespace clang::ast_matchers;
  return declRefExpr(
    to(
      varDecl(
        hasGlobalStorage()
       ,hasName(g_var_name)
      )
       )
    ,hasAncestor(functionDecl() )
    ).bind(gref_bind_name)
  ;
}; // mk_global_var_matcher
// clang-format on

/** \brief Replace use of a global variable with of a
    pre-determined code fragment. */
class global_variable_replacer
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  using matcher_t = clang::ast_matchers::StatementMatcher;
  using matchers_t = std::vector<matcher_t>;
  using replacements_t = clang::tooling::Replacements;

  static const string_t gref_bind_name;

  /** \brief Constructor
    \param reps: pointer to clang::Replacements object, as in
    tool.getReplacements()
    \param new_var: new variable text
     */
  global_variable_replacer(replacements_map_t & rep_map,
                           vec_str const & old_globals,
                           vec_str const & new_vars,
                           bool const dry_run)
      : rep_map_(rep_map),
        old_globals_(old_globals),
        new_vars_(new_vars),
        dry_run_(dry_run)
  {
    assert(old_globals_.size() == new_vars_.size());
  }  // ctor

  /** Process a variable that matches the criteria. */
  virtual void run(result_t const & result)
  {
    using namespace clang;
    SourceManager & src_manager(
        const_cast<SourceManager &>(result.Context->getSourceManager()));
    const DeclRefExpr * g_var =
        result.Nodes.getNodeAs<DeclRefExpr>(gref_bind_name);
    if(g_var) {
      string_t const gvar_name = g_var->getNameInfo().getAsString();
      if(!in_vec(old_globals_, gvar_name)) { return; }
      uint32_t idx = 0;
      while(gvar_name != old_globals_[idx]) idx++;

      clang::tooling::Replacement rep = replace_source_range(
          src_manager, g_var->getSourceRange(), new_vars_[idx]);
      if(!dry_run_) {
        auto & reps = find_repls(g_var, src_manager, rep_map_);
        if(reps.add(rep)) { HERE("add replacement failed"); }
      }
      else {
        llvm::outs() << "global_var_replacer: replacement " << rep.toString()
                     << "\n";
      }
    }
    else {
      check_ptr(g_var, "g_var");
    }
    return;
  }  // global_replacer::run

  /** Generate a set of matchers */
  matchers_t matchers() const
  {
    matchers_t ms;
    for(auto & o : old_globals_) {
      ms.push_back(mk_global_var_matcher(o, gref_bind_name));
    }
    return ms;
  }

private:
  replacements_map_t & rep_map_;
  vec_str const old_globals_;
  vec_str const new_vars_;
  bool dry_run_;
};  // class global_variable_replacer

const string_t global_variable_replacer::gref_bind_name = "globalReference";

}  // namespace corct

#endif  // include guard

// End of file
