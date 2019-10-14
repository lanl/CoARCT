// summarize_command_line.h
// T. M. Kelley
// Apr 25, 2017
// (c) Copyright 2017 LANSLLC, all rights reserved

#ifndef SUMMARIZE_COMMAND_LINE_H
#define SUMMARIZE_COMMAND_LINE_H

#include "types.h"

namespace corct {

/**\brief Print out useful command line parameters to a JSON file. */
void
summarize_command_line(corct::str_t_cr command_name, corct::str_t_cr overview);

}  // namespace corct
#endif  // include guard

// End of file
