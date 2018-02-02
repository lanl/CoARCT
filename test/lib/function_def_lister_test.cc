// global_matchers_test.cc
// Jan 06, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "function_definition_lister.h"
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
  auto m(tst.matcher());
  finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.m_num_funcs;
}

TEST(func_decl_matcher,case1_BasicHit){
  string_t code =
      "#include <iostream>\n"
      "void f(){}\n"
      "class C{\n"
      "  public: \n"
      "  double \n"
            "c(){ return 2.0;}\n"
      "};\n"
      "int g(int, C const &){\n"
      "  /* comment */\n"
            "return 42;\n"
      "}";
  FunctionDefLister fdp("f_decl");
  uint32_t n_matches = run_case(code,fdp);
  EXPECT_EQ(n_matches,3u);
}


// End of file
