// clang_utilities_test.cc
// T. M. Kelley
// Jan 03, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Tests of utilities that build on clang */
#include "utilities.h"
#include "gtest/gtest.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Frontend/FrontendActions.h"
#include <tuple>

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

/* Test strategy: write code including element to be tested, such as member
expression; compile to AST; use matcher to extract interesting node; apply
function to interesting node. It's a lot of work to get around constructing a
consistent AST subtree directly.
*/

using ASTUPtr = std::unique_ptr<ASTUnit>;

/**\brief Compile code fragment to AST.
\param code: valid c++ code
\return {unique_ptr<AST>,ASTContext *, TranslationDecl *}
*/
inline auto prep_code(str_t_cr code) {
  ASTUPtr ast(clang::tooling::buildASTFromCode(code));
  ASTContext* pctx = &(ast->getASTContext());
  TranslationUnitDecl* decl = pctx->getTranslationUnitDecl();
  return std::make_tuple(std::move(ast), pctx, decl);
}

// is_on_lhs:
struct Tests_On_LHS : public callback_t {
  void run(result_t const& result) override {
    MemberExpr const* mexpr = result.Nodes.getNodeAs<MemberExpr>("mexpr");
    ASTContext* pctx = result.Context;
    if (mexpr) {
      actually_on_lhs_ = is_on_lhs(mexpr, *pctx);
    }
    return;
  }  // run
  bool actually_on_lhs_ = false;
};

bool run_case_is_on_lhs(str_t_cr code) {
  ASTUPtr ast; ASTContext* pctx; TranslationUnitDecl* decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  Tests_On_LHS tol;
  finder_t finder;
  DeclarationMatcher m(hasDescendant(memberExpr().bind("mexpr")));
  finder.addMatcher(m, &tol);
  finder.match(*decl, *pctx);
  return tol.actually_on_lhs_;
}  // run_case_is_on_lhs

TEST(utilities, on_lhs_case1) {
  // some code that includes exactly one member reference, and on the LHS
  string_t const code = "struct S{  int i;};void f(){  S s = {1};  s.i = 2;}";
  bool on_lhs = run_case_is_on_lhs(code);
  EXPECT_TRUE(on_lhs);
}  // TEST(utilities,on_lhs){

TEST(utilities, on_lhs_case2) {
  // some code that includes exactly one member reference, but on the RHS
  string_t const code = "struct S{  int i;};void f(){  S s = {1};int i = s.i;}";
  bool on_lhs = run_case_is_on_lhs(code);
  EXPECT_FALSE(on_lhs);
}  // TEST(utilities,on_lhs){

// is_part_of_assignment:
struct Tests_PartOfAssgnmt : public callback_t {
  void run(result_t const& result) override {
    MemberExpr const* mexpr = result.Nodes.getNodeAs<MemberExpr>("mexpr");
    ASTContext* pctx = result.Context;
    if (mexpr) {
      actually_part_ = is_part_of_assignment(mexpr, *pctx);
    }
    return;
  }  // run
  bool actually_part_ = false;
};

bool run_case_part_of_assignment(str_t_cr code) {
  ASTUPtr ast;
  ASTContext* pctx;
  TranslationUnitDecl* decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  Tests_PartOfAssgnmt tpoa;
  finder_t finder;
  DeclarationMatcher m(hasDescendant(memberExpr().bind("mexpr")));
  finder.addMatcher(m, &tpoa);
  finder.match(*decl, *pctx);
  return tpoa.actually_part_;
}

TEST(utilities, is_part_of_assignment_case1) {
  // code includes exactly one member reference, and is part of assignment
  string_t const code =
      "struct S{ int i;};void g(int){return;}void f(){S s;s.i = 3;}";
  bool is_part(run_case_part_of_assignment(code));
  EXPECT_TRUE(is_part);
}

TEST(utilities, is_part_of_assignment_case2) {
  // code includes exactly one member reference, and is not part of assignment
  string_t const code =
      "struct S{ int i;};void g(int){return;}void f(){ S s; g(s.i);}";
  bool is_part(run_case_part_of_assignment(code));
  EXPECT_FALSE(is_part);
}

// End of file
