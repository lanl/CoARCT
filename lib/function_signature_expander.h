// function_signature_expander.h
// T. M. Kelley
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef FUNCTION_SIGNATURE_EXPANDER_H
#define FUNCTION_SIGNATURE_EXPANDER_H

#include "function_repl_gen.h"
#include "signature_insert.h"
#include "types.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include <iostream>

namespace corct
{
/** Match a function (declaration bound to 'fn_bind_name') declaration
 matching specified name that is not in a system header. */
auto mk_fn_decl_matcher(str_t_cr fn_name, str_t_cr fn_bind_name){
  using namespace clang::ast_matchers;
  // clang-format off
  return
    functionDecl(
      unless(isExpansionInSystemHeader()),
      hasName(fn_name)
    ).bind(fn_bind_name);
} // mk_fn_decl_matcher

auto mk_mthd_decl_matcher(str_t_cr fn_name, str_t_cr fn_bind_name){
  using namespace clang::ast_matchers;
  return
    cxxMethodDecl(
      unless(isExpansionInSystemHeader()),
      hasName(fn_name)
    ).bind(fn_bind_name); // cxxMethodDecl
}
// clang-format on

struct expand_signature_traits
{
  using matcher_t = clang::ast_matchers::DeclarationMatcher;
};

/** Inserts the specified parameter into the signature of
    the target functions. The parameter is inserted after the
    last non-defaulted parameter. */
class function_signature_expander :
  public function_replacement_generator<expand_signature_traits>{
public :
  using Base = function_replacement_generator<expand_signature_traits>;
  static const string_t fn_bind_name_;

  void run(result_t const & result) override {
    clang::FunctionDecl * func_decl = const_cast<clang::FunctionDecl *>(
      result.Nodes.getNodeAs<clang::FunctionDecl>(fn_bind_name_));
    clang::SourceManager & src_manager(
      const_cast<clang::SourceManager &>(result.Context->getSourceManager()));
    if(func_decl){
      replacement_t rep = gen_new_signature(func_decl,new_str_,src_manager);
      if(!dry_run_)
      {
        reps_.insert(rep);
      }
    }
    else{
      check_ptr(func_decl,"func_decl");
    }
    return;
  } // run

  matcher_t mk_fn_matcher(str_t_cr target) const override {
    return mk_fn_decl_matcher(target,fn_bind_name_);
  }

  matcher_t mk_mthd_matcher(str_t_cr target) const override {
    return mk_mthd_decl_matcher(target,fn_bind_name_);
  }

  /** \brief Ctor
    \param reps: pointer to clang::Replacements object, as in tool.getReplacements()
    \param new_param: new parameter text, e.g. "std::vector<block_t> & blocks"
    \param targ_fns: vector of target functions
    \param dry_run: unsure about all this?
     */
  function_signature_expander(
    replacements_t & reps,
    vec_str const & targ_fns,
    string_t const & new_param,
    bool const dry_run)
    : Base(reps,targ_fns,new_param,dry_run)
  {} // ctor
}; // function_signature_expander

const string_t function_signature_expander::fn_bind_name_ = "fdecl";
} // corct::

#endif // include guard


// End of file
