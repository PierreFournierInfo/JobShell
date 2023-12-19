#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include "command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "prompt.h"
#include "command_executor.h"


int pipeLimitedTwo(char*, char*);
int killProject(char ** c);

#endif 