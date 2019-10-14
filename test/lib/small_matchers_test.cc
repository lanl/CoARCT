// test_small_matchers.cc
// Jan 05, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "small_matchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "gtest/gtest.h"
#include "prep_code.h"
#include "utilities.h"
#include <tuple>

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

/* Test strategy: write code including element to be tested, such as member
expression; compile to AST; count number of times matcher hits nodes. It's a lot
of work to get around constructing a consistent AST subtree directly.
*/

struct Tests_ptr_ref : public callback_t {
  using matcher_t = StatementMatcher;

  matcher_t matcher() const
  {
    return mk_ptr_matcher(var_bd_name, ref_bd_name, ptr_bd_name);
  }

  void run(result_t const & result) override
  {
    DeclRefExpr const * dexpr =
        result.Nodes.getNodeAs<DeclRefExpr>(ref_bd_name);
    VarDecl const * vdecl = result.Nodes.getNodeAs<VarDecl>(var_bd_name);
    if(dexpr && vdecl)
      matched_++;
    else {
      check_ptr(dexpr, "dexpr");
      check_ptr(vdecl, "vdecl");
    }
    return;
  }  // run

  string_t const var_bd_name = "var";
  string_t const ref_bd_name = "ref";
  string_t ptr_bd_name = "";
  uint32_t matched_ = 0;
};  // Tests_ptr_ref

/** \brief Run a test case,

\tparam Tester: subclass of finder_t; has matcher() method.
\param code to match
\param tst: Test object
\return number of matchers*/
template <typename Tester>
inline uint32_t
run_case_ptr_match(str_t_cr code, Tester & tst)
{
  ASTUPtr ast;
  ASTContext * pctx;
  TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  // decl->dump(); // uncomment for debugging
  auto m(tst.matcher());
  finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.matched_;
}

TEST(mk_ptr_matcher, case1)
{
  string_t code =
      "struct S{  S * next;};void f(S*p){  while(p != p->next){    p = "
      "p->next;  }}";
  Tests_ptr_ref tpr;
  uint32_t const exp_matches = 4u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

TEST(mk_ptr_matcher, case2)
{
  string_t code = "struct S1{  S1 * next;}; void f1(S1*p){  p = 0;}";
  Tests_ptr_ref tpr;
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

TEST(mk_ptr_matcher, case3)
{
  string_t code = "struct S1{  S1 * next;}; void f1(S1*p){  p = 0;}";
  Tests_ptr_ref tpr;
  tpr.ptr_bd_name = "p";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

TEST(mk_ptr_matcher, case4)
{
  string_t code = "struct S1{  S1 * next;}; void f1(S1*p){  p = 0;}";
  Tests_ptr_ref tpr;
  tpr.ptr_bd_name = "q";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

TEST(mk_ptr_matcher, case5)
{
  string_t code = "struct S1{  S1 * next;}; void f1(S1*q){  q = 0;}";
  Tests_ptr_ref tpr;
  tpr.ptr_bd_name = "p";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

struct Tests_arrow_match : public callback_t {
  auto matcher() const
  {
    return match_arrow_next(var_bd_name, ref_bd_name, ptr_bd_name);
  }

  void run(result_t const & result) override
  {
    DeclRefExpr const * dexpr =
        result.Nodes.getNodeAs<DeclRefExpr>(ref_bd_name);
    VarDecl const * vdecl = result.Nodes.getNodeAs<VarDecl>(var_bd_name);
    if(dexpr && vdecl)
      matched_++;
    else {
      check_ptr(dexpr, "dexpr");
      check_ptr(vdecl, "vdecl");
    }
    return;
  }  // run

  string_t const var_bd_name = "var";
  string_t const ref_bd_name = "ref";
  string_t ptr_bd_name = "";
  uint32_t matched_ = 0;
};  // Tests_arrow_match

TEST(arrow_matcher, case1_BasicHit)
{
  string_t code =
      "struct S{  S * next;};void f(S*p){  while(p != p->next){    p = "
      "p->next;  }}";
  Tests_arrow_match tpr;
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(run_case_ptr_match(code, tpr), exp_matches);
}

TEST(arrow_matcher, case2_HitsVarName)
{
  string_t code =
      "struct S{  S * next;};void f(S*p){  while(p != p->next){    p = "
      "p->next;  }}";
  Tests_arrow_match tam;
  tam.ptr_bd_name = "p";
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(run_case_ptr_match(code, tam), exp_matches);
}

TEST(arrow_matcher, case3_MissesOtherVarName)
{
  string_t code =
      "struct S{  S * next;};void f(S*p){  while(p != p->next){    p = "
      "p->next;  }}";
  Tests_arrow_match tam;
  tam.ptr_bd_name = "q";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case_ptr_match(code, tam), exp_matches);
}

TEST(arrow_matcher, case4_MissesNonArrowRefs)
{
  string_t code = "struct S1{  S1 * next;}; void f1(S1*p){  p = 0;}";
  Tests_arrow_match tam;
  tam.ptr_bd_name = "p";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case_ptr_match(code, tam), exp_matches);
}

TEST(count_public, public2)
{
  string_t code = "class A{public: int a; public: int b;};";
  ASTUPtr ast;
  ASTContext * pctx;
  TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  finder_t finder;
  count_public cp;
  auto m = cp.matcher();
  finder.addMatcher(m, &cp);
  finder.matchAST(*pctx);
  count_public::map_t & map(cp.public_count_);
  EXPECT_EQ(1u, map.size());
  EXPECT_EQ(2u, map["A"]);
}

// End of file
