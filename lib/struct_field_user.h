// struct_field_user.h
// Oct 13, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef struct_field_user_H
#define struct_field_user_H

#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include <algorithm>  // std::remove
#include <iostream>
#include <map>
#include <set>
#include <sstream>

namespace corct {
/* Match expressions using members of struct sname, whether in
  the form s.field or s->field
 */
auto
mk_struct_field_matcher(string_t const & sname)
{
  using namespace clang::ast_matchers;
  string_t s_ptr_name = "struct " + sname + " *";
  // clang-format off
  return
    memberExpr(
      isExpansionInMainFile(),
      anyOf(
        hasObjectExpression(
          hasType(
            cxxRecordDecl(
              hasName(
                sname
              )
            ).bind("structure_decl")
          ) // hasType
        ), // hasObjectExpression
        hasObjectExpression(
          hasType(
            asString(s_ptr_name)
          ) // hasType
        ) // hasObjectExpression
      ) // anyOf
     ,hasAncestor(
        functionDecl().bind("function")
      )
    ).bind("memberExpr");
  // clang-format on
}

/** Each time a target struct has a member accessed, track which function used
it and whether it's used on the LHS or RHS (really non-LHS) of an expression.
  */
struct struct_field_user : public callback_t {
public:
  using set_t = std::set<string_t>;
  using func_mem_map_t =
      std::map<string_t, set_t>;  // Key: func name, Vals: Members
  using struct_f_m_map_t = std::map<string_t, func_mem_map_t>;
  using matcher_t = clang::ast_matchers::StatementMatcher;
  using matchers_t = std::vector<matcher_t>;

  matchers_t matchers() const
  {
    matchers_t ms;
    for(auto t : targets_) { ms.push_back(mk_struct_field_matcher(t)); }
    return ms;
  }

  void run(const result_t & result) override
  {
    using namespace clang;
    n_matches_++;
    ASTContext & ctx(*(result.Context));
    MemberExpr const * membr = result.Nodes.getNodeAs<MemberExpr>("memberExpr");
    FunctionDecl const * func =
        result.Nodes.getNodeAs<FunctionDecl>("function");
    if(membr && func) {
      string_t const s_name = get_struct_name(*membr);
      string_t const f_name = func->getNameAsString();
      string_t const m_name = membr->getMemberDecl()->getNameAsString();
      Expr const * m_base = membr->getBase();
      string_t const b_name = m_base->getType().getAsString();
      bool const on_lhs(is_on_lhs(membr, ctx));
      if(on_lhs) { lhs_uses_[s_name][f_name].insert(m_name); }
      else {
        non_lhs_uses_[s_name][f_name].insert(m_name);
      }
    }
    else {
      check_ptr(membr, "membr");
      check_ptr(func, "func");
    }
    return;
  }  // run

  explicit struct_field_user(vec_str & targets)
      : targets_(targets), n_matches_(0)
  {
  }

  // state:
  vec_str targets_;
  struct_f_m_map_t lhs_uses_;
  struct_f_m_map_t non_lhs_uses_;
  uint32_t n_matches_;
};  // struct_field_user

}  // namespace corct

#endif  // include guard

// End of file
