// dump_things.h
// T. M. Kelley
// Sep 16, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved


#ifndef DUMP_THINGS_H
#define DUMP_THINGS_H

#include "types.h"
#include "clang/AST/Type.h"
#include "clang/AST/AST.h"

namespace corct
{

/* These functions closely follow the Clang 3.9.0 ASTDumper class. They match the
style of output used elsewhere in Clang AST: the filename or line nubmer
is not mentioned if it hasn't changed since the last one. */

/**brief dump a declaration to stdout.
\param D: declaration
\param SM: SourceManager
\param tabs: current indentation
  */
void dumpDecl(
  const clang::Decl *d,
  clang::SourceManager const * SM,
  string_t const tabs = "");

/**\brief Dump a SourceLocation to stdout.
\param Loc: source location
\param SM: SourceManager
\param tabs: current indentation
*/
void dumpLocation(
  clang::SourceLocation loc,
  clang::SourceManager const * SM,
  string_t const tabs = "");

/** \brief Dump a SourceRange to stdout.
\param R: source range
\param SM: SourceManager
\param tabs: current indentation
*/
void dumpSourceRange(
  clang::SourceRange r,
  clang::SourceManager const * SM,
  string_t const tabs = "");

/**\brief Dump SourceRange to a string.
\param r: source range
\param SM: Source Manager */
string_t
sourceRangeAsString(
  clang::SourceRange r,
  clang::SourceManager const * sm);

/**\brief Dump SourceLocation to a string.
\param loc: source location
\param SM: Source Manager */
string_t
locationAsString(
  clang::SourceLocation loc,
  clang::SourceManager const * SM);

} // corct::
#endif // include guard


// End of file
