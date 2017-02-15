// utilities.h
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef UTILITIES_H
#define UTILITIES_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "types.h"
#include <algorithm>  // std::find
#include <cstdio>
#include <iostream>
#include <sstream>

#define print_bool(b) (b ? "true" : "false")

inline void here_func(int line,
                      const char* func_name,
                      const char* message,
                      std::ostream& o = std::cout){
  o << func_name << ":" << line << ": " << message << "\n";
  return;
}
#define HERE(message) here_func(__LINE__, __FUNCTION__, message);

namespace corct
{

inline string_t
add_tab(string_t const s)
{
  return s + '\t';
}

inline string_t remove_tab(string_t const s){
  size_t sz = s.find_last_of('\t');
  return s.substr(0, sz);
}

/** \brief is value val in vector v? */
template <typename vec_t, typename val_t = typename vec_t::value_type>
inline bool in_vec(vec_t const& v, val_t const& val){
  return std::find(v.begin(), v.end(), val) != v.end();
}

/** \brief Split a string s into substrings delimited by delim. */
inline vec_str
split(std::string const & s, char const delim)
{
  std::vector<std::string> tokens;
  std::istringstream ss(s);
  std::string token;
  while(std::getline(ss, token, delim)) {
    tokens.push_back(token);
  }
  return tokens;
}  // split

/**\brief Complain if pointer is invalid.
  \param p: pointer
  \param name: name of thing checked
  \param tabs: indentation
  \return none */
template <typename T>
inline void
check_ptr(T * p, string_t const name, string_t const tabs = "",
  std::ostream & s = std::cout)
{
  if(!p) {
    s << tabs << "Invalid pointer " << name << "\n";
  }
}

/** \brief Strip off initial 'struct ' and possible trailing ' *'

  \param membr: member expression

  Looks through typedefs to the canonical name. For example,
  if a struct is accessed through vector<struct_t>, the
  member typename will be given as 'value_type', whereas
  the canonical name is 'struct_t'.
*/
inline string_t
get_struct_name(clang::MemberExpr const & membr)
{
  clang::Expr const * m_base = membr.getBase();
  string_t const b_name =
      m_base->getType().isCanonical()
          ? m_base->getType().getAsString()
          : m_base->getType().getCanonicalType().getAsString();
  size_t const sz_struct = b_name.find("struct ");
  string_t core_name =
      (sz_struct != string_t::npos) ? b_name.substr(7, b_name.size()) : b_name;
  size_t const sz_star = core_name.find(" *");
  if(sz_star != string_t::npos) {
    core_name = core_name.substr(0, sz_star);
  }
  return core_name;
}

/** \brief Assuming that a SourceLocation is the first interesting
 thing on a line, compute an indentation level. */
inline string_t
infer_indent_level(clang::SourceLocation & loc, clang::SourceManager const & sm)
{
  using clang::SourceLocation;
  using clang::PresumedLoc;
  std::stringstream tabs;
  SourceLocation spellingLoc = sm.getSpellingLoc(loc);
  PresumedLoc ploc = sm.getPresumedLoc(spellingLoc);
  if(ploc.isInvalid()) {
    HERE("<invalid sloc>\n\n");
  }
  else {
    uint32_t const col = ploc.getColumn();
    if(col > 0) {
      for(uint32_t i = 0; i < (col - 1); ++i) {
        tabs << " ";
      }
    }
  }
  return tabs.str();
}  // infer_tab_level

/**\brief True if node is on the LHS of operator=

  Looks through parent nodes and try to find one that is
  a BinaryOperator. If a binary assignment operator parent
  is found, decide if the member expr is equal to its LHS.

  This finds both simple assignment and composite assignment
  operations.
  */
inline bool
is_on_lhs(clang::MemberExpr const * membr, clang::ASTContext & ctx)
{
  using namespace clang;
  string_t const s_name = get_struct_name(*membr);
  string_t const m_name = membr->getMemberDecl()->getNameAsString();
  MemberExpr const & mem(*membr);
  auto parents = ctx.getParents(mem);
  bool on_lhs(false);
  for(auto p : parents) {
    BinaryOperator const * bop = p.get<BinaryOperator>();
    if(bop) {
      if(bop->isAssignmentOp()) {
        Expr const * e((Expr *)membr);
        if(bop->getLHS() == e) {
          on_lhs = true;
        }
      } else if(bop->isCompoundAssignmentOp()) {
        std::cout << "\t LOOK OUT!! Overlooked composite assignment!\n";
      }
    }  // if(bop)
  }    // for(p: parents)
  return on_lhs;
}  // is_on_lhs

/** \brief Is expression 1 a parent of expression 2?

  Recursively search parents of node e2 looking for a match to e1. */
template <
    // typename NodeT1,
    typename NodeT2>
inline bool
is_ancestor_of(clang::BinaryOperator const * e1,
               /*clang::MemberExpr*/ NodeT2 const & e2,
               clang::ASTContext & ctx,
               bool const verbose = false)
{
  using namespace clang;
  bool is_parent(false);
  auto parents = ctx.getParents(e2);
  if(verbose) {
    printf("is_ancestor_of: %lu parents\n", parents.size());
    if(parents.size() > 1) {
      printf("\t\tWARNING!!!! %s:%i Unexpected number of parents %lu!!!\n\n",
             __FUNCTION__, __LINE__, parents.size());
    }
  }
  for(auto p : parents) {
    BinaryOperator const * p_as_nt1 = p.template get<BinaryOperator>();
    if(p_as_nt1) {
      if(p_as_nt1 == e1) {
        is_parent = true;
      }
    } else {
      return is_ancestor_of(e1, p, ctx, verbose);
    }
  }
  return is_parent;
}

/** \brief Is member expression part of an assignment? */
inline bool
is_part_of_assignment(clang::MemberExpr const * membr, clang::ASTContext & ctx)
{
  using namespace clang;
  string_t const s_name = get_struct_name(*membr);
  string_t const m_name = membr->getMemberDecl()->getNameAsString();
  MemberExpr const & mem(*membr);
  auto parents = ctx.getParents(mem);
  bool bop_parent(false);
  for(auto p : parents) {
    BinaryOperator const * bop = p.get<BinaryOperator>();
    if(bop) {
      bop_parent = true;
    }  // if(bop)
  }
  return bop_parent;
}  // is_on_lhs

/** \brief Decide if two VarDecl's are the same underlying variable.

 The idea of comparing canonical declarations is from the official Clang
 ASTMatchers tutorial. */
inline bool
compare_vars(clang::VarDecl const * const v1,
             clang::VarDecl const * const v2,
             string_t tabs,
             string_t const name,
             bool const verbose = true)
{
  bool same = v1 && v2 && v1->getCanonicalDecl() == v2->getCanonicalDecl();
  if(verbose) {
    std::cout << tabs << name << (same ? "true" : "false") << "\n";
    if(!same) {
      tabs = add_tab(tabs);
      std::cout << tabs << "left: " << v1->getCanonicalDecl() << "\n";
      std::cout << tabs << "rght: " << v2->getCanonicalDecl() << "\n";
      tabs = remove_tab(tabs);
    }
  }
  return same;
}  // compare_vars

}  // corct::
#endif  // include guard

// End of file
