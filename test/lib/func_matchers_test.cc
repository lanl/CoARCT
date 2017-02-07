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
TEST(mthd_call_matcher, case5_distinguishesBoundVsFree)
{
  string_t const code =
    "void h(){return;}void i(){return h();}struct S{  void h(){return;}  void i(){ return;}};void k(S & s){ s.h(); return;}";
  Testr2 tst(mk_mthd_call_matcher);
  tst.cs_bind_name_ = "cs";
  tst.fn_bind_name_ = "fn";
  tst.targ_name_ = "h";
  uint32_t const exp_matches = 1u;
  EXPECT_EQ(exp_matches,run_case<Testr2>(code, tst));
}

// Now test the full expand_callsite class
TEST(expand_callsite,case1_expands)
{
  string_t const code = "void f(int){return;} void g(){int i(42);f(i); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  string_t new_arg("boo");
  expand_callsite ec(reps,targs,new_arg,false);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.matchAST(*pctx);
  EXPECT_EQ(1u,reps.size());
  replacement_t const & rep1(*reps.begin());
  EXPECT_EQ("input.cc",rep1.getFilePath());
  EXPECT_EQ(43u,rep1.getOffset());
  EXPECT_EQ(0u,rep1.getLength());
  EXPECT_EQ(", boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
// case 2: no parameters in function
TEST(expand_callsite,case2_expands)
{
  string_t const code = "void f(){return;} void g(){f(); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  string_t new_arg("boo");
  expand_callsite ec(reps,targs,new_arg,false);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.matchAST(*pctx);
  EXPECT_EQ(1u,reps.size());
  replacement_t const & rep1(*reps.begin());
  EXPECT_EQ("input.cc",rep1.getFilePath());
  EXPECT_EQ(29u,rep1.getOffset());
  EXPECT_EQ(0u,rep1.getLength());
  EXPECT_EQ("boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
// case 3: has a defaulted parameter in first spot (un-overridden in call)
TEST(expand_callsite,case3_expands)
{
  string_t const code = "void f(int i = 42){return;} void g(){f(); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  string_t new_arg("boo");
  bool const verbose(false);
  expand_callsite ec(reps,targs,new_arg,false,verbose);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.matchAST(*pctx);
  EXPECT_EQ(1u,reps.size());
  replacement_t const & rep1(*reps.begin());
  EXPECT_EQ("input.cc",rep1.getFilePath());
  EXPECT_EQ(38u,rep1.getOffset());
  EXPECT_EQ(0u,rep1.getLength());
  EXPECT_EQ("boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
// case 4: has a defaulted parameter in first spot (overridden in call)
TEST(expand_callsite,case4_expands)
{
  string_t const code = "void f(int i = 42){return;} void g(){int i(43);f(i); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  string_t new_arg("boo");
  bool const verbose(false);
  expand_callsite ec(reps,targs,new_arg,false,verbose);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.matchAST(*pctx);
  EXPECT_EQ(1u,reps.size());
  replacement_t const & rep1(*reps.begin());
  EXPECT_EQ("input.cc",rep1.getFilePath());
  EXPECT_EQ(48u,rep1.getOffset());
  EXPECT_EQ(0u,rep1.getLength());
  EXPECT_EQ("boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
// case 5: defaulted parameter after first spot (not overridden in call)
TEST(expand_callsite,case5_expands)
{
  string_t const code = "void f(double d, int i = 42){return;} void g(){f(3.14159); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  string_t new_arg("boo");
  bool const verbose(false);
  expand_callsite ec(reps,targs,new_arg,false,verbose);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.matchAST(*pctx);
  EXPECT_EQ(1u,reps.size());
  replacement_t const & rep1(*reps.begin());
  EXPECT_EQ("input.cc",rep1.getFilePath());
  EXPECT_EQ(56u,rep1.getOffset());
  EXPECT_EQ(0u,rep1.getLength());
  EXPECT_EQ(", boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
TEST(expand_callsite,case6_method_expands)
{
  using EC = expand_callsite;
  string_t const code =
    "void f(){return;} void g(){f(); return;} struct S{  void h(){return;}  void i(){g();}};void k(S & s){ s.h(); return;}";
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  EC::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str ftargs = {"f","g","h"};
  string_t new_arg("boo");
  EC ec(reps,ftargs,new_arg,false);
  auto ms(ec.matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }
  finder.addMatcher(
      mk_mthd_call_matcher(EC::cs_bind_name_, EC::fn_bind_name_, "h"), &ec);
  finder.matchAST(*pctx);
  replacement_t er1("input.cc",29,0,"boo");
  replacement_t er2("input.cc",82,0,"boo");
  replacement_t er3("input.cc",106,0,"boo");
  EC::Base::replacements_t exp_reps = {er1,er2,er3};
  EXPECT_EQ(exp_reps.size(),reps.size());
  using it_t = expand_callsite::Base::replacements_t::iterator;
  it_t ri = reps.begin(),ei = exp_reps.begin();
  for(; ri != reps.end() && ei != exp_reps.end(); ++ri,++ei){
    bool const rep_ok = *ri == *ei;
    if(!rep_ok){
      std::cout << "rep: " << ri->toString() << ", expected: " << ei->toString()
        << std::endl;
    }
    EXPECT_TRUE(rep_ok);
  }
} // TEST(expand_callsite,expands)


// End of file
