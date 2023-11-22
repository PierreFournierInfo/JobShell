
#ifndef PROMPT_H
#define PROMPT_H

#include "command_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

char* display();
char** separerParEspaces(const char* chaine);
char* update_prompt();
int prompt();
char** diviserEnMots(const char* chaine);
void libererTableauMots(char** tableauMots);

#endif // COMMAND_PARSER_H
