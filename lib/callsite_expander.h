// callsite_expander.h
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

/** Match a call to bound member function with specified name, with
node bound to 'fn_bind_name'. This will work with with object,
reference, or pointer to object.
*/
auto mk_mthd_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name,
  std::string const & targ_name)
{
  using namespace clang::ast_matchers;
  return cxxMemberCallExpr(
    unless(isExpansionInSystemHeader()),
    callee(
      cxxMethodDecl(
        hasName(targ_name)
      ).bind(fn_bind_name) // cxxMethodDecl
    ) // callee
  ).bind(cs_bind_name);
} // mk_mthd_call_matcher
// clang-format on

struct expand_callsite_traits {
  using matcher_t = clang::ast_matchers::StatementMatcher;
};

/**\brief Expands function callsites by adding specified argument after the
last non-default argument in every call to each target function. For example,
given
    void foo(int bar, double baz = 3.14159);
and
    expand_callsite ec(reps,{"foo","fi","fum"},"quz",false)
then ec will replace
    foo(ibar,dbaz);
with
    foo(ibar,quz,dbaz);
and
    foo(ibar);
with
    foo(ibar,quz);

This currently has the (potential) shortcoming that it's not very exact about
the differences between free function calls and method calls. With a bit of
work, it can be used for either. See TEST(expand_callsite,case6_method_expands)
for an example of how this works.
 */
class expand_callsite
    : public function_replacement_generator<expand_callsite_traits> {
public:
  using Base = function_replacement_generator<expand_callsite_traits>;

  static const string_t fn_bind_name_;
  static const string_t cs_bind_name_;

  void run(result_t const & result) override
  {
    using namespace clang;
    CallExpr * call_site =
        const_cast<CallExpr *>(result.Nodes.getNodeAs<CallExpr>(cs_bind_name_));
    FunctionDecl * func_decl = const_cast<FunctionDecl *>(
        result.Nodes.getNodeAs<FunctionDecl>(fn_bind_name_));
    if(call_site && func_decl) {
      string_t callee_name = func_decl->getNameAsString();
      if(corct::in_vec(fn_targets_, callee_name) ||
         corct::in_vec(mthd_targets_, callee_name)) {
        if(verbose_){
          std::cout << "callsite_expander arrived at target function: "
                    << callee_name << ":\n";
        }
        clang::SourceManager & src_manager(const_cast<clang::SourceManager &>(
            result.Context->getSourceManager()));
        replacement_t rep =
            gen_new_call(call_site, func_decl, new_str_, src_manager, verbose_);
        if(verbose_){
          std::cout << "Suggested replacement: " << rep.toString() << "\n";
        }
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

  matcher_t mk_fn_matcher(string_t const & t) const override
  {
    return mk_fn_call_matcher(cs_bind_name_,fn_bind_name_, t);
  }

  matcher_t mk_mthd_matcher(string_t const & t) const override
  {
    return mk_mthd_call_matcher(cs_bind_name_,fn_bind_name_, t);
  }

  expand_callsite(Base::replacements_t & reps,
                  vec_str const & targets,
                  str_t_cr new_arg,
                  bool const dry_run,
                  bool const verbose = false)
      : function_replacement_generator(reps, targets, new_arg, dry_run)
      , verbose_(verbose)
  {}

private:
  bool const verbose_;
};  // expand_callsite

const string_t expand_callsite::fn_bind_name_ = "callee";
const string_t expand_callsite::cs_bind_name_ = "callsite";

}  // corct::

#endif  // include guard

// End of file
