// summarize_command_line.cc
// T. M. Kelley
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "summarize_command_line.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

namespace corct{
void print_header(std::ostream & o,corct::str_t_cr ex_name){
  o << "{\"executable\" : \"" << ex_name << "\","
    << "\n\"options\": [\n";
  return;
}

void print_footer(std::ostream & o){
  o << "]\n"
    << "}";
  return;
}

template <class set>
bool in_set(set&s,typename set::value_type & val){
  return s.end() != s.find(val);
}

corct::string_t escape(corct::str_t_cr in_s){
  corct::string_t s;
  std::set<char> e_chars = {'\"'};
  for(char c : in_s){
    if(in_set(e_chars,c)){
      s += "\\";
    }
    s += c;
  }
  return s;
}

void print_option(std::ostream & s,llvm::cl::Option & o){
  s << " {\"option\" : {\n"
    << "    \"name\" : \"" << o.ArgStr.str() << "\",\n"
    << "    \"desc\" : \"" << escape(o.HelpStr.str()) << "\"}}"
    ;
}

void summarize_command_line(corct::str_t_cr command_name){
  using namespace llvm::cl;
  using namespace corct;
  set_str excluded_opts = {
    "opt-bisect-limit",
    "help-hidden",
    "xp",
    "view-background",
    "pass-remarks-analysis",
    "pass-remarks-missed",
    "stats",
    "as-secure-log-file-name",
    "rng-seed",
    "pass-remarks",
    "asm-macro-max-nesting-depth"
  };
  std::stringstream fname;
  fname << command_name << ".summ";
  std::ofstream s(fname.str());
  if(!s){
    std::cerr << "Cannot open output file '" << fname.str() << "'\n";
    return;
  }
  std::cout << "Generating command line summary in '" << fname.str() << "'\n";
  print_header(s,command_name);
  bool after_the_first(false);
  llvm::StringMap<Option *> & opt_map(getRegisteredOptions());
  for(auto & v : opt_map){
    string_t key(v.getKey().str());
    if(in_set(excluded_opts,key)){
      continue;
    }
    Option & o(*v.getValue());
    if(after_the_first){
      s << ",\n";
    }
    print_option(s,o);
    after_the_first = true;
  }
  print_footer(s);
  return;
}

} // corct::

// End of file
