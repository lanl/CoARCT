
// LoopConvert.cc
// T. M. Kelley
// Aug 17, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

/* Like GlobalDetect, only now we're trying to replace some stuff. */

#include "callsite_expander.h"
#include "dump_things.h"
#include "function_signature_expander.h"
#include "global_variable_replacer.h"
#include "make_replacement.h"
#include "utilities.h"

#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Type.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <vector>

using namespace clang::tooling;
using namespace llvm;

using corct::vec_str;

const char * addl_help =
    "Replace global variable references with locals, or thread new local "
    "variables through call chains";

static llvm::cl::OptionCategory XpndOpts("Expanding function signatures");

static llvm::cl::opt<bool> expand_func(
    "Xpnd",
    llvm::cl::desc("insert new parameter NP to target function tf, new arg "
                   "(NA) to calls to tf"),
    llvm::cl::cat(XpndOpts),
    llvm::cl::init(false));

static cl::opt<std::string> new_func_param_string(
    "np",
    cl::desc("param to add to the global-using functions' signature (with "
             "-xpnd), e.g. -np=\"SimConfig const & sim_config\""),
    cl::value_desc("new-param-string"),
    cl::cat(XpndOpts));

static cl::opt<std::string> new_func_arg_string(
    "na",
    cl::desc("argument to add to the global-using functions' call sites (with "
             "-xpnd), e.g. -na=\"sim_config\""),
    cl::value_desc("new-arg-string"),
    cl::cat(XpndOpts));

static llvm::cl::OptionCategory RrOpts(
    "Fixing/replacing references to globals");

static cl::opt<bool> rep_refs(
    "R",
    cl::desc(
        "replace references to global vars (leave function signatures alone)"),
    cl::cat(RrOpts),
    cl::init(false));

static cl::opt<std::string> old_var_string(
    "gvar",
    cl::desc("global variable(s) to replace, comma separated (with -R), e.g. "
             "-old=\"NR,NB\""),
    cl::value_desc("old-var-string"),
    cl::cat(RrOpts));

static cl::opt<std::string> new_var_string(
    "lvar",
    cl::desc("each global variable's replacement, comma separated, resp. to "
             "old-var-string; e.g. -new=\"sim_cgf.NR,sim_cfg.NB\""),
    cl::value_desc("new-var-string"),
    cl::cat(RrOpts));

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

// some command line options:
static llvm::cl::OptionCategory GROpts("Common options for global-replace");

static cl::opt<std::string> target_func_string(
    "tf",
    cl::desc("target function(s) to modify, separated by commas if nec. E.g. "
             "-tf=\"foo,bar,baz\""),
    cl::value_desc("target-function-string"),
    cl::cat(GROpts));

static cl::opt<bool> dry_run("d",
                             cl::desc("dry run"),
                             cl::cat(GROpts),
                             cl::init(false));

static llvm::cl::OptionCategory CompilationOpts("General compiler options:");

void
announce_dry(bool const dry_run_)
{
  if(dry_run_) {
    std::cout << "This is a dry run\n";
  }
  else {
    std::cout << "This is not a dry run\n";
  }
  return;
}  // announce_dry

void
list_compilations(CommonOptionsParser & opt_prs)
{
  auto & comps(opt_prs.getCompilations());
  std::cout << "# of compile commands: " << comps.getAllCompileCommands().size()
            << "\n";
  std::cout << "Sources from compilation:\n";
  for(auto & source : opt_prs.getSourcePathList()) {
    std::cout << source << "\n";
  }
  return;
}

int
main(int argc, const char ** argv)
{
  using corct::split;
  CommonOptionsParser opt_prs(argc, argv, CompilationOpts, addl_help);
  RefactoringTool Tool(opt_prs.getCompilations(), opt_prs.getSourcePathList());

  announce_dry(dry_run);
  list_compilations(opt_prs);

  vec_str old_var_strings(split(old_var_string, ','));
  vec_str new_var_strings(split(new_var_string, ','));
  if(old_var_strings.size() != new_var_strings.size()) {
    std::cerr << "Must have one replacement for each global variable!\n";
    return -1;
  }

  corct::global_variable_replacer v_replacer(
      &Tool.getReplacements(), old_var_strings, new_var_strings, dry_run);

  // sort out target functions
  vec_str targ_fns(split(target_func_string, ','));

  corct::function_signature_expander f_expander(
      Tool.getReplacements(), targ_fns, new_func_param_string, dry_run);

  corct::expand_callsite s_expander(Tool.getReplacements(), targ_fns,
                                    new_func_arg_string, dry_run);

  clang::ast_matchers::MatchFinder finder;

  corct::global_variable_replacer::matchers_t global_ref_matchers =
      v_replacer.matchers();
  corct::function_signature_expander::matchers_t exp_matchers =
      f_expander.fn_matchers();
  corct::expand_callsite::matchers_t site_matchers = s_expander.fn_matchers();

  std::cout << targ_fns.size() << " targets, along with " << exp_matchers.size()
            << " matchers\n";

  if(rep_refs) {
    for(auto & m : global_ref_matchers) {
      finder.addMatcher(m, &v_replacer);
    }
  }
  else if(expand_func) {
    std::cout << "Expanding functions\n";
    for(uint32_t i = 0; i < site_matchers.size(); ++i) {
      finder.addMatcher(site_matchers[i], &s_expander);
      finder.addMatcher(exp_matchers[i], &f_expander);
    }
  }

  Tool.runAndSave(newFrontendActionFactory(&finder).get());

  llvm::outs() << "Replacements collected: \n";
  for(auto & r : Tool.getReplacements()) {
    llvm::outs() << r.toString() << "\n";
  }
  return 0;
}  // main

// End of file
