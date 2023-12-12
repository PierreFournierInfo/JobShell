CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE
LDFLAGS = -L. -lcommand_parser -lreadline

TARGET = jsh
OBJS = main.o prompt.o job_manager.o redirection.o command_parser.o command_executor.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c command_parser.h command_executor.h redirection.h prompt.h
prompt.o: prompt.c prompt.h
job_manager.o: job_manager.c job_manager.h
redirection.o: redirection.c redirection.h
command_parser.o: command_parser.c command_parser.h
command_executor.o: command_executor.c command_executor.h

clean:
	rm -f $(TARGET) $(OBJS)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

.PHONY: all clean valgrind
