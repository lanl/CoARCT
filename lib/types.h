// types.h
// Oct 06, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef TYPES_H
#define TYPES_H

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <string>
#include <vector>

// forward declarations
namespace clang
{
class SourceManager;

namespace tooling
{
class Replacement;
}  // tooling::
}  // clang::

namespace corct
{
using callback_t = clang::ast_matchers::MatchFinder::MatchCallback;
using finder_t = clang::ast_matchers::MatchFinder;
using result_t = clang::ast_matchers::MatchFinder::MatchResult;
using replacement_t = clang::tooling::Replacement;
using string_t = std::string;
using str_t_cr = string_t const &;
using vec_str = std::vector<string_t>;
using vec_repl = std::vector<replacement_t>;
using sm_ptr_t = clang::SourceManager *;
using sm_ref_t = clang::SourceManager &;
using sm_cref_t = clang::SourceManager const &;
using sm_cc_ptr_t = clang::SourceManager const * const;
}  // corct::

#endif  // include guard

// End of file
