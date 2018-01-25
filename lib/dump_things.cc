// dump_things.cc
// Sep 16, 2016
// (c) Copyright 2016 LANSLLC, all rights reserved

#include "dump_things.h"
#include "types.h"
#include "utilities.h"
#include <iostream>
#include <sstream>

namespace corct
{
namespace
{
/* These maintain state across calls to dump */
string_t last_fname = "";
uint32_t last_lineno = 0xFFFFFFF;
}  // anonymous::

void clearLocation(){
  last_fname = "";
  last_lineno = 0xFFFFFFFl;
  return;
}

void
dumpLocation(clang::SourceLocation loc,
             clang::SourceManager const * sm,
             string_t const tabs)
{
  if(!sm) {
    HERE("Invalid SourceManager, cannot dumpLocation\n");
    return;
  }
  clang::SourceLocation SpellingLoc = sm->getSpellingLoc(loc);
  clang::PresumedLoc ploc = sm->getPresumedLoc(SpellingLoc);

  if(ploc.isInvalid()) {
    std::cout << tabs << "<invalid sloc>";
    return;
  }

  string_t fname = ploc.getFilename();
  uint32_t const lineno = ploc.getLine();
  uint32_t const colno = ploc.getColumn();
  if(fname != last_fname) {
    std::cout << tabs << fname << ':' << lineno << ':' << colno;
    last_fname = fname;
    last_lineno = lineno;
  } else if(lineno != last_lineno) {
    std::cout << tabs << "line" << ':' << lineno << ':' << colno;
    last_lineno = lineno;
  } else {
    std::cout << tabs << "col" << ':' << colno;
  }
  return;
}  // dumpLocation

// dumpLocation, but to a string
string_t
locationAsString(clang::SourceLocation loc,
                 clang::SourceManager const * const sm)
{
  std::stringstream s;
  if(!sm) {
    s << "Invalid SourceManager, cannot dump Location\n";
    return s.str();
  }
  clang::SourceLocation SpellingLoc = sm->getSpellingLoc(loc);
  clang::PresumedLoc ploc = sm->getPresumedLoc(SpellingLoc);
  if(ploc.isInvalid()) {
    s << "<invalid sloc>";
    return s.str();
  }

  string_t fname = ploc.getFilename();
  uint32_t const lineno = ploc.getLine();
  uint32_t const colno = ploc.getColumn();
  if(fname != last_fname) {
    s << fname << ':' << lineno << ':' << colno;
    last_fname = fname;
    last_lineno = lineno;
  } else if(lineno != last_lineno) {
    s << "line" << ':' << lineno << ':' << colno;
    last_lineno = lineno;
  } else {
    s << "col" << ':' << colno;
  }
  return s.str();
}  // locationAsString

void
dumpSourceRange(clang::SourceRange R,
                clang::SourceManager const * const sm,
                string_t const tabs)
{
  if(!sm) {
    HERE("Invalid SourceManager, cannot dump SourceRange\n");
    return;
  }
  std::cout << tabs << "<";
  dumpLocation(R.getBegin(), sm);
  if(R.getBegin() != R.getEnd()) {
    std::cout << ", ";
    dumpLocation(R.getEnd(), sm);
  }
  std::cout << ">";
  return;
}  // dumpSourceRange

void
dumpFullSourceRange(clang::SourceRange R,
                clang::SourceManager const * const sm,
                string_t const tabs)
{
  last_fname = "";
  last_lineno = 0xFFFFFFF;
  if(!sm) {
    HERE("Invalid SourceManager, cannot dump SourceRange\n");
    return;
  }
  std::cout << tabs << "<";
  dumpLocation(R.getBegin(), sm);
  if(R.getBegin() != R.getEnd()) {
    std::cout << ", ";
    dumpLocation(R.getEnd(), sm);
  }
  std::cout << ">";
  return;
}  // dumpSourceRange

// dumpSourceRange, but to a string
string_t
sourceRangeAsString(clang::SourceRange r, clang::SourceManager const * sm)
{
  if(!sm) {
    return "";
  }
  std::stringstream s;
  s << "<" << locationAsString(r.getBegin(), sm);
  if(r.getBegin() != r.getEnd()) {
    s << ", " << locationAsString(r.getEnd(), sm);
  }
  s << ">";
  return s.str();
}  // sourceRangeAsString

// from clang's ASTDumper:
void
dumpDecl(clang::Decl const * const D,
         clang::SourceManager const * const sm,
         string_t const tabs)
{
  using clang::Decl;
  using clang::NamedDecl;
  using clang::Module;
  using clang::cast;
  using clang::dyn_cast;
  using clang::FunctionDecl;
  // dumpChild([=] {
  if(!D) {
    // ColorScope Color(*this, NullColor);
    std::cout << tabs << "<<<NULL>>>";
    return;
  }
  if(!sm) {
    HERE("Invalid SourceManager, cannot dumpDecl\n");
    return;
  }

  {
    std::cout << tabs << D->getDeclKindName() << "Decl";
  }
  std::cout << tabs << ' ' << (void *)D;
  if(D->getLexicalDeclContext() != D->getDeclContext())
    std::cout << tabs << " parent " << cast<Decl>(D->getDeclContext());
  dumpSourceRange(D->getSourceRange(), sm);
  std::cout << tabs << " <location>";
  dumpLocation(D->getLocation(), sm);
  std::cout << tabs << "</location>";
  if(Module * M = D->getImportedOwningModule())
    std::cout << tabs << " in " << M->getFullModuleName();
  else if(Module * M = D->getLocalOwningModule())
    std::cout << tabs << " in (local) " << M->getFullModuleName();

  if(const NamedDecl * ND = dyn_cast<NamedDecl>(D))
    if(ND->isHidden()) std::cout << tabs << " hidden";
  if(D->isImplicit()) std::cout << tabs << " implicit";
  if(D->isUsed())
    std::cout << tabs << " used";
  else if(D->isThisDeclarationReferenced())
    std::cout << tabs << " referenced";
  if(D->isInvalidDecl()) std::cout << tabs << " invalid";
  if(const clang::FunctionDecl * FD = dyn_cast<FunctionDecl>(D))
    if(FD->isConstexpr()) std::cout << tabs << " constexpr";

  std::cout << "\n";
  return;
}  // dumpDecl

}  // corct::

// End of file
