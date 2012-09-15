SHELL   = /bin/sh
CC      = gcc
CFLAGS  = -c -std=c99 -Wall -Isrc -Ilib -Werror
SOURCES = src/http_request.c src/program_options.c src/sockets.c src/status.c src/webserver.c
OBJECTS = $(SOURCES:.c=.o)
TESTS   = tests/test_http_request.h tests/test_program_options.h tests/test_sockets.h

all: $(OBJECTS) bin/webserver bin/run_tests

# Object file dependencies
src/http_request.o: src/http_request.h
src/program_options.o: src/program_options.h
src/sockets.o: src/sockets.h src/status.h
src/status.o: src/status.h
src/webserver.o: src/webserver.h src/sockets.h src/http_request.h
src/webserver_main.o: src/program_options.h src/webserver.h
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
