#ifndef PIPE_H
#define PIPE_H
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

#define PERMISSIONS 0644

bool redirection_verify_pipe(char* tableau);
void tokenizer(char *token[], char *s, char *delimParameter, int *total);
void command_pipe(char *command);

#endif 