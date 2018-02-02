// callsite_common.h
// T. M. Kelley

// functions used by several callsite clients.

#pragma once

#include "clang/ASTMatchers/ASTMatchers.h"

namespace corct{

// clang-format off
/** Match a call to function with specified name, bound in "function". Pretty
 sure this won't pick up a function pointer (TODO need to check that).
*/
inline
auto mk_fn_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name,
  std::string const & targ_name)
{
  using namespace clang::ast_matchers;
  return callExpr(
    unless(isExpansionInSystemHeader()),
    hasDescendant(
      declRefExpr(
        to(
          functionDecl(
            hasName(targ_name)
          ).bind(fn_bind_name)
        )
      )
    )//.bind("recDecl")
  ).bind(cs_bind_name);
} // mk_fn_call_matcher

/** Match a callsite (bound to ) to any function (bound to "fn_bd_name"). Pretty
 sure this won't pick up a function pointer (TODO need to check that).
*/
inline
auto mk_any_fn_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name)
{
  using namespace clang::ast_matchers;
  return callExpr(
    unless(isExpansionInSystemHeader()),
    hasDescendant(
      declRefExpr(
        to(
          functionDecl().bind(fn_bind_name)
        )
      )
    )//.bind("recDecl")
  ).bind(cs_bind_name);
} // mk_fn_call_matcher

/** Match a call to bound member function with specified name, with
 * node bound to 'fn_bind_name'. This will work with with object,
 * reference, or pointer to object.
 */
inline
auto mk_mthd_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name,
  std::string const & targ_name)
{
  using namespace clang::ast_matchers;
  return cxxMemberCallExpr(
    unless(isExpansionInSystemHeader()),
    callee(
      cxxMethodDecl(
        hasName(targ_name)
      ).bind(fn_bind_name) // cxxMethodDecl
    ) // callee
  ).bind(cs_bind_name);
} // mk_mthd_call_matcher

/** Match callsite (bound to cs_bind_name) to any bound member function
 * (bound to fn_bind_name), with node bound to 'fn_bind_name'.
 * This will work with with object, reference, or pointer to object.
 */
inline
auto mk_any_mthd_call_matcher(
  std::string const & cs_bind_name,
  std::string const & fn_bind_name)
{
  using namespace clang::ast_matchers;
  return cxxMemberCallExpr(
    unless(isExpansionInSystemHeader()),
    callee(
      cxxMethodDecl().bind(fn_bind_name) // cxxMethodDecl
    ) // callee
  ).bind(cs_bind_name);
} // mk_mthd_call_matcher

/** Match any call expression binding to a method declaration if possible, and
 * to a function declaration otherwise.
 *
 * \param cs_bind_name: name to bind the callsite to
 * \param mt_bind_name: name to bind a method declaration to
 * \param fn_bind_name: name to bind a function declaration to
 * \return expression matcher
 *
 * One of mt_bind_name or fn_bind_name should be valid.
 */
inline
auto mk_callsite_matcher(
  std::string const & cs_bind_name,
  std::string const & mt_bind_name,
  std::string const & fn_bind_name)
{
  using namespace clang::ast_matchers;
  return callExpr(
    unless(isExpansionInSystemHeader()),
    anyOf(
      callee(
        cxxMethodDecl().bind(mt_bind_name) // cxxMethodDecl
      ) // callee
     ,callee(
        functionDecl().bind(fn_bind_name)
     ) // callee
    ) // anyOf
  ).bind(cs_bind_name); // callExpr
} // mk_mthd_call_matcher

/** Match a call expression to a particular function name, binding to a
 * method declaration if possible, and to a function declaration otherwise.
 *
 * \param cs_bind_name: name to bind the callsite to
 * \param mt_bind_name: name to bind a method declaration to
 * \param fn_bind_name: name to bind a function declaration to
 * \param target_name: name of callee
 * \return expression matcher
 *
 * One of mt_bind_name or fn_bind_name should be valid.
 */
inline
auto mk_callsite_matcher(
  std::string const & cs_bind_name,
  std::string const & mt_bind_name,
  std::string const & fn_bind_name,
  std::string const & target_name)
{
  using namespace clang::ast_matchers;
  return callExpr(
    unless(isExpansionInSystemHeader()),
    anyOf(
      callee(
        cxxMethodDecl(
          hasName(target_name)
        ).bind(mt_bind_name) // cxxMethodDecl
      ) // callee
     ,callee(
        functionDecl(
          hasName(target_name)
        ).bind(fn_bind_name)
     ) // callee
    ) // anyOf
  ).bind(cs_bind_name); // callExpr
} // mk_mthd_call_matcher

// clang-format on

} // corct::

// End of file
