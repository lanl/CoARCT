// global_matchers_test.cc
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "callsite_lister.h"
#include "prep_code.h"
#include "gtest/gtest.h"
#include <tuple>

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
  auto ms(tst.matchers());
  finder_t finder;
  for(auto & m : ms) {
    finder.addMatcher(m, &tst);
  }
  finder.matchAST(*pctx);
  return tst.m_num_calls;
}

TEST(callsite_matcher,case1_BasicHit){
  string_t code =
      "#include <iostream>\n"
      "void h(){return;}\n"
      "\n"
      "void i(){return h();}\n"
      "";
  vec_str targets = {"h"};
  callsite_counter csl(targets);
  uint32_t n_matches = run_case(code,csl);
  EXPECT_EQ(n_matches,1u);
}


// End of file
