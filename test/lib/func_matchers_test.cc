// func_matchers_test.cc
// T. M. Kelley
// Feb 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "callsite_expander.h"
#include "prep_code.h"
#include "gtest/gtest.h"

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

/** \brief Run a test case,

\tparam Tester: subclass of finder_t; has matcher() method.
\param code to match
\param tst: Test object
\return number of matchers*/
template <typename Tester>
inline uint32_t
run_case(str_t_cr code, Tester & tst)
{
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  // decl->dump(); // uncomment for debugging
  auto m(tst.matcher());
  finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.matched_;
}

template <typename Match_Maker_t>
struct Tests_fn_call_matcher : public callback_t {
  auto matcher() const {
    return mk_matcher_(cs_bind_name_, fn_bind_name_, targ_name_);
  }

  void run(result_t const & result) override
  {
    CallExpr * call_site =
        const_cast<CallExpr *>(result.Nodes.getNodeAs<CallExpr>(cs_bind_name_));
    FunctionDecl const * fdecl =
        result.Nodes.getNodeAs<FunctionDecl>(fn_bind_name_);
    if(fdecl && call_site)
      matched_++;
    else {
      check_ptr(fdecl, "fdecl");
      check_ptr(call_site, "call_site");
    }
    return;
  }  // run

  explicit Tests_fn_call_matcher(Match_Maker_t & mkr)
      : mk_matcher_(mkr)//, cs_bind_name_(""), matched_(0)
  {}

  Match_Maker_t & mk_matcher_;
  string_t cs_bind_name_ = "";
  string_t fn_bind_name_ = "";
  string_t targ_name_ = "";
  uint32_t matched_ = 0;
};  // Tests_fn_call_matcher

using Testr1 = Tests_fn_call_matcher<decltype(mk_fn_call_matcher)>;

TEST(fn_call_matcher, case1_BasicHit)
{
  string_t const code = "void f(){return;} void g(){f(); return;}";
  Testr1 tst(mk_fn_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "f";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(exp_matches,run_case<Testr1>(code, tst));
}
TEST(fn_call_matcher, case2_BasicMiss)
{
  string_t const code = "void f(){return;} void g(){f(); return;}";
  Testr1 tst(mk_fn_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "g";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(exp_matches,run_case<Testr1>(code, tst));
}
TEST(fn_call_matcher, case3_CalledInStructMethod)
{
  string_t const code =
    "void f(){return;} void g(){f(); return;} struct S{  void h(){return;}  void i(){g();}};void k(S & s){ s.h(); return;}";
  Testr1 tst(mk_fn_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "g";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(exp_matches,run_case<Testr1>(code, tst));
}
TEST(fn_call_matcher, case4_MultipleCalls)
{
  string_t const code =
    "void f(){return;} void g(){f(); return;} struct S{  void h(){return;}  void i(){f();}};void k(S & s){ s.h(); return;}";
  Testr1 tst(mk_fn_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "f";
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(exp_matches,run_case<Testr1>(code, tst));
}

template <typename Match_Maker_t>
struct Tests_mthd_call_matcher : public callback_t {
  auto matcher() const {
    return mk_matcher_(cs_bind_name_, fn_bind_name_, targ_name_);
  }

  void run(result_t const & result) override
  {
    CXXMemberCallExpr * call_site = const_cast<CXXMemberCallExpr *>(
        result.Nodes.getNodeAs<CXXMemberCallExpr>(cs_bind_name_));
    CXXMethodDecl const * fdecl =
        result.Nodes.getNodeAs<CXXMethodDecl>(fn_bind_name_);
    if(fdecl && call_site)
      matched_++;
    else {
      HERE("Wha??");
      check_ptr(fdecl, "fdecl");
      check_ptr(call_site, "call_site");
    }
    return;
  }  // run

  explicit Tests_mthd_call_matcher(Match_Maker_t & mkr)
      : mk_matcher_(mkr)//, cs_bind_name_(""), matched_(0)
  {}

  Match_Maker_t & mk_matcher_;
  string_t cs_bind_name_ = "";
  string_t fn_bind_name_ = "";
  string_t targ_name_ = "";
  uint32_t matched_ = 0;
};  // Tests_mthd_call_matcher

using Testr2 = Tests_mthd_call_matcher<decltype(mk_mthd_call_matcher)>;

TEST(mthd_call_matcher, case1_BasicHit)
{
  string_t const code =
    "struct S{  void h(){return;}  void i(){this->h();}};void k(S & s){ s.h(); return;}";
  Testr2 tst(mk_mthd_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "h";
  uint32_t const exp_matches = 2u;
  EXPECT_EQ(exp_matches,run_case<Testr2>(code, tst));
}
TEST(mthd_call_matcher, case2_BasicMiss)
{
  string_t const code =
    "struct S{  void h(){return;}  void i(){this->h();}};void k(S & s){ s.h(); return;}";
  Testr2 tst(mk_mthd_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "k";
  uint32_t const exp_matches = 0u;
  EXPECT_EQ(exp_matches,run_case<Testr2>(code, tst));
}
TEST(mthd_call_matcher, case3_ptrHit)
{
  string_t const code =
    "struct S{  void h(){return;}  void i(){return;}};void m(S * ps){ ps->h();}";
  Testr2 tst(mk_mthd_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "h";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(exp_matches,run_case<Testr2>(code, tst));
}
TEST(mthd_call_matcher, case4_objectHit)
{
  string_t const code =
    "struct S{  void h(){return;}  void i(){return;}};void n(S s){ s.i();}";
  Testr2 tst(mk_mthd_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "i";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(exp_matches,run_case<Testr2>(code, tst));
}


// End of file
