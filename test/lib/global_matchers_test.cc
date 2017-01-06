// global_matchers_test.cc
// T. M. Kelley
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved
#include "global_matchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "gtest/gtest.h"
#include "utilities.h"
#include <tuple>

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

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

template <typename Match_Maker_t>
struct Tests_global : public callback_t {
  auto matcher() const { return mk_matcher_(bd_var_name_); }

  void run(result_t const& result) override {
    FunctionDecl const * fdecl = result.Nodes.getNodeAs<FunctionDecl>("function");
    VarDecl const * var = result.Nodes.getNodeAs<VarDecl>("varName");
    Expr const * g_var = result.Nodes.getNodeAs<Expr>("globalReference");
    if(fdecl && g_var && var) matched_++;
    else{
      check_ptr(fdecl,"fdecl");
      check_ptr(g_var,"g_var");
      check_ptr(var,"var");
    }
    return;
  }  // run

  explicit Tests_global(Match_Maker_t & mkr)
  : mk_matcher_(mkr),bd_var_name_(""),matched_(0) {}

  Match_Maker_t & mk_matcher_;
  string_t bd_var_name_ = "";
  uint32_t matched_ = 0;
}; // Tests_ptr_ref


/** \brief Run a test case,

\tparam Tester: subclass of finder_t; has matcher() method.
\param code to match
\param tst: Test object
\return number of matchers*/
template <typename Tester>
inline uint32_t
run_case(str_t_cr code, Tester & tst){
  ASTUPtr ast; ASTContext* pctx; TranslationUnitDecl* decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  // decl->dump(); // uncomment for debugging
  auto m(tst.matcher());
  finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.matched_;
}

using Testr1 = Tests_global<decltype(mk_global_var_matcher)>;
TEST(global_var_matcher,case1_BasicHit){
  string_t const code = "int global_i;void f(){  global_i = 1;}";
  Testr1 tst(mk_global_var_matcher);
  tst.bd_var_name_ = "global_i";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case<Testr1>(code,tst),exp_matches);
}
TEST(global_var_matcher,case2_BasicMiss){
  string_t const code = "int global_i;void f(){  global_i = 1;}";
  Testr1 tst(mk_global_var_matcher);
  tst.bd_var_name_ = "global_j";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case<Testr1>(code,tst),exp_matches);
}
TEST(global_var_matcher,case3_MissLocal){
  string_t const code = "int global_i;void f(){  int global_i; global_i = 1;}";
  Testr1 tst(mk_global_var_matcher);
  tst.bd_var_name_ = "global_i";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case<Testr1>(code,tst),exp_matches);
}
// not specifying any name
TEST(global_var_matcher,case4_BasicHitAll){
  string_t const code =
      "int global_i; double g_f;void f(){  global_i = 1;g_f = 3.14;}";
  Testr1 tst(mk_global_var_matcher);
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(run_case<Testr1>(code,tst),exp_matches);
}
TEST(global_var_matcher,case5_HitsOneLocalAll){
  string_t const code =
      "int global_i;double g_f;void f(){  int global_i; global_i = 1;g_f = 3.14;}";
  Testr1 tst(mk_global_var_matcher);
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case<Testr1>(code,tst),exp_matches);
}

using Testr2 = Tests_global<decltype(mk_global_fn_matcher)>;
TEST(global_fn_matcher,case1_BasicHit){
  string_t const code = "int global_i;void f(){  global_i = 1;}";
  Testr2 tst(mk_global_fn_matcher);
  tst.bd_var_name_ = "global_i";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case<Testr2>(code,tst),exp_matches);
}
TEST(global_fn_matcher,case2_BasicMiss){
  string_t const code = "int global_i;void f(){  global_i = 1;}";
  Testr2 tst(mk_global_fn_matcher);
  tst.bd_var_name_ = "global_j";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case<Testr2>(code,tst),exp_matches);
}
TEST(global_fn_matcher,case3_MissLocal){
  string_t const code = "int global_i;void f(){  int global_i; global_i = 1;}";
  Testr2 tst(mk_global_fn_matcher);
  tst.bd_var_name_ = "global_i";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(run_case<Testr2>(code,tst),exp_matches);
}
// not specifying any name
TEST(global_fn_matcher,case4_BasicHitAll){
  string_t const code =
      "int global_i; double g_f;void f(){  global_i = 1;g_f = 3.14;}";
  Testr2 tst(mk_global_fn_matcher);
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case<Testr2>(code,tst),exp_matches);
}
TEST(global_fn_matcher,case5_HitsOneLocalAll){
  string_t const code =
      "int global_i;double g_f;void f(){  int global_i; global_i = 1;g_f = 3.14;}";
  Testr2 tst(mk_global_fn_matcher);
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(run_case<Testr2>(code,tst),exp_matches);
}


// End of file
