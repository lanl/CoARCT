// struct_field_users_test.cc
// T. M. Kelley
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "struct_field_user.h"
#include "gtest/gtest.h"
#include <tuple>

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

using ASTUPtr = std::unique_ptr<ASTUnit>;

/**\brief Compile code fragment to AST.
\param code: valid c++ code
\return {unique_ptr<AST>,ASTContext *, TranslationDecl *}
*/
inline auto
prep_code(str_t_cr code)
{
  ASTUPtr ast(clang::tooling::buildASTFromCode(code));
  ASTContext * pctx = &(ast->getASTContext());
  TranslationUnitDecl * decl = pctx->getTranslationUnitDecl();
  return std::make_tuple(std::move(ast), pctx, decl);
}

inline uint32_t
run_case(str_t_cr code, struct_field_user & tst){
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  // decl->dump(); // uncomment for debugging
  auto ms(tst.matchers());
  finder_t finder;
  for(auto & m : ms){
    finder.addMatcher(m, &tst);
  }
  finder.matchAST(*pctx);
  return tst.n_matches_;
}

TEST(struct_field_user,instantiate){
  vec_str ts = {"foo_t","bar_t"};
  struct_field_user sfu(ts);
  EXPECT_EQ(sfu.n_matches_,0u);
}

TEST(struct_field_user,case1){
  string_t const code =
      "struct foo_t{int i;};struct bar_t{double k;};void f1(foo_t & f){f.i = "
      "16;}void f2(bar_t & b){b.k = 3.14;}";
  vec_str ts = {"foo_t","bar_t"};
  struct_field_user sfu(ts);
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(exp_matches,run_case(code, sfu));
  EXPECT_EQ(0u,sfu.non_lhs_uses_["foo_t"]["f1"].size());
  EXPECT_EQ(0u,sfu.non_lhs_uses_["bar_t"]["f2"].size());
  EXPECT_EQ(1u,sfu.lhs_uses_["foo_t"]["f1"].size());
  EXPECT_EQ(1u,sfu.lhs_uses_["bar_t"]["f2"].size());
}

// End of file
