
#ifndef REDIRECTION_H
#define REDIRECTION_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include "command_executor.h"
#include "command_parser.h"
#include "redirection.h"
#include <errno.h>

#define MAX_COMMANDS 100

void command_r(char** res,int taille);
size_t tailleTableauChar(char **tableau);
bool verif_fic(const char *fic);
void redirect(char** res, int* pipefd,int taille);
char **before_com(char **res) ;

bool redirection_verify_pipe(char** tableau, int l);
void tokenizer(char *token[], char *s, char *delimParameter, int *total);
void command_pipe(char *command);
#endif 