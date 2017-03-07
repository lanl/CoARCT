// function_sig_matchers.h
// T. M. Kelley
// Feb 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved


#ifndef FUNCTION_SIG_MATCHERS_H
#define FUNCTION_SIG_MATCHERS_H

#include "dump_things.h"
#include "types.h"
#include "utilities.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <iostream>
#include <map>
#include <sstream>
#include <boost/type_index.hpp>
#include <initializer_list>

//template <typename T> const char * type_as_string(){return "unknown";}
//
//// clang-format off
//#define STRINGIFY_TYPE(t) template <> const char * type_as_string<t>(){return #t;}
//// clang-format on

/*
This is a non-trivial bit of code to write. the goal is to create a matcher
that matches all functions with the same signature. The trick I've counted
on is using a variadic template struct called Gen_Params that adds a
parameter matcher for each parameter, as well as a return type matcher.

Things that are missing:
1. I have a mechanism for identifying types:
  hasType(asString(type_as_string<t>() ))
The problem with that is somewhere, the function type_as_string<T> needs to be
instantiated. Right now, that's done with the CPP macro STRINGIFY_TYPE. So the
user would need to explicitly STRINGIFY_TYPE for each type in the function sig.
Not a killer, necessarily, but it would be nice to be able to make that
implicit.

For example, is there some way to reify the type in the code, so that I could
write
  template <class T>
  mk_matcher(){
    parmVarDecl(hasType(T))

Bwa-ha-ha: found two ways: boost and __cxa_demangle(typeid(T).name())
2. Need a way to sew up the Return matcher and the set of param matchers into
a functionDecl. For instance, map a type list to a tuple of parameter matchers,
then fold that tuple into the functionDecl matcher.
3.
*/

namespace corct{

template <class T>
string_t type_as_string(){
  return boost::typeindex::type_id<T>().pretty_name();
} // type_as_string

template <typename ...Ts>
struct param_list{
  template <size_t N> using type =
    typename std::tuple_element<N,std::tuple<Ts...>>::type;
};

template <typename T, typename ...Ts>
struct Print_Params{
  void operator()(){
    std::cout << type_as_string<T>() << "\n";
    Print_Params<Ts...>()();
  } // operator()
}; // Print_Params

template <typename T>
struct Print_Params<T>{
  void operator()(){
    std::cout << type_as_string<T>() << "\n";
  } // operator()
}; // Print_Params

template <class C>
struct Function_Sig{};

// clang-format off
template <size_t n,typename T>
auto param_matcher(){
  using namespace clang::ast_matchers;
  std::stringstream bstr;
  bstr << "parm" << n;
  string_t type_str(type_as_string<T>());
  // clang-format off
  return
    hasParameter(
      n,
      parmVarDecl(
        hasType(
          asString(
            type_str
          )
        )
      ).bind(bstr.str()) // parmVarDecl
    ) // hasParameter
  ;
  // clang-format on
} // param_matcher
// clang-format on

template <typename T, size_t s>
size_t f(){return s;}

template <typename ...Ts, std::size_t ...Ss>
auto mk_tuple_fs_impl(std::index_sequence<Ss...>){
  // std::index_sequence_for<Ts...> Ss;
  return std::make_tuple(f<Ts,Ss>()...);
}

template <typename ...Ts, typename Idcs = std::index_sequence_for<Ts...>>
auto mk_tuple_fs(){
  return mk_tuple_fs_impl<Ts...>(Idcs() );
}

using MType = decltype(
    clang::ast_matchers::hasParameter(0, clang::ast_matchers::parmVarDecl()));
using DMVec = std::vector<MType>;

namespace detail{
template <typename ...Ts, size_t ...Ss>
auto mk_params_impl(std::index_sequence<Ss...>){
  return std::make_tuple(param_matcher<Ss,Ts>()...);
}
} // detail::

template <typename ...Ts, typename Idcs = std::index_sequence_for<Ts...>>
auto mk_params(){
  return detail::mk_params_impl<Ts...>(Idcs());
}

template <class Tuple, size_t ...Us>
auto allOfParams_impl(Tuple &t, std::index_sequence<Us...>){
  using namespace clang::ast_matchers;
  return
  // functionDecl(
  //   returns(asString( "int"/*type_as_string<Ret_T>()*/ )),
    allOf(
      std::get<Us>(t)...
    ) // allOf
  // )
  ;
}

template <class Ret_T, class ...Ts, size_t ...Us>
auto enchilada_impl(std::index_sequence<Us...>){
  using namespace clang::ast_matchers;
  string_t const ret_str = type_as_string<Ret_T>();
  return
  functionDecl(
    returns(asString( ret_str)),
    allOf(
      param_matcher<Us,Ts>()...
    ) // allOf
  )
  ;
}


// template <class Tuple, size_t ...Us>
// auto allOfParams_impl(Tuple &&t, std::index_sequence<Us...>){
//   return
//   clang::ast_matchers::allOf(
//     std::get<Us>(t)...
//   );
// }

/* Problem: it's hard to name the type of this thing: as the type is meant
to be something like params_again<MType...> */
template <typename ...Ts>
struct params_again{

  using Idcs = std::index_sequence_for<Ts...>;

  std::tuple<Ts...> params_;

  auto allOfParams(){
    return allOfParams_impl<Ts...>(params_,Idcs());
  } // allParams

  explicit params_again(std::tuple<Ts...> & t) : params_(t){}
  explicit params_again(std::tuple<Ts...> && t) : params_(t){}
}; // params_again

template <class...> struct types{using type=types;};
template <class Sig> struct args;
template <class R, class...Args> struct args<R(Args...)> : types<Args...>{};
template <class Sig> using args_t = typename args<Sig>::type;


template <typename Ret_T, typename ...Args>
struct Function_Sig<Ret_T(*)(Args...)> : types<Args...>{
  using PA_T = decltype(mk_params<Args...>);
  using Idcs = std::index_sequence_for<Args...>;

  static
  auto func_sig_matcher(){
    using namespace clang::ast_matchers;
    auto m = enchilada_impl<Ret_T,Args...>(Idcs());
    return m;
    // auto p_matchers = params_matcher();
    // auto r_matcher = ret_type_matcher();
    // return
    //   // enchilada_impl<Ret_T,PA_T>(mk_params<Args...>(), Idcs());
    //   functionDecl(
    //     r_matcher,
    //     p_matchers
    //     // ret_type_matcher()
    //     // ,
    //     // params_matcher()
    //   );
  } // func_sig_matcher

  static
  auto params_matcher(){
    auto p_matchers = mk_params<Args...>();
    return allOfParams_impl(p_matchers, Idcs());
    // return allOfParams_impl(mk_params<Args...>(), Idcs());
  } // params_matcher

  static
  auto ret_type_matcher(){
    using namespace clang::ast_matchers;
    string_t ret_str(type_as_string<Ret_T>());
    return returns(asString("double"));
  } // ret_type_matcher
};

template <int i, typename T, typename ...Ts>
struct Print_ParamsN{
  void operator()(){
    std::cout << i << ":" << type_as_string<T>() << ", ";
    Print_ParamsN<i-1,Ts...>()();
  } // operator()
}; // Print_Params

template <typename T>
struct Print_ParamsN<1,T>{
  void operator()(){
    std::cout << "1:" << type_as_string<T>() << "\n";
  } // operator()
}; // Print_Params


//template <typename ...Ts>
//struct match_maker{
//  auto operator()(){
//    using namespace clang::ast_matchers;
//    return
//    functionDecl(
//      allOf(
//        (param_matcher<Is...,Ts...>(),...)
//      ) // allOf
//    ) // functionDecl
//  }
//}; // match_maker

//template <size_t i,size_t N,typename T, typename... Ts>
//struct Gen_Params{
//  void operator()(DMVec & ms){
//    ms.push_back( param_matcher<N-i-1,T>() );
//    return Gen_Params<i-1, N,Ts...>()(ms);
//  } // operator()
//};
//
//template <size_t N, typename T>
//struct Gen_Params<1,N,T>{
//  void operator()(DMVec & ms){
//    ms.push_back( param_matcher<N-1,T>() );
//    return;
//  }
//};
//

// template <typename T>
// void printParamTypes(){
//   std::cout << type_as_string<T>() << "\n";
// }

//template <typename T, typename ...Ts>
//// typename std::enable_if<i < N,void>::type
//void
//printParamTypes(){
//  // const char *pname = type_as_string< param_list<Ts...>::type<i> >();
//  std::cout << type_as_string<T>() << "\n";
//  printParamTypes<Ts...>();
//  // if(i < N){
//  //   printParamTypes<i+1,N,Ts...>();
//  // }
//}


//   using namespace clang::ast_matchers;
//   // return returns(asString(type_as_string<T>()));
// } // match_t base

// template <uint32_t n, typename T>
// auto gen_param_t(){
//   using namespace clang::ast_matchers;
//   // return returns(asString(type_as_string<T>()));
// } // match_t base

// template <uint32_t n, typename T, typename... Ts>
// auto gen_param_t(){
//   using namespace clang::ast_matchers;
//   return n > 1 ?
//     allOf(
//       hasParameter(
//         n,
//         hasType(
//           asString(
//             type_as_string<T>()
//           )
//         )
//       ),
//       gen_param_t<(n-1u),Ts...>()
//     ) // allOf
//     : hasParameter(
//         n,
//         hasType(
//           asString(
//             type_as_string<T>()
//           )
//         )
//       ) // hasParameter
//   ;
// }// match_T

} // corct::

#endif // include guard


// End of file
