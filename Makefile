SHELL   = /bin/sh
CC      = gcc
CFLAGS  = -c -std=c99 -Wall -Isrc -Ilib -Werror
SOURCES = src/program_options.c src/webserver.c src/sockets.c
OBJECTS = $(SOURCES:.c=.o)
TESTS   = tests/test_program_options.h tests/test_sockets.h

all: $(OBJECTS) bin/webserver bin/run_tests

# Object file dependencies
src/program_options.o: src/program_options.h
src/sockets.o: src/sockets.h
src/webserver.o: src/webserver.h
src/webserver_main.o: src/webserver_main.c src/program_options.h src/webserver.h
tests/run_tests.o: $(SOURCES) $(TESTS)

# Executables
bin/webserver: $(OBJECTS) src/webserver_main.o
	#
	#===== Building bin/webserver =====
	$(CC) $(OBJECTS) src/webserver_main.o -o bin/webserver

bin/run_tests: $(OBJECTS) tests/run_tests.o
	#
	#===== Building bin/run_tests =====
	[ -e bin ] || mkdir bin
	$(CC) $(OBJECTS) tests/run_tests.o -o bin/run_tests

# Cleaning
clean:
	find . -name "*.o" -exec rm -fv {} \;
	rm -vf bin/*
