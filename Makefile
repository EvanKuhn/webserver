SHELL   = /bin/sh
CC      = gcc
CFLAGS  = -c -std=c99 -Wall -Isrc -Ilib
SOURCES = src/program_options.c
OBJECTS = $(SOURCES:.c=.o)
TESTS   = tests/test_program_options.h

all: $(OBJECTS) test bin/webserver
test: $(OBJECTS) bin/run_tests

# Object file dependencies
src/main.o: src/main.c src/program_options.h src/program_options.c
tests/run_tests.o: $(SOURCES) $(TESTS)

# Executables
bin/webserver: src/webserver.o
	$(CC) $(OBJECTS) src/webserver.o -o bin/webserver

bin/run_tests: $(OBJECTS) tests/run_tests.o
	[ -e bin ] || mkdir bin
	$(CC) $(OBJECTS) tests/run_tests.o -o bin/run_tests

# Cleaning
clean:
	find . -name "*.o" -exec rm -fv {} \;
	rm -vf bin/*
