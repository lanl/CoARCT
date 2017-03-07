// small_matchers.h
// Dec 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef SMALL_MATCHERS_H
#define SMALL_MATCHERS_H

#include "dump_things.h"
#include "types.h"
#include "utilities.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <iostream>

namespace corct
{
/** \brief Match a reference to a pointer, binding variable name bd_var_name
  to the pointer declaration and bd_ref_nm to the reference to the pointer.

  Think "p" in "p != end"
  */
inline auto
mk_ptr_matcher(str_t_cr bd_var_nm, str_t_cr bd_ref_nm, str_t_cr ptr_var_nm = "")
{
  using namespace clang::ast_matchers;
// clang-format off
  if(ptr_var_nm == ""){
    return
    declRefExpr(
      hasType(
        pointerType()
      )  //hasType
     ,to(
        varDecl().bind(bd_var_nm)
      ) // to
    ).bind(bd_ref_nm); // declRefExpr
  }
  else{
    return
    declRefExpr(
      hasType(
        pointerType()
      ) // hasType
     ,to(
        varDecl(
          hasName(ptr_var_nm)
      ).bind(bd_var_nm)) // to
    ).bind(bd_ref_nm); // declRefExpr
  }
// clang-format on
} // mk_ptr_matcher

/** \brief "p->next"

Match a member expression that is an arrow to field 'next', and
a reference to pointer on the LHS of the arrow, optionally specifying the
pointer variable name. Binds bd_var_name to the (pointer) variable declaration
and bd_ref_nm to the reference to the pointer.
*/
inline auto
match_arrow_next(str_t_cr bd_var_nm,
                 str_t_cr bd_ref_nm,
                 str_t_cr ptr_var_nm = "")
{
  using namespace clang::ast_matchers;
// clang-format off
  return
    // ignoringImpCasts(
      memberExpr(
        isArrow(),
        member(
          hasName("next")
        )
       ,hasDescendant(
          mk_ptr_matcher(bd_var_nm,bd_ref_nm,ptr_var_nm)
        )// hasDescendant
      )//memberExpr
    // ) // ignoringImpCasts
  ;
// clang-format on
} // match_arrow_next

/**\brief Counts 'public:' declarations in C++ structs and classes,
accumulates count in map public_count_.
*/
struct count_public : public callback_t {
  using map_t = std::map<string_t, uint32_t>;
  using map_it = map_t::iterator;

  /**\brief "public:"
  Match a "public:" declaration in a C++ class or struct.
  */
  auto matcher()
  {
    using namespace clang::ast_matchers;
    // clang-format off
    return
    accessSpecDecl(
      isPublic(),
      hasAncestor(
        cxxRecordDecl().bind("crd")
      )
    ).bind("asd")
    ;
    // clang-format on
  } // matcher()

  void run(result_t const & result) override
  {
    using namespace clang;
    AccessSpecDecl const * asd = result.Nodes.getNodeAs<AccessSpecDecl>("asd");
    CXXRecordDecl const * crd = result.Nodes.getNodeAs<CXXRecordDecl>("crd");
    clang::SourceManager & src_manager(
        const_cast<clang::SourceManager &>(result.Context->getSourceManager()));
    if(asd && crd) {
      string_t const struct_name = crd->getNameAsString();
      map_it it = public_count_.find(struct_name);
      if(it != public_count_.end()) {
        public_count_[struct_name]++;
      }
      else {
        public_count_[struct_name] = 1;
      }
      if(verbose_){
        std::cout << "Found public declaration at "
          << sourceRangeAsString(asd->getSourceRange(), &src_manager)
          << " in struct '" << struct_name << "'\n";
      }
    }
    else {
      check_ptr(asd, "asd");
      check_ptr(crd, "crd");
    }
    return;
  }  // run

  map_t public_count_;
  bool verbose_ = false;
};  // count_public

} // corct::

#endif // include guard


// End of file
