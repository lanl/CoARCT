// signature_insert.h
// T. M. Kelley
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef SIGNATURE_INSERT_H
#define SIGNATURE_INSERT_H

#include "types.h"
#include "clang/ASTMatchers/ASTMatchers.h"

// forward decls
namespace clang
{
class FunctionDecl;
class SourceManager;
class CallExpr;

namespace tooling{

class Replacement;

} // clang::Tooling::

} // clang::

namespace corct
{

clang::tooling::Replacement gen_new_signature(
  clang::FunctionDecl *f_decl
 ,string_t const & new_param_text
 ,clang::SourceManager const & sm
  );

clang::tooling::Replacement gen_new_call(
  clang::CallExpr *call_expr
  ,clang::FunctionDecl *func_decl
  ,std::string const & new_arg_text
  ,clang::SourceManager const & sm
  ,bool const verbose = false
  );

clang::tooling::Replacement gen_signature_repl(
  clang::FunctionDecl *f_decl
  ,std::string const & old_name
  ,std::string const & type_name
  ,std::string const & new_param
  ,clang::SourceManager const & sm
  ,bool const verbose = false
  );


} // corct::

#endif // include guard


// End of file
