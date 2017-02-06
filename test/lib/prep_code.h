// prep_code.h
// T. M. Kelley
// Feb 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved


#ifndef PREP_CODE_H
#define PREP_CODE_H

#include "types.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h" //  //buildASTFromCode

using ASTUPtr = std::unique_ptr<clang::ASTUnit>;

/**\brief Compile code fragment to AST.
\param code: valid c++ code
\return {unique_ptr<AST>,ASTContext *, TranslationDecl *}
*/
inline auto
prep_code(corct::str_t_cr code)
{
  ASTUPtr ast(clang::tooling::buildASTFromCode(code));
  clang::ASTContext * pctx = &(ast->getASTContext());
  clang::TranslationUnitDecl * decl = pctx->getTranslationUnitDecl();
  return std::make_tuple(std::move(ast), pctx, decl);
}


#endif // include guard


// End of file
