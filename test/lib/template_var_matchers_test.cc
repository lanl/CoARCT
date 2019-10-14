// template_var_matchers_test.cc
// Jan 05, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/**\file Unit tests of template variable matchers*/

#include "template_var_matchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "gtest/gtest.h"
#include "match_count.h"
#include "prep_code.h"
// #include "utilities.h"
#include <vector>

using namespace corct;
using namespace corct_test;
using namespace clang;
using namespace clang::ast_matchers;

/* Test strategy: write code including element to be tested, such as member
expression; compile to AST; count number of times matcher hits nodes. It's a lot
of work to get around constructing a consistent AST subtree directly.
*/

struct Tests_templ_match : public callback_t {
  using matcher_t = DeclarationMatcher;

  matcher_t matcher() const
  {
    if(namespace_name != "") {
      return mk_templ_var_matcher(template_name, namespace_name);
    }
    return mk_templ_var_matcher(template_name);
  }

  void run(result_t const & result) override
  {
    matched_++;
    return;
  }  // run

  string_t template_name = "";
  string_t namespace_name = "";
  uint32_t matched_ = 0;
};  // Tests_ptr_ref

TEST(template_var_matchers, template_name)
{
  {
    string_t code = "template <class T> class Aardvarks{}; Aardvarks<int> a;";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 1u);
  }
  {
    string_t code = "template <class T> class Bardvarks{}; Bardvarks<int> a;";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 0u);
  }
  {
    string_t code =
        "template <class T> class Aardvarks{};"
        "Aardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> class Aardvarks{};"
        "  Aardvarks<double> a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 5u);
  }
}  // TEST(template_var_matchers,named_template)

TEST(template_var_matchers, namespaced_template)
{
  {
    string_t code =
        "template <class T> class Aardvarks{};"
        "Aardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> class Aardvarks{};"
        "  Aardvarks<double> a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    tester.namespace_name = "ns";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 1u);
  }
}  // TEST(template_var_matchers,namespaced_template)

TEST(template_var_matchers, typedefd_template)
{
  /* The matcher "sees through" typedefs: */
  {
    string_t code =
        "template <class T> class Aardvarks{};"
        "using Cardvarks = Aardvarks<int>;"
        "Cardvarks a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> class Aardvarks{};"
        "  Aardvarks<double> a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 5u);
  }
  /* Same thing. Note that his generates a C++11 warning. */
  {
    string_t code =
        "template <class T> class Aardvarks{};"
        "template <class U> using Cardvarks = Aardvarks<U>;"
        "Cardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> class Aardvarks{};"
        "  Aardvarks<double> a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 5u);
  }
  /* The matcher "sees through" typedefs, with namespaces: */
  {
    string_t code =
        "template <class T> class Aardvarks{};"
        "using Cardvarks = Aardvarks<int>;"
        "Cardvarks a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> class Aardvarks{};"
        "  using Cardvarks = Aardvarks<int>;"
        "  Cardvarks a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    tester.namespace_name = "ns";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 1u);
  }
  /* Same thing. Note that his generates a C++11 warning. */
  {
    string_t code =
        "template <class T> struct Aardvarks{};"
        "template <class U> using Cardvarks = Aardvarks<U>;"
        "Cardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> struct Aardvarks{};"
        "  template <class U> using Cardvarks = Aardvarks<U>;"
        "  Cardvarks<double> a;"
        "}";
    Tests_templ_match tester;
    tester.template_name = "Aardvarks";
    tester.namespace_name = "ns";
    uint32_t n_matches = count_matches<Tests_templ_match>(code, tester);
    EXPECT_EQ(n_matches, 1u);
  }

}  // TEST(template_var_matchers,typedefd_template){

TEST(template_var_matchers, template_var_reporter)
{
  using namespace clang;
  using vec_strs_t = template_var_reporter::vec_strs_t;
  {
    string_t code =
        "template <class T> struct Aardvarks{};"
        "template <class U> using Cardvarks = Aardvarks<U>;"
        "Cardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> struct Aardvarks{};"
        "  template <class U> using Cardvarks = Aardvarks<U>;"
        "  Cardvarks<double> a;"
        "}";
    template_var_reporter repo("Aardvarks");
    ASTUPtr ast;
    ASTContext * pctx;
    TranslationUnitDecl * decl;
    std::tie(ast, pctx, decl) = prep_code(code);
    template_var_reporter::matchers_t ms(repo.matchers());
    corct::finder_t finder;
    for(auto m : ms) { finder.addMatcher(m, &repo); }
    finder.matchAST(*pctx);
    auto & args(repo.args_);
    EXPECT_EQ(args.size(), 5);
    vec_strs_t exp_keys = {"a", "b", "c", "d", "ns::a"};
    vec_strs_t exp_vals[5] = {
        {"int"}, {"float"}, {"float"}, {"float"}, {"double"}};
    for(size_t i = 0; i < args.size() && i < exp_keys.size(); ++i) {
      string_t & var_name = exp_keys[i];
      EXPECT_EQ(args.count(var_name), 1u);
      vec_strs_t & val = exp_vals[i];
      EXPECT_EQ(args[var_name], val);
    }
  }
  /* Same as above, but restricted to namespace*/
  {
    string_t code =
        "template <class T> struct Aardvarks{};"
        "template <class U> using Cardvarks = Aardvarks<U>;"
        "Cardvarks<int> a;"
        "Aardvarks<float> b,c,d;"
        "namespace ns{"
        "  template <class T> struct Aardvarks{};"
        "  template <class U> using Cardvarks = Aardvarks<U>;"
        "  Cardvarks<double> a;"
        "}";
    template_var_reporter repo("Aardvarks", "ns");
    ASTUPtr ast;
    ASTContext * pctx;
    TranslationUnitDecl * decl;
    std::tie(ast, pctx, decl) = prep_code(code);
    template_var_reporter::matchers_t ms(repo.matchers());
    corct::finder_t finder;
    for(auto m : ms) { finder.addMatcher(m, &repo); }
    finder.matchAST(*pctx);
    auto & args(repo.args_);
    EXPECT_EQ(args.size(), 1);
    vec_strs_t exp_keys = {"ns::a"};
    vec_strs_t exp_vals[5] = {{"double"}};
    for(size_t i = 0; i < args.size() && i < exp_keys.size(); ++i) {
      string_t & var_name = exp_keys[i];
      EXPECT_EQ(args.count(var_name), 1u);
      vec_strs_t & val = exp_vals[i];
      EXPECT_EQ(args[var_name], val);
    }
  }

}  // TEST(template_var_matchers,template_var_reporter)

// End of file
