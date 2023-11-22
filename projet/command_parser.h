
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdbool.h>
extern 
bool is_internal_command(const char *command);
void execute_internal_command(const char *command);

#endif // COMMAND_PARSER_H
