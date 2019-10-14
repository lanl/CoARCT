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
#include <sstream>

using namespace llvm;

// Command line support
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

using type_set_t = std::set<corct::string_t>;
using map_args_t = corct::template_var_reporter::map_args_t;
using vec_strs_t = corct::template_var_reporter::vec_strs_t;

// Functions that process the results
/**\brief distill the results into a set*/
type_set_t
collate_types(map_args_t const & args);

/** \brief Remove 'class ', 'struct ', '__1::', and ' ' */
void
fix_class(corct::string_t & s);

/**\brief Remove substring from s. */
void
seek_and_remove(corct::string_t & s, corct::string_t const & substr);

/**\brief process the set of types into result, in this case a tuple written
 * to a stringstream. */
void
process_type_set(type_set_t const & ts, std::ostream & s);

int
main(int argc, const char ** argv)
{
  using namespace corct;
  using namespace clang::tooling;
  using tvr_t = template_var_reporter;
  CommonOptionsParser opt_prs(argc, argv, TVFOpts, addl_help);
  RefactoringTool tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());
  // Alert the compiler instance to std lib header locations
  ArgumentsAdjuster ardj1 = getInsertArgumentAdjuster(clang_inc_dir1.c_str());
  ArgumentsAdjuster ardj2 = getInsertArgumentAdjuster(clang_inc_dir2.c_str());
  tool.appendArgumentsAdjuster(ardj1);
  tool.appendArgumentsAdjuster(ardj2);
  // examine command line arguments
  if(template_name.empty()) {
    printf("%s:%i Must specify a template to search for!\n", __FUNCTION__,
           __LINE__);
    return -1;
  }
  // Configure the callback object, matchers, finder
  tvr_t tr(template_name);
  if(!namespace_name.empty()) { tr.namespace_name_ = namespace_name; }
  finder_t finder;
  tvr_t::matchers_t ms(tr.matchers());
  for(auto & m : ms) { finder.addMatcher(m, &tr); }
  // run the tool
  tool.run(newFrontendActionFactory(&finder).get());
  // process the results
  type_set_t t(collate_types(tr.args_));
  std::stringstream s;
  process_type_set(t, s);
  std::cout << s.str();
  return 0;
}  // main

type_set_t
collate_types(map_args_t const & args)
{
  std::set<corct::string_t> type_set;
  for(auto p : args) {
    vec_strs_t const & type_args(p.second);
    for(auto & t : type_args) { type_set.insert(t); }
  }
  return type_set;
}  // collate_types

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
}  // fix_class

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

void
process_type_set(type_set_t const & ts, std::ostream & s)
{
  s << "using types = std::tuple<\n";
  size_t n_ts(ts.size());
  size_t i(0);
  for(auto & t1 : ts) {
    std::string t(t1);
    fix_class(t);
    s << "\t" << t;
    if(i++ < (n_ts - 1)) { s << ",\n"; }
  }
  s << ">;\n";
  return;
}  // process_type_set

// End of file
