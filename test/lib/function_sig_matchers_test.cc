// function_sig_matchers_test.cc
// Feb 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "function_sig_matchers.h"
#include "prep_code.h"
#include "gtest/gtest.h"

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

double g(int x, double y){return (double)x+y;}
double h(int x, double& y){return (double)x+y;}
double k(int x, const double & y){return (double)x+y;}
double m(volatile int x, const volatile double y){return double();}

struct functor{
  template <typename T>
  void operator()(T&&t){
    std::cout << t << "\n";
  }
}; // functor!

TEST(function_sig_matchers,type_as_string){
  EXPECT_EQ("double",corct::type_as_string<double>());
  EXPECT_EQ("functor",corct::type_as_string<functor>());
}

TEST(function_sig_matchers,param_matcher_reference){
  double d1,&d(d1);
  auto m = param_matcher<0,decltype(d)>();
}
TEST(function_sig_matchers,check_is_const){
  double const y = 1.0;
  EXPECT_TRUE(std::is_const<decltype(y)>::value);
}

struct fsig_matcher_test : public callback_t{
  template <class F>
  auto matcher(){
    return Function_Sig<F>::func_sig_matcher();
  }

  void run(result_t const & result) override {
    FunctionDecl const * pfdecl = result.Nodes.getNodeAs<FunctionDecl>("fdecl");
    SourceManager & sm(result.Context->getSourceManager());
    if(pfdecl){
      matched_++;
      std::cout << pfdecl->getNameAsString() << " declared at "
        << sourceRangeAsString(pfdecl->getSourceRange(),&sm)
        << " declares parameters:\n";
      FunctionDecl::param_const_iterator pit;
      for(pit = pfdecl->param_begin();pit!=pfdecl->param_end();++pit){
        ParmVarDecl const *p_prm = *pit;
        QualType qt = p_prm->getOriginalType();
        string_t tname = qt.getAsString();
        std::cout
          << " function scope depth: " << p_prm->getFunctionScopeDepth()
          << ", function scope index: " << p_prm->getFunctionScopeIndex()
          << ", name '" << p_prm->getNameAsString() << "', type name '"
          << tname << "'" << std::endl;
      }
    }
    else{
      check_ptr(pfdecl,"pfdecl");
    }
    return;
  } // run
  uint32_t matched_ = 0;
}; // fsig_matcher_test

template <typename Tester, typename F>
inline uint32_t
run_case(str_t_cr code, Tester & tst)
{
  ASTUPtr ast; ASTContext * pctx; TranslationUnitDecl * decl;
  std::tie(ast, pctx, decl) = prep_code(code);
  auto m(tst.template matcher<F>());
  finder_t finder;
  finder.addMatcher(m, &tst);
  finder.matchAST(*pctx);
  return tst.matched_;
}

TEST(function_sig_matcher,case1_basicHit){
  using ftype = decltype(&g);
  string_t code = "double g(int x, double y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(1u,tester.matched_);
}

TEST(function_sig_matcher,case2_basicHit){
  using ftype = decltype(&g);
  string_t code = "double h(int x, double y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(1u,tester.matched_);
}

TEST(function_sig_matcher,case3_basicMiss){
  using ftype = decltype(&g);
  string_t code = "double h(int x, int y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(0u,tester.matched_);
}

TEST(function_sig_matcher,case4_Hit_Reference){
  using ftype = decltype(&h);
  string_t code = "double h(int x, double& y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(1u,tester.matched_);
}
/*double k(int x, const double & y){return (double)x+y;}  */
TEST(function_sig_matcher,case5_Hit_ConstRef){
  using ftype = decltype(&k);
  string_t code = "double h(int x, const double & y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(1u,tester.matched_);
  // Function_Sig<ftype>::vec_type_info tis =
  //     Function_Sig<ftype>().param_type_strings();
  // for(auto & s:tis){
  //   printf("%s:%i type: '%s'\n",__FUNCTION__,__LINE__,s.to_string().c_str());
  // }
}

TEST(function_sig_matcher,case6_Hit_ConstRef){
  using ftype = decltype(&k);
  string_t code = "double h(int x, double & y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(0u,tester.matched_);
}
/*double m(volatile int x, const volatile double y){return double();} */
TEST(function_sig_matcher,case7_Hit_Volatile){
  using ftype = decltype(&m);
  string_t code = "double mnm(volatile int x, const volatile double y){return (double)x+y;}";
  fsig_matcher_test tester;
  run_case<fsig_matcher_test,ftype>(code,tester);
  EXPECT_EQ(0u,tester.matched_);
}

// End of file
