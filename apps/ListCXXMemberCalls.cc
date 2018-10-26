// ListCXXMemberCalls.cc
// Oct 26, 2018
// (c) Copyright 2018 LANSLLC, all rights reserved

/* Demonstrate AST Matcher to list all cxx member calls in a namespace.
 * This was inspired by Stack Overflow #52993315 */

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "dump_things.h"
#include "llvm/Support/CommandLine.h"
#include "utilities.h"
#include <iostream>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

/* Make a matcher for call expressions in namespace ns_name */
auto
mk_call_expr_matcher(std::string const & ns_name)
{
  /* There is more than one way to write an AST matcher. Here are three looks
   * at accomplishing (or trying to accomplish) the same goal. */

  /* First: the easy way of writing this matcher. It might be inefficient in a
   * large project, if it matched many call expressions, only to reject
   * them because the enclosing namespace was wrong. But it is correct. */
  // return cxxMemberCallExpr(hasAncestor(namespaceDecl(hasName(ns_name))))
  //     .bind("call");

  /* Second: try to match first on namespace, then on call expr. But i't
   * work as wanted--it will stop after the first match! */
  // return namespaceDecl(hasName(ns_name),
  //                      hasDescendant(cxxMemberCallExpr().bind("call")));

  /* Third: match first on namespace, then use forEachDescendant to match all
   * the callexpr's. */
  return namespaceDecl(hasName(ns_name),
                       forEachDescendant(cxxMemberCallExpr().bind("call")));
}

/* CallPrinter::run() will be called when a match is found */
struct CallPrinter : public MatchFinder::MatchCallback {
  void run(MatchFinder::MatchResult const & result) override
  {
    using namespace clang;
    SourceManager & sm(result.Context->getSourceManager());
    CXXMemberCallExpr const * call =
        result.Nodes.getNodeAs<CXXMemberCallExpr>("call");
    if(call) {
      num_calls++;
      auto const method_name(call->getMethodDecl()->getNameAsString());
      auto const callee_name(call->getRecordDecl()->getNameAsString());
      std::cout << "Method '" << method_name << "' invoked by object of type '"
                << callee_name << "' at "
                << corct::locationAsString(call->getBeginLoc(), &sm) << "\n";
    }
    else {
      corct::check_ptr(call, "call");
    }
    return;
  }  // run

  uint32_t num_calls = 0;
};  // CallPrinter

static llvm::cl::OptionCategory mem_cat("list-member-call options");

static cl::opt<std::string> ns_name_string("ns",
                                           cl::desc("namespace"),
                                           cl::value_desc("ns"),
                                           cl::cat(mem_cat),
                                           cl::init(""));

int
main(int argc, const char ** argv)
{
  CommonOptionsParser OptionsParser(argc, argv, mem_cat);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  CallPrinter cp;
  MatchFinder finder;

  finder.addMatcher(mk_call_expr_matcher(ns_name_string), &cp);
  int rslt = Tool.run(newFrontendActionFactory(&finder).get());
  std::cout << "Reported " << cp.num_calls << " member calls\n";
  return rslt;
}

// End of file
