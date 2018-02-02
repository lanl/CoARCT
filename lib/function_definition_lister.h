// function_definition_lister.h
// Jan 25, 2018

#pragma once

#include "dump_things.h"
#include "function_common.h"
#include "types.h"
#include "utilities.h"

namespace corct {

/** \class FunctionDeclPrinter: Print the name and source range of funtions
 * defined in a translation unit (excluding those defined in a system header.)
 */
struct FunctionDefLister : public callback_t {

  // clang-format off
  auto matcher(){
    using namespace clang::ast_matchers;
    return
    functionDecl(
      isDefinition(),
      unless(isExpansionInSystemHeader())
    ).bind(bd_name_);
  }
  // clang-format on

  void run(result_t const & result) override
  {
    using namespace clang;
    FunctionDecl const * fdecl = result.Nodes.getNodeAs<FunctionDecl>(bd_name_);
    if(fdecl) {
      m_num_funcs++;
      SourceManager & sm(result.Context->getSourceManager());
      print_function_decl_details(fdecl,sm,std::cout);
      std::cout << "-=--=--=--=--=--=-\n";
    }
    else {
      corct::check_ptr(fdecl, "fdecl");
    }
    return;
  }  // run

  explicit FunctionDefLister(str_t_cr bd_name) : bd_name_(bd_name) {}

  size_t m_num_funcs = 0u;

private:
  string_t bd_name_ = "";
};  // FunctionDefLister

}  // namespace corct

// End of file
