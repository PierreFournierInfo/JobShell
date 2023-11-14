CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -L. -lcommand_parser

all: jsh

jsh: main.o libcommand_parser.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c command_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

libcommand_parser.a: command_parser.o
	ar rcs $@ $^

command_parser.o: command_parser.c command_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a jsh

.PHONY: all clean
