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
  std::stringstream type_strm;
  if(std::is_const<T>::value) {
    type_strm << "const ";
  }
  type_strm << boost::typeindex::type_id<T>().pretty_name();
  return type_strm.str();
}  // type_as_string

/**\brief Generate a matcher to parameter n, of type T.
 * ParmVarDecl bound to "parm"
 */
template <size_t n, typename T>
auto
param_matcher()
{
  // printf("%s:%i\n", __PRETTY_FUNCTION__, __LINE__);
  using namespace clang::ast_matchers;
  std::stringstream pstr;
  pstr << "parm" << n;
  std::stringstream tstr;
  bool const is_const(std::is_const<std::remove_reference_t<T>>::value);
  bool const is_volatile(std::is_volatile<std::remove_reference_t<T>>::value);
  bool const is_lvalue_reference(std::is_lvalue_reference<T>::value);
  // bool const is_rvalue_reference(std::is_rvalue_reference<T>::value);

  if(is_const) tstr << "const ";
  if(is_volatile) tstr << "volatile ";
  tstr << type_as_string<T>();
  // clang-format off
  auto t_match =
    hasType(
      qualType(
        asString(
          tstr.str()
        )
        ,
        is_volatile ? isVolatileQualified() : unless(isVolatileQualified()),
        is_const ? isConstQualified() : unless(isConstQualified())
      ) //qualType
    ); // hasType
  auto ref_t_match =
    hasType(
      references(
        qualType(
          asString(
            tstr.str()
          )
          ,
          is_volatile ? isVolatileQualified() : unless(isVolatileQualified()),
          is_const ? isConstQualified() : unless(isConstQualified())
        ) //qualType
      ) // references
    ); // hasType
  return
    hasParameter(
      n,
      parmVarDecl(
        is_lvalue_reference ?
          (/*printf("%s:%i ref\n",__FUNCTION__,__LINE__),*/ ref_t_match) :
          (/*printf("%s:%i non-ref\n",__FUNCTION__,__LINE__),*/ t_match)
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

template <class C>
struct Function_Sig {
};

template <typename Ret_T, typename... Args>
struct Function_Sig<Ret_T (*)(Args...)> {
  using Idcs = std::index_sequence_for<Args...>;

  static auto func_sig_matcher()
  {
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    using namespace clang::ast_matchers;
    return functionDecl(ret_type_matcher(), params_matcher()).bind("fdecl");
  }  // func_sig_matcher

  static auto params_matcher() { return allOf_params_impl<Args &&...>(Idcs()); }
  static auto ret_type_matcher()
  {
    using namespace clang::ast_matchers;
    return returns(asString(type_as_string<Ret_T>()));
  }  // ret_type_matcher

};  // Function_Sig

template <class C>
struct Param_Traits{};

template <class R, class... Args>
struct Param_Traits<R (*)(Args...)> {
  struct type_traits {
    bool is_const_;
    bool is_volatile_;
    bool is_lvalue_reference_;
    bool is_rvalue_reference_;
    string_t base_name_;

    string_t to_string() const
    {
      std::stringstream s;
      s << std::boolalpha << "{"
        << "base_name: " << base_name_ << "; const: " << is_const_
        << "; volatile: " << is_volatile_
        << "; lvalue_reference: " << is_lvalue_reference_
        << "; rvalue_reference: " << is_rvalue_reference_ << "}";
      return s.str();
    }
  }; // struct type_traits

  using vec_type_traits = std::vector<type_traits>;

  // static
  vec_type_traits param_type_traits()
  {
    vec_type_traits tis = {
        {std::is_const<std::remove_reference_t<Args>>::value,
         std::is_volatile<Args>::value, std::is_lvalue_reference<Args>::value,
         std::is_rvalue_reference<Args>::value, type_as_string<Args>()}...};
    return tis;
  }  // param_type_traits
};

}  // corct::

#endif  // include guard

// End of file
