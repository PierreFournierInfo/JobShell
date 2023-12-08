
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "job_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

extern int valeur_de_retour;
void execute_internal_command(const char *command);

#endif // COMMAND_PARSER_H
