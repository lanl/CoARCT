// replacement_generator.h
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
per target). It maintains state like a pointer to a Clang Replacements
data structure. It maintains a string that indicates some kind of new
text, such as a new parameter text or new argument text. Finally, it has a dry
run indicator.

The interface distinguishes between function targets and method targets. While
both canbe handled by the same callback, they require different matchers. This
actually allows the callback to distinguish between methods and free functions
with the same name.
*/
template <typename traits_t>
class function_replacement_generator
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  // types and constants
  using matcher_t = typename traits_t::matcher_t;
  using matchers_t = std::vector<matcher_t>;

  // interface
  /** \brief Callback when a matcher finds a match. */
  virtual void run(const result_t & result) override = 0;

  /**\brief Generate an AST matcher for target function. */
  virtual matcher_t mk_fn_matcher(str_t_cr target) const = 0;

  /**\brief Generate an AST matcher for target function. */
  virtual matcher_t mk_mthd_matcher(str_t_cr target) const = 0;

  /** \brief Get a set of function matchers to which derived class will respond.
   */
  matchers_t fn_matchers() const
  {
    matchers_t ms;
    for(auto const & t : fn_targets_) {
      ms.push_back(mk_fn_matcher(t));
    }
    return ms;
  }

  /** \brief Get a set of method matchers to which derived class will respond.
   */
  matchers_t mthd_matchers() const
  {
    matchers_t ms;
    for(auto const & t : mthd_targets_) {
      ms.push_back(mk_mthd_matcher(t));
    }
    return ms;
  }

  function_replacement_generator(replacements_map_t & rep_map,
                                 vec_str const & fn_targets,
                                 str_t_cr new_str,
                                 bool const dry_run)
      : rep_map_(rep_map), fn_targets_(fn_targets), new_str_(new_str), dry_run_(dry_run)
  {}

  function_replacement_generator(replacements_map_t & rep_map_,
                                 vec_str const & fn_targets,
                                 vec_str const & mthd_targets,
                                 str_t_cr new_str,
                                 bool const dry_run)
      : rep_map_(rep_map_),
        fn_targets_(fn_targets),
        mthd_targets_(mthd_targets),
        new_str_(new_str),
        dry_run_(dry_run)
  {}

  virtual ~function_replacement_generator() {}

  replacements_t const & get_replacements(string_t const & fname) const
  {
    return rep_map_[fname];
  }

  replacements_map_t const & get_replacements_map() const { return rep_map_;}

  // state
protected:
  replacements_map_t & rep_map_;
  vec_str const fn_targets_; //!< function targets
  vec_str const mthd_targets_; //!< method targets
  str_t_cr new_str_;
  bool const dry_run_;
};  // replacement_generator

}  // corct

#endif // include guard
// End of file
