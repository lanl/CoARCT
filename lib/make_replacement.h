// make_replacement.h
// T. M. Kelley
// Aug 19, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef MAKE_REPLACEMENT_H
#define MAKE_REPLACEMENT_H

#include "types.h"
// #include "clang/Tooling/Core/Replacement.h"
#include "clang/Basic/LangOptions.h"
// #include "clang/Basic/SourceManager.h"
// #include "clang/Lex/Lexer.h"

namespace corct{
/** \brief Replace indicated source range with given text.

  You can initialize the StringRef with a std::string.

  This version replaces the indicated source range, through the
  end of the last token in the source range. It uses code from
  Scott Pakin to compute the latter.

*/
inline
replacement_t
replace_source_range(
  clang::SourceManager const & sm,
  clang::SourceRange const & range,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  SourceLocation start_loc = range.getBegin();     // Start of range
  SourceLocation start_end_loc = range.getEnd();   // Start of last token at end of range
  clang::LangOptions lopt;
  SourceLocation end_loc(
    clang::Lexer::getLocForEndOfToken(start_end_loc, 0, sm, lopt));  // True end of range
  unsigned int length = sm.getFileOffset(end_loc) - sm.getFileOffset(start_loc) ;
  return replacement_t(sm, start_loc, length, replacement);
}

/** Replace exactly the indicated source range--this does not
   compute to the end of the last token in the source range.
   Sometimes that's what I want \_:~_/ */
inline
replacement_t
replace_source_range_naive(
  clang::SourceManager const & sm,
  clang::SourceRange const & range,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  SourceLocation start_loc = range.getBegin();     // Start of range
  SourceLocation start_end_loc = range.getEnd();   // Start of last token at end of range
  unsigned int length = sm.getFileOffset(start_end_loc) - sm.getFileOffset(start_loc) ;
  return replacement_t(sm, start_loc, length, replacement);
}

/** Insert replacement string after the source range. */
inline
replacement_t
append_source_range(
  clang::SourceManager const & sm,
  clang::SourceRange const & range,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  // SourceLocation start_loc = range.getBegin();     // Start of range
  SourceLocation start_end_loc = range.getEnd();   // Start of last token at end of range
  clang::LangOptions lopt;
  SourceLocation end_loc(
    clang::Lexer::getLocForEndOfToken(start_end_loc, 0, sm, lopt));  // True end of range
  // unsigned int length = sm.getFileOffset(end_loc) - sm.getFileOffset(start_loc) ;
  return replacement_t(sm, end_loc, 0, replacement);
} // append_source_range

/** Insert replacement string after the source range. */
inline
replacement_t
append_source_loc(
  clang::SourceManager const & sm,
  clang::SourceLocation const & start_end_loc,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  // SourceLocation start_loc = range.getBegin();     // Start of range
  // SourceLocation start_end_loc = range.getEnd();   // Start of last token at end of range
  clang::LangOptions lopt;
  SourceLocation end_loc(
    clang::Lexer::getLocForEndOfToken(start_end_loc, 0, sm, lopt));  // True end of range
  // unsigned int length = sm.getFileOffset(end_loc) - sm.getFileOffset(start_loc) ;
  return replacement_t(sm, end_loc, 0, replacement);
} // append_source_loc

/** \brief insert a parameter if it't the only one in the function.

  Known to fail on:
  \code
  void f(void);
  \endcode
   */
inline
replacement_t
insert_only_param(
  clang::SourceManager const & sm,
  clang::SourceLocation const & start,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  clang::LangOptions lopt;
  SourceLocation end_loc(clang::Lexer::getLocForEndOfToken(start, 0, sm, lopt));  // True end of range
  SourceLocation end_plus_one = end_loc.getLocWithOffset(1);
  return replacement_t(sm, end_plus_one, 0, replacement);
} // insert_only_param


/** Insert replacement string before the source range (only uses the start). */
inline
replacement_t
prepend_source_range(
  clang::SourceManager const & sm,
  clang::SourceRange const & range,
  clang::StringRef const replacement)
{
  using clang::SourceLocation;
  SourceLocation start_loc = range.getBegin();     // Start of range
  // SourceLocation start_end_loc = range.getEnd();   // Start of last token at end of range
  // clang::LangOptions lopt;
  // SourceLocation end_loc(
  //   clang::Lexer::getLocForEndOfToken(start_end_loc, 0, sm, lopt));  // True end of range
  // unsigned int length = sm.getFileOffset(end_loc) - sm.getFileOffset(start_loc) ;
  return replacement_t(sm, start_loc, 0, replacement);
} // prepend_source_range

/** Insert replacement string before the source range (only uses the start). */
inline
replacement_t
prepend_source_loc(
  clang::SourceManager const & sm,
  clang::SourceLocation const & start_loc,
  clang::StringRef const replacement)
{
  return replacement_t(sm, start_loc, 0, replacement);
} // prepend_source_range

} // corct::

#endif // include guard


// End of file
