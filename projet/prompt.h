
#ifndef PROMPT_H
#define PROMPT_H

#include "command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

void display();
void update_prompt();
int prompt();

#endif // COMMAND_PARSER_H
