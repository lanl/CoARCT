// member_ref.h
// Dec 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef MEMBER_REF_H
#define MEMBER_REF_H

#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Core/Replacement.h"
#include <iostream>
#include <vector>

namespace corct
{
auto
mk_member_ref_arrow(string_t const & type_name, string_t const & var_name)
{
  using namespace clang::ast_matchers;
  std::stringstream struct_ptr_name;
  struct_ptr_name << "struct " << type_name << " *";
  // clang-format off
  return
    memberExpr(
      hasObjectExpression(
        hasType(
          asString(struct_ptr_name.str())
        ) // hasType
      ) //hasObjectExpression
     ,hasDescendant(
        declRefExpr(
          to(
            varDecl(
              hasName(var_name)
            ) // varDecl
          ) // to
        ) //declRefExpr
      ) //hasDescendant
    ).bind("membrExpr") // memberExpr
  ;
  // clang-format on
};  // mk_member_ref_arrow

/** \brief Find and replace pstruct->field with struct_ref.field

  In terms of ctor parameters, <old_var_name>->fld ==> <new_var_name>.fld
*/
struct member_ref_replacer
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  using matcher_t = clang::ast_matchers::StatementMatcher;

  matcher_t matcher() const
  {
    return mk_member_ref_arrow(type_name_, old_var_name_);
  }

  /** \brief Replace arrow member expression with dot member expression */
  replacement_t mk_repl(clang::MemberExpr const * mexpr,
                        clang::SourceManager & sm)
  {
    std::stringstream repl_str;
    repl_str << new_var_name_ << ".";
    std::cout << tabs_ << "mk_repl: proposed replacement = '" << repl_str.str()
              << "'\n";
    return replace_source_range_naive(
        sm, {mexpr->getLocStart(), mexpr->getLocEnd()}, repl_str.str());
  }  // mk_repl

  void run(const result_t & result) override
  {
    using namespace clang;
    SourceManager & sm(
        const_cast<SourceManager &>(result.Context->getSourceManager()));
    MemberExpr const * mexpr = result.Nodes.getNodeAs<MemberExpr>("membrExpr");
    if(mexpr) {
      std::cout << tabs_ << "member_ref_replacer: found a match\n";
      tabs_ = add_tab(tabs_);
      dumpSourceRange({mexpr->getLocStart(), mexpr->getLocEnd()}, &sm, tabs_);
      std::cout << std::endl;
      repls_.push_back(mk_repl(mexpr, sm));
      tabs_ = remove_tab(tabs_);
    }
    else {
      check_ptr(mexpr, "memberExpr");
    }
    return;
  }  // run

  /** \param type_name: type name of struct to replace (no pointer-ness!)
      \param new_var_name: name of reference variable to be used
      \param new_var_name: name of pointer variable to seek
      \param tabs: indentation for diagnostic messages.
      \param vec_repl: replacements vector
       */
  member_ref_replacer(string_t const & type_name,
                      string_t const & new_var_name,
                      string_t const & old_var_name,
                      string_t const tabs,
                      vec_repl & repls)
      : type_name_(type_name),
        new_var_name_(new_var_name),
        old_var_name_(old_var_name),
        tabs_(tabs),
        repls_(repls)
  {}

  // state
  string_t const type_name_;
  string_t const new_var_name_;
  string_t const old_var_name_;
  string_t tabs_;

  // replacements are accumulated here in the run method.
  vec_repl & repls_;
};  // member_ref_replacer

}  // corct::

#endif  // include guard

// End of file
