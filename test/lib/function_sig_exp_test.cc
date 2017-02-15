// function_sig_exp_test.cc
// Feb 08, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "function_signature_expander.h"
#include "prep_code.h"
#include "gtest/gtest.h"

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

using FSE = function_signature_expander;
using repls_t = FSE::replacements_t;
using reps_it = repls_t::iterator;

string_t const new_param = "int np";
string_t const cnp = ", int np";
string_t const npc = "int np, ";
string_t const fname = "input.cc";

template <typename Tester>
bool
run_case_fse(string_t const & code, Tester & t, repls_t const & reps_exp)
{
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  auto ms(t.fn_matchers());
  finder_t finder;
  for(auto & m: ms){
    finder.addMatcher(m, &t);
  }
  finder.matchAST(*pctx);
  repls_t const & reps(t.get_replacements());
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


TEST(function_sig_exp,instantiate){
  repls_t reps;
  vec_str ftargs = {"f","g"};
  bool const dry_run(false);
  FSE(reps,ftargs,new_param,dry_run);
  EXPECT_TRUE(true);
} // instantiate
// Case 1: functions with 0/1 parameters, no defaults
TEST(function_sig_exp,expands_case1){
  string_t const code =
    "void f(int){return;} void g(){int i(42);f(i); return;}";
  repls_t reps;
  vec_str ftargs = {"f","g"};
  bool const dry_run(false);
  FSE fse(reps,ftargs,new_param,dry_run);
  repls_t exp_repls = {{fname,10u,0u,cnp},{fname,28u,0u,new_param}};
  run_case_fse(code,fse,exp_repls);
}
// Case 2: no matches
TEST(function_sig_exp,expands_case2){
  string_t const code =
    "void f(int){return;} void g(){int i(42);f(i); return;}";
  repls_t reps;
  vec_str ftargs = {"p","q"};
  bool const dry_run(false);
  FSE fse(reps,ftargs,new_param,dry_run);
  repls_t exp_repls = {};
  run_case_fse(code,fse,exp_repls);
}
/* Case 3: functions with some defaults
    p(int i = 42) --> p(int np, int i = 42)
    q(int foo, double pi = 3.2) --> q(int foo, int np, double pi = 3.2)
*/
TEST(function_sig_exp,expands_case3){
  string_t const code =
    "void p(int i=42); void q(int foo, double pi = 3.2);";
  repls_t reps;
  vec_str ftargs = {"p","q"};
  bool const dry_run(false);
  FSE fse(reps,ftargs,new_param,dry_run);
  repls_t exp_repls = {{fname,7u,0u,npc},{fname,34u,0u,npc}};
  run_case_fse(code,fse,exp_repls);
}

// End of file
