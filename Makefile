CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L. -lcommand_parser -lreadline

all: jsh

jsh: main.o prompt.o libcommand_parser.a 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c command_parser.h command_executor.h prompt.h
	$(CC) $(CFLAGS) -c -o $@ $<

libcommand_parser.a: command_parser.o command_executor.o 
	ar rcs $@ $^

command_parser.o: command_parser.c command_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

command_executor.o: command_executor.c command_executor.h
	$(CC) $(CFLAGS) -c -o $@ $<

signal_handler.o: 

prompt.o: prompt.c prompt.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a jsh


valgrind: jsh
	valgrind --leak-check=full ./jsh

.PHONY: all clean
