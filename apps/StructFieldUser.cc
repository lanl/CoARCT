
// StructFieldUser.cc
// T. M. Kelley
// Oct. 25, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

/* Find which functions use which fields. */

//#include "dump_things.h"
//#include "here.h"
//#include "make_replacement.h"
//#include "utilities.h"
//#include "struct_field_user.h"
//// Declares clang::SyntaxOnlyAction.
//#include "clang/Frontend/FrontendActions.h"
//#include "clang/Tooling/CommonOptionsParser.h"
//#include "LessAnnoyingOptionsParser.h"
//#include "clang/Tooling/Tooling.h"
//#include "clang/Tooling/Refactoring.h"
//#include "clang/Tooling/Core/Replacement.h"
//#include "clang/Basic/LangOptions.h"
//// Declares llvm::cl::extrahelp.
//#include "llvm/Support/CommandLine.h"
//#include "clang/ASTMatchers/ASTMatchers.h"
//#include "clang/ASTMatchers/ASTMatchFinder.h"
//#include "clang/AST/ASTContext.h"
//#include "clang/AST/Type.h"
//#include <iostream>
//#include <vector>
//#include <set>
//
//using namespace clang::tooling;
//using namespace llvm;
//
//static llvm::cl::OptionCategory LoopOpts("Loop options");
//
//const char * addl_help = "Detect certain while loops";
//
//static llvm::cl::OptionCategory SFUOpts("Common options for struct-field-use");
//
//static
//cl::opt<std::string> target_struct_string("tf",
//  cl::desc("target struct(s) to modify, separated by commas if nec. E.g. -tf=\"cell_t,bas_t,region_t\""),
//  cl::value_desc("target-struct-string"),cl::cat(SFUOpts));
//
///** Print out in various possibly useful ways. */
//template <typename MapOMapOSet>
//void print_fields(MapOMapOSet const & m)
//{
//  using corct::string_t;
//  corct::string_t tabs("");
//  for(auto map_it : m)
//  {
//    string_t const s_name = (map_it.first);
//    for(auto mm_it : (map_it.second))
//    {
//      string_t const f_name =  mm_it.first;
//      // tabs = corct::add_tab(tabs);
//      for(auto membr : mm_it.second)
//      {
//        std::cout << tabs << f_name << " " << s_name << " " << membr << "\n";
//      }
//      // tabs = corct::remove_tab(tabs);
//    }
//  }
//  return;
//}
//
//int main(int argc, const char **argv) {
//  using corct::split;
//  using corct::string_t;
//  LessAnnoyingOptionsParser opt_prs(argc, argv, LoopOpts,addl_help);
//
//  RefactoringTool Tool(opt_prs.getCompilations(),opt_prs.getSourcePathList());
//
//  corct::vec_str targ_fns(split(target_struct_string,','));
//
//  corct::struct_field_user s_finder(targ_fns);
//
//  clang::ast_matchers::MatchFinder finder;
//
//  corct::struct_field_user::matchers_t field_matchers = s_finder.matchers();
//  for(auto m:field_matchers){
//    finder.addMatcher(m, &s_finder);
//  }
//  Tool.run(newFrontendActionFactory(&finder).get());
//
//  // using struct_f_m_map_t = corct::struct_field_user::struct_f_m_map_t;
//  // using func_mem_map_t = corct::struct_field_user::func_mem_map_t;
//  std::cout << "Fields written:\n";
//  print_fields(s_finder.lhs_uses_);
//  std::cout << "Fields accessed, but not written:\n";
//  print_fields(s_finder.non_lhs_uses_);
//
//  return 0;
//} // main

int main(){return 0;}

// End of file
