// template_var_matchers.h
// Nov 15, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#pragma once

#include "dump_things.h"
#include "make_replacement.h"
#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

namespace corct {

/* Note on matching template variables: in Clang 5, the code was three matchers
 * simpler:
 *
    varDecl(
      hasDeclaration(
        classTemplateSpecializationDecl(
           matchesName(template_name)
        ).bind("mtvm_classDecl") // classTemplateSpecializationDecl
      ) // hasType
    ).bind("mtvm_varDecl"); // varDecl
 *
 * Under Clang 5, this matched down to the desugared type. So if you had a case
 * like this:
     template <class T> class Aardvarks{};
     using Cardvarks = Aardvarks<int>;
     Cardvarks a;
 * it would match a. This changed in Clang 6. A comment appears in the
 * documentation for hasDeclaration that makes it clear that hasType will match
 * the sugared type. I've followed that comment to preserve the behavior of
 * templ_var_matcher: thus the hasUnqualifiedDesugaredType and its helpers.
 */

// clang-format off
clang::ast_matchers::DeclarationMatcher
mk_templ_var_matcher(str_t_cr template_name){
  using namespace clang::ast_matchers;
  return
    varDecl(
      hasType(
        hasUnqualifiedDesugaredType(
          recordType(
            hasDeclaration(
              classTemplateSpecializationDecl(
                 matchesName(template_name)
              ).bind("mtvm_classDecl") // classTemplateSpecializationDecl
            ) // hasDeclaration
          ) // recordType
        ) // hasUnqualifiedDesugaredType
      ) // hasType
    ).bind("mtvm_varDecl"); // varDecl
} // mk_templ_var_matcher

auto
mk_templ_var_matcher(str_t_cr template_name, str_t_cr namespace_name)
{
  using namespace clang::ast_matchers;
  return
    varDecl(
      hasType(
        hasUnqualifiedDesugaredType(
          recordType(
            hasDeclaration(
              classTemplateSpecializationDecl(
                matchesName(template_name)
               ,hasDeclContext(
                  namespaceDecl(
                    hasName(namespace_name)
                  ) // namespaceDecl
                ) // hasDeclContext
              ).bind("mtvm_classDecl") // classTemplateSpecializationDecl
            ) // hasDeclaration
          ) // recordType
        ) // hasUnqualifiedDesugaredType
      ) // hasType
    ).bind("mtvm_varDecl"); // varDecl
} // mk_templ_var_matcher
// clang-format on

/**\brief Find instances of a template variable and record the names of the
 * template arguments. */
struct template_var_reporter : public callback_t {
  using vec_strs_t = std::vector<string_t>;
  using map_args_t =
      std::map<string_t /*var name*/, vec_strs_t /*template args*/>;
  using matcher_t = clang::ast_matchers::DeclarationMatcher;
  using matchers_t = std::vector<matcher_t>;

  matchers_t matchers() const
  {
    matchers_t ms;
    if(!namespace_name_.empty()) {
      ms.push_back(mk_templ_var_matcher(template_name_, namespace_name_));
    }
    else {
      ms.push_back(mk_templ_var_matcher(template_name_));
    }
    return ms;
  }  // matchers

  virtual void run(const result_t & result) override
  {
    using namespace clang;
    // ASTContext & ctx(*(result.Context));
    ClassTemplateSpecializationDecl const * c_decl =
        result.Nodes.getNodeAs<ClassTemplateSpecializationDecl>(
            "mtvm_classDecl");
    VarDecl const * v_decl = result.Nodes.getNodeAs<VarDecl>("mtvm_varDecl");
    if(c_decl && v_decl) {
      string_t var_name = v_decl->getQualifiedNameAsString();
      auto & args_list(c_decl->getTemplateArgs());
      uint32_t n_args = args_list.size();
      vec_strs_t arg_names;
      for(uint32_t i = 0; i < n_args; ++i) {
        TemplateArgument const & ta(args_list.get(i));
        bool is_it_a_pack(ta.isPackExpansion());
        if(is_it_a_pack) {
          // hmmm...
          printf("%s:%i Found a template pack arg, not sure what to do\n",
                 __FUNCTION__, __LINE__);
        }
        else {
          QualType qtype(ta.getAsType());
          string_t qtype_name(qtype.getAsString());
          arg_names.push_back(qtype_name);
        }
      }  // for(arg:args)
      args_[var_name] = std::move(arg_names);
    }
    else {
      check_ptr(c_decl, "classTemplateSpecializationDecl");
      check_ptr(v_decl, "varDecl");
    }
    return;
  }  // run

  template_var_reporter(str_t_cr template_name, str_t_cr namespace_name = "")
      : template_name_(template_name), namespace_name_(namespace_name)
  {
  }

  map_args_t args_;
  string_t template_name_;
  string_t namespace_name_;
};  // template_var_reporter

}  // corct::

// End of file
