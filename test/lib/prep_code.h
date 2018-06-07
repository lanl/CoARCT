// prep_code.h
// Feb 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved


#ifndef PREP_CODE_H
#define PREP_CODE_H

#include "types.h"
#include "utilities.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Tooling/Tooling.h" //  //buildASTFromCode

using ASTUPtr = std::unique_ptr<clang::ASTUnit>;

/**\brief Compile code fragment to AST.
 * \param code: valid c++ code
 * \return {unique_ptr<AST>,ASTContext *, TranslationDecl *}
 *
 * Will provide -std=c++14 and  Clang include directories as compiler flags.
 */
inline auto
prep_code(corct::str_t_cr code)
{
  corct::vec_str args = {"-std=c++14", "-nostdinc++", corct::clang_inc_dir1,
                         corct::clang_inc_dir2};
  ASTUPtr ast(clang::tooling::buildASTFromCodeWithArgs(code,args));
  clang::ASTContext * pctx = &(ast->getASTContext());
  clang::TranslationUnitDecl * decl = pctx->getTranslationUnitDecl();
  return std::make_tuple(std::move(ast), pctx, decl);
}


#endif // include guard


// End of file
