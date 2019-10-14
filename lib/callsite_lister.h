// callsite_lister.h
// Jan 25, 2018

#pragma once

#include "callsite_common.h"
#include "dump_things.h"
#include "types.h"
#include "utilities.h"

namespace corct {

// helper
void
print_call_details(clang::FunctionDecl const * callee,
                   clang::FunctionDecl const * caller,
                   clang::CallExpr const * callsite,
                   clang::SourceManager & sm,
                   std::ostream & o);

/**\class callsite_lister. This will identify all functions in which a callsite
 * is present.
 *
 * The search behavior can be limited to specific callee functions or
 * methods by naming them in the ctor's 'targets' parameter. If 'targets' is
 * empty, all functions with callsites will be listed.
 *
 * When a function is found that calls one of the targets, its name and source
 * range will be printed.
 */
struct callsite_lister : public callback_t {
  using matcher_t = clang::ast_matchers::DeclarationMatcher;
  using matchers_t = std::vector<matcher_t>;

  matchers_t matchers()
  {
    using namespace clang::ast_matchers;
    matchers_t ms;
    if(m_targets.empty()) {
      auto callsite_m(mk_callsite_matcher(cs_bd_name, mt_bd_name, fn_bd_name));
      auto m = functionDecl(hasDescendant(callsite_m)).bind(caller_bd_name);
      ms.push_back(m);
    }
    else {
      for(auto & t : m_targets) {
        auto callsite_m(
            mk_callsite_matcher(cs_bd_name, mt_bd_name, fn_bd_name, t));
        auto m = functionDecl(hasDescendant(callsite_m)).bind(caller_bd_name);
        ms.push_back(m);
      }
    }
    return ms;
  }  // matchers

  void run(result_t const & result) override
  {
    using namespace clang;
    FunctionDecl const * fdecl =
        result.Nodes.getNodeAs<FunctionDecl>(fn_bd_name);
    FunctionDecl const * caller =
        result.Nodes.getNodeAs<FunctionDecl>(caller_bd_name);
    CXXMethodDecl const * mdecl =
        result.Nodes.getNodeAs<CXXMethodDecl>(mt_bd_name);
    CallExpr const * csite = result.Nodes.getNodeAs<CallExpr>(cs_bd_name);
    SourceManager & sm(result.Context->getSourceManager());
    if(csite && fdecl && caller) {
      print_call_details(fdecl, caller, csite, sm, std::cout);
      m_num_calls++;
    }
    else if(csite && mdecl && caller) {
      print_call_details(mdecl, caller, csite, sm, std::cout);
      m_num_calls++;
    }
    else {
      check_ptr(csite, "csite");
      check_ptr(fdecl, "fdecl");
      check_ptr(mdecl, "mdecl");
      check_ptr(caller, "caller");
    }
    return;
  }  // run

  explicit callsite_lister(vec_str const & targets) : m_targets(targets) {}

  uint32_t m_num_calls = 0;

private:
  vec_str m_targets;
  static string_t const cs_bd_name;
  static string_t const mt_bd_name;
  static string_t const fn_bd_name;
  static string_t const caller_bd_name;
};  // callsite_lister

/**\class callsite_lister: Count sites where functions or methods are called.
 * If a target set is defined, only calls to functions with names in that set
 * will be reported. If no set is supplied, all calls will be listed.
 * This demonstrates and tests the matchers in callsite_common
 */
struct callsite_counter : public callback_t {
  using matcher_t = clang::ast_matchers::StatementMatcher;
  using matchers_t = std::vector<matcher_t>;

  matchers_t matchers()
  {
    matchers_t ms;
    if(m_targets.empty()) {
      ms.push_back(mk_callsite_matcher(cs_bd_name, mt_bd_name, fn_bd_name));
    }
    else {
      for(auto & t : m_targets) {
        ms.push_back(
            mk_callsite_matcher(cs_bd_name, mt_bd_name, fn_bd_name, t));
      }
    }
    return ms;
  }  // matchers

  void run(result_t const & result) override
  {
    using namespace clang;
    FunctionDecl const * fdecl =
        result.Nodes.getNodeAs<FunctionDecl>(fn_bd_name);
    CXXMethodDecl const * mdecl =
        result.Nodes.getNodeAs<CXXMethodDecl>(mt_bd_name);
    CallExpr const * csite = result.Nodes.getNodeAs<CallExpr>(cs_bd_name);
    if(csite && mdecl) { m_num_calls++; }
    else if(csite && fdecl) {
      m_num_calls++;
    }
    else {
      check_ptr(csite, "csite");
      check_ptr(fdecl, "fdecl");
      check_ptr(mdecl, "mdecl");
    }
    return;
  }  // run

  explicit callsite_counter(vec_str const & targets) : m_targets(targets) {}

  uint32_t m_num_calls = 0;

private:
  vec_str m_targets;
  string_t const cs_bd_name = "callsite";
  string_t const mt_bd_name = "m_decl";
  string_t const fn_bd_name = "f_decl";
};  // callsite_lister

}  // namespace corct

// End of file
