// callsite_expander_test.cc
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

string_t const fname = "input.cc";
string_t const cboo = ", boo";
string_t const boo = "boo";
string_t const new_arg = "boo";

using EC = expand_callsite;
using replacements_t = EC::Base::replacements_t;
using reps_it = replacements_t::iterator;

template <typename Tester>
bool run_case_ec(string_t const & code, Tester & t, replacements_t const & reps_exp){
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  auto ms(t.fn_matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &t);
  }
  finder.matchAST(*pctx);
  replacements_t const & reps(t.get_replacements());
  bool const size_ok(reps_exp.size() == reps.size());
  EXPECT_EQ(reps_exp.size(),reps.size());
  bool const reps_match = reps_exp == reps;
  if(!reps_match){
    std::cout << "reps disagree:\n";
    reps_it ri = reps.begin(), ei = reps_exp.begin();
    for(; ri != reps.end() && ei != reps_exp.end(); ++ri, ++ei){
      std::cout << "expected: " << ei->toString() << ", actual: "
        << ri->toString()<< "\n";
    }
  }
  EXPECT_EQ(reps_exp,reps);
  return size_ok && reps_match;
} // run_case_ec

TEST(expand_callsite,case1_expands)
{
  string_t const code = "void f(int){return;} void g(){int i(42);f(i); return;}";
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  expand_callsite ec(reps,targs,new_arg,false);
  replacements_t exp_r = {{fname,43u,0u,cboo}};
  run_case_ec<EC>(code,ec,exp_r);
}

// case 2: no parameters in function
TEST(expand_callsite,case2_expands)
{
  string_t const code = "void f(){return;} void g(){f(); return;}";
  // ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  // std::tie(ast, pctx, decl) = prep_code(code);
  replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  expand_callsite ec(reps,targs,new_arg,false);
  // auto ms(ec.matchers());
  // finder_t finder;
  // for(auto & m: ms){
  //   finder.addMatcher(m, &ec);
  // }
  // finder.matchAST(*pctx);
  // EXPECT_EQ(1u,reps.size());
  // replacements_t const & rep1(*reps.begin());
  replacements_t exp_reps = {{fname,29u,0u,boo}};
  run_case_ec(code,ec,exp_reps);
  // EXPECT_EQ("input.cc",rep1.getFilePath());
  // EXPECT_EQ(29u,rep1.getOffset());
  // EXPECT_EQ(0u,rep1.getLength());
  // EXPECT_EQ("boo",rep1.getReplacementText());
} // TEST(expand_callsite,expands)
// case 3: has a defaulted parameter in first spot (un-overridden in call)
TEST(expand_callsite,case3_expands)
{
  string_t const code = "void f(int i = 42){return;} void g(){f(); return;}";
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  expand_callsite ec(reps,targs,new_arg,false);
  replacements_t exp_reps = {{fname,38u,0u,boo}};
  run_case_ec(code,ec,exp_reps);
} // TEST(expand_callsite,expands)
// case 4: has a defaulted parameter in first spot (overridden in call)
TEST(expand_callsite,case4_expands)
{
  string_t const code = "void f(int i = 42){return;} void g(){int i(43);f(i); return;}";
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  expand_callsite ec(reps,targs,new_arg,false);
  replacements_t exp_reps = {{fname,48u,0u,boo}};
  run_case_ec(code,ec,exp_reps);
} // TEST(expand_callsite,expands)
// case 5: defaulted parameter after first spot (not overridden in call)
TEST(expand_callsite,case5_expands)
{
  string_t const code = "void f(double d, int i = 42){return;} void g(){f(3.14159); return;}";
  expand_callsite::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str targs = {"f"};
  expand_callsite ec(reps,targs,new_arg,false);
  replacements_t exp_reps = {{fname,56u,0u,cboo}};
  run_case_ec(code,ec,exp_reps);
} // TEST(expand_callsite,expands)
/* This test checks both function and methods matching using the
same Callback. This currently requires manually registering the method names.*/
TEST(expand_callsite,case6_method_expands)
{
  string_t const code =
    "void f(){return;} void g(){f(); return;} struct S{  void h(){return;}  void i(){g();}};void k(S & s){ s.h(); return;}";
  EC::Base::replacements_t reps; // in 3.9, this is std::set
  vec_str ftargs = {"f","g","h"};
  replacement_t er1(fname,29,0,boo);
  replacement_t er2(fname,82,0,boo);
  replacement_t er3(fname,106,0,boo);
  expand_callsite ec(reps,ftargs,new_arg,false);
  EC::Base::replacements_t exp_reps = {er1,er2,er3};

  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  auto ms(ec.fn_matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &ec);
  }

  string_t const mthd = "h";
  auto mthd_matcher =
    mk_mthd_call_matcher(EC::cs_bind_name_,EC::fn_bind_name_,mthd);
  finder.addMatcher(mthd_matcher,&ec);

  finder.matchAST(*pctx);
  EXPECT_EQ(exp_reps.size(),reps.size());
  EXPECT_EQ(exp_reps,reps);
} // TEST(expand_callsite,expands)


// End of file
