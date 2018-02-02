// function_common.h
// (c) Copyright 2018 LANSLLC, all rights reserved

#pragma once

#include "types.h"

// forward declarations
#include <iosfwd>
namespace clang{
  class QualType;
  class ParmVarDecl;
  class FunctionDecl;
  class SourceManager;
  class CXXRecordDecl;
  class CXXMethodDecl;
}

namespace corct{

void
print_type_details(clang::QualType const &qt, string_t &tabs, std::ostream &o);

/**\brief Write information about a parameter declaration */
void
print_parameter_decl_details(clang::ParmVarDecl const * p_decl,
                             clang::SourceManager & sm,
                             string_t & tabs,
                             std::ostream & o);

void
print_templated_kind(clang::FunctionDecl::TemplatedKind const & tk,
                     str_t_cr tabs,
                     std::ostream & o);

void
print_class_decl_details(clang::CXXRecordDecl const * c_decl,
                         string_t &tabs,
                         std::ostream & o);

void
print_method_decl_details(clang::CXXMethodDecl const * m_decl,
                          string_t &tabs,
                          std::ostream & o);

/**\brief Write out information about a function declaration */
void
print_function_decl_details(clang::FunctionDecl const * f_decl,
                           clang::SourceManager & sm,
                           std::ostream & o);

} // corct::

// End of file
