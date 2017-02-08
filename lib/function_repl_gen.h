// replacement_generator.h
// T. M. Kelley
// Oct 11, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#ifndef FUNCTION_REPL_GEN_H
#define FUNCTION_REPL_GEN_H

#include "types.h"
#include "utilities.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Tooling.h"

namespace corct {
/** \brief Base class for simple function refactorizations.

It supports generating a set of matchers for named targets (one matcher
per target. It maintains state like a pointer to a Clang Replacements
data structure. It maintains a string that indicates some kind of new
text, such as a new parameter text or new argument text. Finally, it has a dry
run indicator.
*/
template <typename traits_t>
class function_replacement_generator
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  // types and constants
  using matcher_t = typename traits_t::matcher_t;
  using matchers_t = std::vector<matcher_t>;
  using replacements_t = clang::tooling::Replacements;

  // interface
  /** \brief Callback when a matcher finds a match. */
  virtual void run(const result_t & result) override = 0;

  /**\brief Generate an AST matcher for target function. */
  virtual matcher_t mk_matcher(str_t_cr target) const = 0;

  /** \brief Get a set of matchers to which derived class will respond. */
  matchers_t matchers() const
  {
    matchers_t ms;
    for(auto const & t : targets_) {
      ms.push_back(mk_matcher(t));
    }
    return ms;
  }

  function_replacement_generator(replacements_t & reps,
                                 vec_str const & targets,
                                 str_t_cr new_str,
                                 bool const dry_run)
      : reps_(reps), targets_(targets), new_str_(new_str), dry_run_(dry_run)
  {}

  virtual ~function_replacement_generator() {}

  replacements_t const & get_replacements() const { return reps_;}

  // state
protected:
  replacements_t & reps_;
  vec_str const & targets_; //!< function targets (used in function matchers)
  str_t_cr new_str_;
  bool const dry_run_;
};  // replacement_generator

}  // corct

#endif // include guard
// End of file
