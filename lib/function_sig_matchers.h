// function_sig_matchers.h
// Feb 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#ifndef FUNCTION_SIG_MATCHERS_H
#define FUNCTION_SIG_MATCHERS_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "dump_things.h"
#include "types.h"
#include "utilities.h"
#include <boost/type_index.hpp>
#include <initializer_list>
#include <iostream>
#include <map>
#include <sstream>
#include <type_traits>

namespace corct {

template <class T>
string_t
type_as_string()
{
  return boost::typeindex::type_id<T>().pretty_name();
}  // type_as_string

/*
references
isConstQualified
isVolatileQualified
*/

/**\brief Generate a matcher to parameter n, of type T.
 * ParmVarDecl bound to "parm"
 */
template <size_t n,typename T>
auto param_matcher(){
  using namespace clang::ast_matchers;
  std::stringstream pstr;
  pstr << "parm" << n;
  std::stringstream tstr;
  tstr << type_as_string<T>();
  if(std::is_const<T>::value) tstr << " const";
  if(std::is_volatile<T>::value) tstr << " volatile";
  if(std::is_lvalue_reference<T>::value) tstr << "&";
  if(std::is_rvalue_reference<T>::value) tstr << "&&";
  printf("%s:%i Creating matcher for param %lu, type '%s'\n",
    __FUNCTION__,__LINE__,n,tstr.str().c_str());
  // clang-format off
  return
    hasParameter(
      n,
      parmVarDecl(
        hasType(
          asString(
            tstr.str()
          )
        )
      ).bind(pstr.str()) // parmVarDecl
    ) // hasParameter
  ;
  // clang-format on
}  // param_matcher
// clang-format on

template <class... Ts, size_t... Us>
auto
allOf_params_impl(std::index_sequence<Us...>)
{
  return clang::ast_matchers::allOf(param_matcher<Us, Ts>()...);
}

template <class C> struct Function_Sig {};

template <typename Ret_T, typename... Args>
struct Function_Sig<Ret_T (*)(Args...)> {
  using Idcs = std::index_sequence_for<Args...>;

  static auto func_sig_matcher()
  {
    using namespace clang::ast_matchers;
    return functionDecl(ret_type_matcher(), params_matcher()).bind("fdecl");
  }  // func_sig_matcher

  static auto params_matcher() { return allOf_params_impl<Args...>(Idcs()); }
  static auto ret_type_matcher()
  {
    using namespace clang::ast_matchers;
    return returns(asString(type_as_string<Ret_T>()));
  }  // ret_type_matcher
};   // Function_Sig

}  // corct::

#endif  // include guard

// End of file
