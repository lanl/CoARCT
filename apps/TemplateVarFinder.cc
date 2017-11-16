// TemplateVarFinder.cc
// Nov 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

/* Find variables of a given template type, record the template type args for
 * that variable, and then process that data. Current processing generates
 * a tuple of the set of all types used in the template.
 *
 * For example, suppose you look for all variables of type std::vector, and the
 * program creates std::vector<int>, std::vector<string> and
 * std::vector<double>: this produces tuple<int,string,double> (order not
 * guaranteed). */
#include "template_var_matchers.h"
#include "types.h"
#include "utilities.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <set>

using namespace llvm;

static llvm::cl::OptionCategory TVFOpts(
    "Common options for template-var-report");

const char * addl_help =
    "Gather up all the types with which a template is instantiated; print"
    " a tuple of the set of those types.";

static cl::opt<std::string> template_name(
    "tn",
    cl::desc("Pattern of template to match, e.g.\"vector\""),
    cl::value_desc("template-name-pattern"),
    cl::cat(TVFOpts));

static cl::opt<std::string> namespace_name(
    "nn",
    cl::desc("Optional namespace to limit search e.g.\"std\" to search for "
             "\"std::vector\""),
    cl::value_desc("namespace-name"),
    cl::cat(TVFOpts),
    cl::init(""));

static cl::opt<std::string> input_traits_name(
    "nn",
    cl::desc("Optional namespace to limit search e.g.\"std\" to search for "
             "\"std::vector\""),
    cl::value_desc("namespace-name"),
    cl::cat(TVFOpts),
    cl::init(""));

using type_set_t = std::set<corct::string_t>;
using map_args_t = corct::template_var_reporter::map_args_t;
using vec_strs_t = corct::template_var_reporter::vec_strs_t;

type_set_t
collate_types(map_args_t const & args)
{
  std::set<corct::string_t> type_set;
  for(auto p : args) {
    vec_strs_t const & type_args(p.second);
    for(auto & t : type_args) {
      type_set.insert(t);
    }
  }
  return type_set;
}  // collate_types

void
fix_class(corct::string_t & s);

/**\brief Remove substring from s. */
void
seek_and_remove(corct::string_t & s, corct::string_t const & substr)
{
  size_t n = s.find(substr);
  if(n != std::string::npos) {
    s.replace(n, substr.size(), "");
    return fix_class(s);
  }
  return;
}

/** \brief Remove 'class ', 'struct ', '__1::', and ' ' */
void
fix_class(corct::string_t & s)
{
  corct::string_t struct_str("struct ");
  corct::string_t class_str("class ");
  corct::string_t ns_str("__1::");
  seek_and_remove(s, struct_str);
  seek_and_remove(s, class_str);
  seek_and_remove(s, ns_str);
  seek_and_remove(s, " ");
  return;
}

void
process_type_set(type_set_t const & ts)
{
  std::cout << "using " < < < < " = std::tuple<\n";
  size_t n_ts(ts.size());
  size_t i(0);
  for(auto & t1 : ts) {
    std::string t(t1);
    fix_class(t);
    std::cout << "\t" << t;
    if(i++ < (n_ts - 1)) {
      std::cout << ",\n";
    }
  }
  std::cout << ">;\n";
  return;
}  // process_type_set

corct::string_t clang_inc_dir1(CLANG_INC_DIR1);
corct::string_t clang_inc_dir2(CLANG_INC_DIR2);

int
main(int argc, const char ** argv)
{
  using namespace corct;
  using namespace clang::tooling;
  using tvr_t = template_var_reporter;
  CommonOptionsParser opt_prs(argc, argv, TVFOpts, addl_help);
  RefactoringTool tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());

  string_t const my_inc_dir1 = "-I" + clang_inc_dir1;
  // "-I/Users/tkelley/wnld/llvm/clang+llvm-4.0.0-x86_64-apple-darwin/include/"
  // "c++/v1";
  string_t my_inc_dir2 = "-I" + clang_inc_dir2;
  // "-I/Users/tkelley/wnld/llvm/clang+llvm-4.0.0-x86_64-apple-darwin/bin/../"
  // "lib/clang/4.0.0/include";
  printf("%s:%i my_inc_dir1: %s\n", __FUNCTION__, __LINE__,
         my_inc_dir1.c_str());
  ArgumentsAdjuster ardj1 = getInsertArgumentAdjuster(my_inc_dir1.c_str());
  ArgumentsAdjuster ardj2 = getInsertArgumentAdjuster(my_inc_dir2.c_str());
  tool.appendArgumentsAdjuster(ardj1);
  tool.appendArgumentsAdjuster(ardj2);

  if(template_name.empty()) {
    printf("%s:%i Must specify a template to search for!\n", __FUNCTION__,
           __LINE__);
    return -1;
  }
  tvr_t tr(template_name);
  if(!namespace_name.empty()) {
    tr.namespace_name_ = namespace_name;
  }
  finder_t finder;
  tvr_t::matchers_t ms(tr.matchers());
  for(auto & m : ms) {
    finder.addMatcher(m, &tr);
  }
  tool.run(newFrontendActionFactory(&finder).get());

  type_set_t t(collate_types(tr.args_));
  process_type_set(t);
  return 0;
}  // main

// End of file
