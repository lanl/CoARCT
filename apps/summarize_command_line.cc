// summarize_command_line.cc
// T. M. Kelley
// (c) Copyright 2017 LANSLLC, all rights reserved

#include "summarize_command_line.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

namespace corct {
void
print_header(std::ostream & o, corct::str_t_cr ex_name, corct::str_t_cr ovvw)
{
  o << "{\"executable\" : \"" << ex_name << "\","
    << "\n\"overview\": \"" << ovvw << "\","
    << "\n\"options\": [\n";
  return;
}

void
print_footer(std::ostream & o)
{
  o << "]\n"
    << "}";
  return;
}

template <class set>
bool
in_set(set & s, typename set::value_type & val)
{
  return s.end() != s.find(val);
}

corct::string_t
escape(corct::str_t_cr in_s)
{
  corct::string_t s;
  std::set<char> e_chars = {'\"'};
  for(char c : in_s) {
    if(in_set(e_chars, c)) { s += "\\"; }
    s += c;
  }
  return s;
}

std::map<string_t, string_t> builtin_opts = {{"extra-arg", "<string>"},
                                             {"extra-arg-before", "<string>"},
                                             {"p", "<string>"}};

bool
is_builtin(string_t const & key)
{
  return builtin_opts.end() != builtin_opts.find(key);
}  // is_builtin

/* TODO: we don't have a good way to access the description for LLVM
 * "builtin" options. For right now, this is just broken. */
void
print_option(std::ostream & s, llvm::cl::Option & o)
{
  // std::cout << "\n\nOption info: ";
  // o.printOptionInfo(30);
  // std::cout << "\nOption Value: ";
  // o.printOptionValue(o.getOptionWidth(),false);
  // std::cout << "\n---------------------------------\n";
  string_t key(o.ArgStr.str());
  s << " {\"option\" : {\n"
    << "    \"name\" : \"" << key << "\",\n";
  if(!o.ValueStr.str().empty()) {
    s << "    \"value\" : \"" << o.ValueStr.str() << "\",\n";
  }
  else if(is_builtin(key)) {
    s << "    \"value\" : \"" << builtin_opts[key] << "\",\n";
  }
  else {
    s << "    \"value\" : \"NONENADAZILCH\",\n";
  }
  s << "    \"desc\" : \"" << escape(o.HelpStr.str()) << "\"}}";
}

void
summarize_command_line(corct::str_t_cr command_name, corct::str_t_cr ovvw)
{
  using namespace llvm::cl;
  using namespace corct;
  /* This list is arbitrary--it's a guess at things that won't interest most
     end users. The exception is 'help'; that conflicts with 'help' in
     Python's argparser. Sigh. */
  set_str excluded_opts = {"opt-bisect-limit",
                           "help",
                           "help-hidden",
                           "xp",
                           "view-background",
                           "pass-remarks-analysis",
                           "pass-remarks-missed",
                           "stats",
                           "as-secure-log-file-name",
                           "rng-seed",
                           "pass-remarks",
                           "asm-macro-max-nesting-depth",
                           "stats-json",
                           "track-memory",
                           "static-func-full-module-prefix",
                           "info-output-file"};
  std::stringstream fname;
  fname << command_name << ".json";
  std::ofstream s(fname.str());
  if(!s) {
    std::cerr << "Cannot open output file '" << fname.str() << "'\n";
    return;
  }
  std::cout << "Generating command line summary in '" << fname.str() << "'\n";
  print_header(s, command_name, ovvw);
  bool after_the_first(false);
  llvm::StringMap<Option *> & opt_map(getRegisteredOptions());
  for(auto & v : opt_map) {
    string_t key(v.getKey().str());
    if(in_set(excluded_opts, key)) { continue; }
    Option & o(*v.getValue());
    if(after_the_first) { s << ",\n"; }
    print_option(s, o);
    after_the_first = true;
  }
  print_footer(s);
  return;
}

}  // namespace corct

// End of file
