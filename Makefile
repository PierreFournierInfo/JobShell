CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L. -lcommand_parser -lreadline

all: jsh

jsh: main.o prompt.o job_manager.o libcommand_parser.a 

	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c command_parser.h command_executor.h prompt.h job_manager.h
	$(CC) $(CFLAGS) -c -o $@ $<

libcommand_parser.a: command_parser.o command_executor.o 
	ar rcs $@ $^

command_parser.o: command_parser.c command_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

job_manager.o: job_manager.c job_manager.h
	$(CC) $(CFLAGS) -c -o $@ $<


command_executor.o: command_executor.c command_executor.h
	$(CC) $(CFLAGS) -c -o $@ $<

prompt.o: prompt.c prompt.h
	$(CC) $(CFLAGS) -c -o $@ $<



clean:
	rm -f *.o *.a jsh

valgrind: jsh
	valgrind --leak-check=full ./jsh

.PHONY: all clean
