// function_common.cc
// Jan 30, 2018
// (c) Copyright 2018 LANSLLC, all rights reserved

#include "function_common.h"
#include "clang/AST/AST.h"
#include "clang/AST/PrettyPrinter.h"
#include "dump_things.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "types.h"
#include "utilities.h"

#include <iostream>

namespace corct {

void
print_type_details(clang::QualType const & qt,
                   string_t & tabs,
                   std::ostream & o)
{
  // name
  o << tabs << qt.getAsString() << "\n";
  // const? volatile? restrict?
  o << tabs << "const: " << qt.isConstQualified() << "\n";
  o << tabs << "volatile: " << qt.isVolatileQualified() << "\n";
  o << tabs << "restrict: " << qt.isRestrictQualified() << "\n";
  clang::QualType qt_nr(qt.getNonReferenceType());
  bool is_ref(qt != qt_nr);
  o << tabs << "reference: " << is_ref << "\n";
  if(is_ref) {
    o << tabs << "type referred to:\n";
    tabs = add_tab(tabs);
    print_type_details(qt_nr, tabs, o);
    tabs = remove_tab(tabs);
  }
  return;
}  // print_type_details

/**\brief Write information about a parameter declaration */
void
print_parameter_decl_details(clang::ParmVarDecl const * p_decl,
                             clang::SourceManager & sm,
                             string_t & tabs,
                             std::ostream & o)
{
  // name
  o << tabs << "name: " << p_decl->getNameAsString() << "\n";
  // source range
  o << tabs << "source range: "
    << fullSourceRangeAsString(p_decl->getSourceRange(), &sm) << "\n";
  // type
  clang::QualType qt(p_decl->getOriginalType());
  o << tabs << "type:\n";
  tabs = add_tab(tabs);
  print_type_details(qt, tabs, o);
  tabs = remove_tab(tabs);
  // default argument
  bool const hasDefault(p_decl->hasDefaultArg());
  o << tabs << "has default: " << hasDefault << "\n";
  if(hasDefault) {
    clang::SourceRange sr(p_decl->getDefaultArgRange());
    o << tabs << "default source range: " << fullSourceRangeAsString(sr, &sm)
      << "\n";
  }
  return;
}  // print_parameter_decl_details

void
print_templated_kind(clang::FunctionDecl::TemplatedKind const & tk,
                     str_t_cr tabs,
                     std::ostream & o)
{
  switch(tk) {
    case clang::FunctionDecl::TemplatedKind::TK_NonTemplate:
      o << tabs << "template kind: NonTemplate"
        << "\n";
      break;
    case clang::FunctionDecl::TemplatedKind::TK_FunctionTemplate:
      o << tabs << "template kind: FunctionTemplate"
        << "\n";
      break;
    case clang::FunctionDecl::TemplatedKind::TK_MemberSpecialization:
      o << tabs << "template kind: MemberSpecialization"
        << "\n";
      break;
    case clang::FunctionDecl::TemplatedKind::TK_FunctionTemplateSpecialization:
      o << tabs << "template kind: FunctionTemplateSpecialization"
        << "\n";
      break;
    case clang::FunctionDecl::TemplatedKind::
        TK_DependentFunctionTemplateSpecialization:
      o << tabs << "template kind: DependentFunctionTemplateSpecialization"
        << "\n";
      break;
  }
}  // print_templated_kind

void
print_class_decl_details(clang::CXXRecordDecl const * c_decl,
                         string_t & tabs,
                         std::ostream & o)
{
  o << tabs << "name: " << c_decl->getNameAsString() << "\n";
  return;
}  // print_class_decl_details

void
print_method_decl_details(clang::CXXMethodDecl const * m_decl,
                          string_t & tabs,
                          std::ostream & o)
{
  o << tabs << "class method:\n";
  tabs = add_tab(tabs);
  clang::CXXRecordDecl const * p_class = m_decl->getParent();
  o << tabs << "class:\n";
  tabs = add_tab(tabs);
  print_class_decl_details(p_class, tabs, o);
  tabs = remove_tab(tabs);
  o << tabs << "const: " << m_decl->isConst() << "\n";
  o << tabs << "volatile: " << m_decl->isVolatile() << "\n";
  o << tabs << "virtual: " << m_decl->isVirtual() << "\n";
  tabs = remove_tab(tabs);
  return;
}  // print_method_decl_details

/**\brief Write out information about a function declaration */
void
print_function_decl_details(clang::FunctionDecl const * f_decl,
                            clang::SourceManager & sm,
                            std::ostream & o)
{
  string_t tabs = "";
  o << std::boolalpha << "Function:"
    << "\n";
  tabs = add_tab(tabs);
  // Information:
  // name
  o << tabs << "name: " << f_decl->getNameAsString() << "\n";
  string_t sstr;
  llvm::raw_string_ostream rs_str(sstr);
  f_decl->getNameForDiagnostic(
      rs_str, clang::PrintingPolicy(clang::LangOptions()), true);
  o << tabs << "diagnostic name: " << rs_str.str() << "\n";
  // source range
  o << tabs << "source range: "
    << fullSourceRangeAsString(f_decl->getSourceRange(), &sm) << "\n";
  // context (namespace? class/struct?)
  // Is it a template function?
  print_templated_kind(f_decl->getTemplatedKind(), tabs, o);
  o << tabs << "isTemplateInstantiation: " << f_decl->isTemplateInstantiation()
    << "\n";
  clang::CXXMethodDecl const * m_decl =
      llvm::dyn_cast<clang::CXXMethodDecl>(f_decl);
  bool is_class_method(nullptr != m_decl);
  o << tabs << "is class method: " << is_class_method << "\n";
  if(is_class_method) { print_method_decl_details(m_decl, tabs, o); }
  // qualifiers (const, mutable, static, inline ...)
  o << tabs << "inline: " << f_decl->isInlineSpecified() << "\n";

  // What is the return type
  clang::QualType ret_type(f_decl->getReturnType());
  o << tabs << "return type:\n";
  tabs = add_tab(tabs);
  print_type_details(ret_type, tabs, o);
  tabs = remove_tab(tabs);

  // How many parameters are there?
  uint32_t const n_params(f_decl->getNumParams());
  o << tabs << "num params: " << n_params << "\n";
  for(uint32_t i = 0; i < n_params; ++i) {
    o << tabs << "param " << std::to_string(i) << ":\n";
    tabs = add_tab(tabs);
    clang::ParmVarDecl const * p_param(f_decl->getParamDecl(i));
    print_parameter_decl_details(p_param, sm, tabs, o);
    tabs = remove_tab(tabs);
  }

  return;
}  // dump_function_decl_details

}  // namespace corct

// End of file

// End of file
