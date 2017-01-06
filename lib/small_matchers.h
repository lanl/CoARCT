// small_matchers.h
// T. M. Kelley
// Dec 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef SMALL_MATCHERS_H
#define SMALL_MATCHERS_H

#include "types.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Core/Replacement.h"

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
}

} // corct::

#endif // include guard


// End of file
