// callsite_lister.cc
// Feb 02, 2018
// (c) Copyright 2018 LANSLLC, all rights reserved

#include "callsite_lister.h"

namespace corct {

const string_t callsite_lister::cs_bd_name = "callsite";
const string_t callsite_lister::mt_bd_name = "m_decl";
const string_t callsite_lister::fn_bd_name = "f_decl";
const string_t callsite_lister::caller_bd_name = "caller";

void
print_call_details(clang::FunctionDecl const * callee,
                   clang::FunctionDecl const * caller,
                   clang::CallExpr const * callsite,
                   clang::SourceManager & sm,
                   std::ostream & o)
{
  string_t tabs = "\t";
  // callee information
  o << std::boolalpha << "target function:\n"
    << tabs << "callee: " << callee->getNameAsString() << "\n"
    << tabs << "callee source range: "
    << fullSourceRangeAsString(callee->getSourceRange(), &sm) << "\n"
    << tabs << "callsite source range: "
    << fullSourceRangeAsString(callsite->getSourceRange(), &sm) << "\n"
    << tabs << "isTemplateInstantiation: " << callee->isTemplateInstantiation()
    << "\n";

  ;
  // caller information
  o << tabs << "caller:" << caller->getNameAsString() << "\n"
    << tabs << "caller source range: "
    << fullSourceRangeAsString(caller->getSourceRange(), &sm)
    << "\n-=--=--=--=--=--=-\n";
  return;
}  // print_short_function_decl_details

}  // namespace corct

// End of file
