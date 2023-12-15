
#ifndef PROMPT_H
#define PROMPT_H
#include "command_parser.h"
#include "redirection.h"
#include "command_executor.h"
#include "job_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>


char* display();
char** separerParEspaces(const char* chaine,int* taille);
char* update_prompt();
int prompt();
void freeAll(char** lib,int t);
bool redirection_verif(char* input);
void afficherTableauChar(char **tableau);
#endif // COMMAND_PARSER_H
