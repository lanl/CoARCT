// function_sig_matchers_test.cc
// Feb 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "function_sig_matchers.h"
#include "prep_code.h"
#include "gtest/gtest.h"
// #include "tuple_utils.h"

using namespace corct;
using namespace clang;
using namespace clang::ast_matchers;

// STRINGIFY_TYPE(double);
// STRINGIFY_TYPE(int);

double g(int x, double y){return (double)x+y;}

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

TEST(function_sig_matchers,instantiate){
  // Print_Params<double,int> pp;
  // pp();
  // Print_ParamsN<2,double,int> ppn;
  // ppn();
  // Function_Sig<decltype(&f)> fs;
  // auto m = fs.ret_type_matcher();

  // std::tuple<int,string_t,double> a(42,"foo",3.14);
  // corct::for_each_in_tuple(a,functor());
  // corct::DMVec dms;
  // Gen_Params<2,2,int,double> gp;
  // gp(dms);
  // EXPECT_EQ(2u,dms.size());
}

TEST(function_sig_matchers,allOf){
  using namespace corct;
  using namespace clang::ast_matchers;
  // auto m =
  //   allOf(
  //     param_matcher<0,int>(),
  //     param_matcher<1,double>()
  //   );
  auto t = mk_tuple_fs<int,double>();
  auto t1 = std::make_tuple(0,1);
  EXPECT_EQ(t1,t);
  // params_again<MType,MType> pa = {mk_params<int,double>()};
  using TOM = std::tuple<MType,MType>;
  // params_again<TOM> pa(mk_params<int,double>());
  // auto m = pa.allOfParams();
  using ftype = decltype(&g);
  printf("%s:%i \n",__FUNCTION__,__LINE__);
  // auto arg_matcher = Function_Sig<ftype>::params_matcher();
  printf("%s:%i \n",__FUNCTION__,__LINE__);
  auto fmatcher = Function_Sig<ftype>::func_sig_matcher();
  printf("%s:%i \n",__FUNCTION__,__LINE__);
}
// End of file
