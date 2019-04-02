// signature_insert.cc
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#include "signature_insert.h"
#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/AST/Type.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>
#include <sstream>

namespace corct
{

using clang::tooling::Replacement;
using clang::CallExpr;
using clang::FunctionDecl;
using clang::SourceManager;
using clang::ParmVarDecl;

Replacement gen_new_call(
  CallExpr *call_expr
  ,FunctionDecl *func_decl
  ,str_t_cr & new_arg_text
  ,SourceManager const & sm
  ,bool const verbose
  )
{
  using namespace clang;
  using a_iter = CallExpr::arg_iterator;
  using p_iter = FunctionDecl::param_iterator;
  // SourceManager const & sm(ctx.getSourceManager());
  std::stringstream rep_str;
  // Does the function have any default arguments?
  // Big assumption here: this means that there are defaulted
  // arguments. Not sure that's always true. If not, the else
  // branch below may be borken.
  SourceLocation call_start = call_expr -> getBeginLoc();
  SourceLocation call_end = call_expr -> getEndLoc();
  if(verbose){
    std::cout << "call start: ";
    corct::dumpLocation(call_start,&sm);
    std::cout << "\ncall end: ";
    corct::dumpLocation(call_end,&sm);
    std::cout << "\n";
  }
  if(0 == call_expr-> getNumArgs())
  {
    if(verbose){
      HERE("0 args");
    }
    // what location to use?
    return prepend_source_range(sm,call_expr -> getRParenLoc(),new_arg_text);
  }
  bool const has_defaults =
    func_decl->getMinRequiredArguments() != func_decl->getNumParams();
  if(!has_defaults)
  {
    a_iter ait = call_expr -> arg_end();
    ait--;
    Expr *last(*ait);
    if(verbose){
      corct::dumpSourceRange(last->getSourceRange(), &sm);
    }
    rep_str << ", " << new_arg_text;
    return append_source_range(sm,last->getSourceRange(),rep_str.str());
  }
  else
  {
    a_iter ait = call_expr->arg_begin();
    uint32_t n(0);
    for(p_iter pit = func_decl->param_begin(); pit!= func_decl->param_end(); ++pit,++ait)
    {
      ParmVarDecl *lastp(*pit);
      n++;
      if(lastp->hasDefaultArg())
      {
        Expr *lasta(*--ait);
        if(verbose){
          std::cout << "In default action: ";
          corct::dumpSourceRange(lasta->getSourceRange(), &sm);
          std::cout << "\n";
        }
        if(n>1){
          rep_str << ", ";
        }
        rep_str << new_arg_text;
        // std::cout << "In default action: ";
        // corct::dumpSourceRange(lasta->getSourceRange(), &sm);
        // std::cout << "\n";
        return append_source_range(sm,lasta->getSourceRange(),rep_str.str());
      }
    }
  }
  std::cout << "gen_new_call: moving into undefined territory\n";
  return Replacement();  // 'unreachable'
} // gen_new_call

namespace
{
/**\brief A parameter matches if it has the same type and the same name. */
bool param_matches(ParmVarDecl *p_decl
  ,std::string const & type_name
  ,std::string const & parm_name
  ,bool const verbose=false)
{
  std::string t_name(p_decl->getType().getAsString());
  std::string p_name = p_decl -> getNameAsString();
  bool matches =
    parm_name == p_name &&
    type_name == t_name;
  if(!matches && verbose)
  {
    std::cout << "param_matches: match failed\n\ttarget type = '"
      << type_name << "', this type = '" << t_name << "'\n\ttarget name = '"
      << parm_name << "', this name = '" << p_name << "'\n";
  }
  return matches;
} // param_matches

} //anonymous::

/**\brief replace parameter with old_name with new_param.
  \param f_decl: the function declaration to work on
  \param old_name: old parameter name
  \param new_param: new parameter declaration, including type, const, &, etc. */
Replacement gen_signature_repl(
  FunctionDecl *f_decl
  ,std::string const & old_name
  ,std::string const & type_name
  ,std::string const & new_param
  ,SourceManager const & sm
  ,bool const verbose
  )
{
  // 1. Find the right parameter declaration
  using p_iter = FunctionDecl::param_iterator;
  ParmVarDecl *p_decl = nullptr;
  for(p_iter pit = f_decl->param_begin(); pit != f_decl->param_end(); pit++)
  {
    if(param_matches(*pit,type_name,old_name))
    {
      p_decl = *pit;
      if(verbose)
      {
        std::cout << "gen_signature_repl: parameter with name '"
          << (*pit)->getNameAsString() << "' matches "
          << old_name << std::endl;
      }
    }
    else
    {
      if(verbose)
      {
        std::cout << "gen_signature_repl: parameter with name '"
          << (*pit)->getNameAsString() << "' does not match "
          << old_name << std::endl;
      }
    }
  }
  // 2. Generate replacement for that parameter
  Replacement repl;
  if(nullptr != p_decl){
    repl = replace_source_range(sm,p_decl->getSourceRange(),
      new_param);
  }
  return repl;
}

/** Generate a new function signature with the new parameter as the last
    argument without a default.

    \param f_decl: the function declaration to modify
    \param new_param_text: new param to be inserted, to include const and &
    \param sm: SourceManager reference*/
Replacement gen_new_signature(
  FunctionDecl *f_decl
  ,std::string const & new_param_text
  ,SourceManager const & sm
  )
{
  using p_iter = FunctionDecl::param_iterator;
  // SourceManager const & sm(ctx.getSourceManager());
  std::stringstream rep_str;
  // Small problem: this fails if the function is declared '... f(void){...}'
  if(0 == f_decl->getNumParams()){
    return insert_only_param(sm,f_decl->getLocation(),new_param_text);
  }

  // Does the function have any default arguments?
  // Big assumption here: this means that there are defaulted
  // arguments. Not sure that's always true. If not, the else
  // branch below may be borken.
  // TODO Is there a better way to determine if a function has default arguments?
  bool const has_defaults =
    f_decl->getMinRequiredArguments() != f_decl->getNumParams();
  if(!has_defaults)
  {
    p_iter pit = f_decl-> param_end();
    pit--;
    ParmVarDecl *last(*pit);
    rep_str << ", " << new_param_text;
    return append_source_range(sm,last->getSourceRange(),rep_str.str());
  }// if(!has_defaults)
  else
  {
    // insert before first defaulted param
    for(p_iter pit = f_decl->param_begin(); pit!= f_decl->param_end(); ++pit)
    {
      ParmVarDecl *last(*pit);
      if(last->hasDefaultArg())
      {
        rep_str << new_param_text << ", ";
        return prepend_source_range(sm,last->getSourceRange(),rep_str.str());
      }
    }
  } // else...if(!has_defaults)
  return Replacement();  // 'unreachable'
} // gen_new_signature

} // corct::

// End of file
