// callsite_expander.h
// T. M. Kelley
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef CALLSITE_EXPANDER_H
#define CALLSITE_EXPANDER_H

#include "function_repl_gen.h"
#include "make_replacement.h"
#include "signature_insert.h"
#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>

namespace corct {
// clang-format off
/** Match a call to function with specified name, bound in "function". Pretty
 sure this won't pick up a function pointer (TODO need to check that).
*/
auto mk_fn_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name,
  std::string const & targ_name)
{
  using namespace clang::ast_matchers;
  return callExpr(
    unless(isExpansionInSystemHeader()),
    hasDescendant(
      declRefExpr(
        to(
          functionDecl(
            hasName(targ_name)
          ).bind(fn_bind_name)
        )
      )
    )//.bind("recDecl")
  ).bind(cs_bind_name);
} // mk_fn_call_matcher
// clang-format on

// clang-format off
/** Match a call to function with specified name, bound in "function". Pretty
 sure this won't pick up a function pointer (TODO need to check that).
*/
auto mk_mthd_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name,
  std::string const & targ_name)
{
  using namespace clang::ast_matchers;
  return cxxMemberCallExpr(
    unless(isExpansionInSystemHeader()),
    hasDescendant(
      declRefExpr(
        to(
          functionDecl(
            hasName(targ_name)
          ).bind(fn_bind_name)
        )
      )
    )//.bind("recDecl")
  ).bind(cs_bind_name);
} // mk_mthd_call_matcher
// clang-format on


struct expand_callsite_traits {
  using matcher_t = clang::ast_matchers::StatementMatcher;
};

/**\brief Exoands function callsites by adding specified argument after the
last non-default argument. */
class expand_callsite
    : public function_replacement_generator<expand_callsite_traits> {
public:
  using Base = function_replacement_generator<expand_callsite_traits>;

  const string_t fn_bind_name_ = "callee";
  const string_t cs_bind_name_ = "callsite";

  void run(result_t const & result) override
  {
    using namespace clang;
    CallExpr * call_site =
        const_cast<CallExpr *>(result.Nodes.getNodeAs<CallExpr>(cs_bind_name_));
    FunctionDecl * func_decl = const_cast<FunctionDecl *>(
        result.Nodes.getNodeAs<FunctionDecl>(fn_bind_name_));
    if(call_site && func_decl) {
      string_t callee_name = func_decl->getNameAsString();
      if(corct::in_vec(targets_, callee_name)) {
        std::cout << "callsite_expander arrived at target function: "
                  << callee_name << ":\n";
        clang::SourceManager & src_manager(const_cast<clang::SourceManager &>(
            result.Context->getSourceManager()));
        replacement_t rep =
            gen_new_call(call_site, func_decl, new_str_, src_manager);
        std::cout << "Suggested replacement: " << rep.toString() << "\n";
        if(!dry_run_) {
          reps_.insert(rep);
        }
      }  // if callee_name in targets
    }
    else {  // one of the nodes was not valid??
      check_ptr(func_decl, "func_decl");
      check_ptr(call_site, "call_site");
    }
    return;
  }  // run

  matcher_t mk_matcher(string_t const & t) const override
  {
    return mk_fn_call_matcher(cs_bind_name_,fn_bind_name_, t);
  }

  expand_callsite(Base::replacements_t & reps,
                  vec_str const & targets,
                  str_t_cr new_arg,
                  bool const dry_run)
      : function_replacement_generator(reps, targets, new_arg, dry_run)
  {}

};  // expand_callsite

}  // corct::

#endif  // include guard

// End of file
