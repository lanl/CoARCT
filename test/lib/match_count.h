// match_counter.h
// Nov 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved


#pragma once

#include "prep_code.h"
#include "types.h"

namespace corct_test{

/** \brief Run a test case,

\tparam Tester: subclass of finder_t; has matcher() method.
\param code to match
\param tst: Test object
\return number of matches found */
template <typename Tester>
inline uint32_t
count_matches(corct::str_t_cr code, Tester & tst){
  using namespace clang;
  ASTUPtr ast; ASTContext* pctx; TranslationUnitDecl* decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  // decl->dump(); // uncomment for debugging
  auto m(tst.matcher());
  corct::finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.matched_;
}

} // corct_test

// End of file
