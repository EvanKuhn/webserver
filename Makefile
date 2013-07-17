SHELL   = /bin/sh
CC      = gcc
CFLAGS  = -c -std=c99 -Wall -Isrc -Ilib/nu_unit -Werror -g -Wwrite-strings
SOURCES = src/http_enums.c src/http_request.c src/http_response.c src/logging.c \
          src/program_options.c src/sockets.c src/status.c src/std_string.c \
          src/webserver.c src/webserver_config.c src/utils.c
HEADERS = $(SOURCES:.c=.h)
OBJECTS = $(SOURCES:.c=.o)
TESTS   = tests/test_http_enums.h tests/test_http_request.h tests/test_http_response.h \
					tests/test_program_options.h tests/test_sockets.h tests/test_string.h \
					tests/test_utils.h
MKDIRS  = mkdir -p bin/

all: submodules $(OBJECTS) bin/webserver bin/run_tests

# Object file dependencies
src/http_enums.o: src/http_enums.h
src/http_request.o: src/http_request.h src/utils.h
src/logging.o: src/logging.h
src/program_options.o: src/program_options.h src/webserver_config.h
src/sockets.o: src/sockets.h src/status.h
src/status.o: src/status.h
src/std_string.o: src/std_string.h
src/webserver.o: src/webserver.h src/sockets.h src/http_request.h
src/webserver_config.o: src/webserver_config.h
src/webserver_main.o: src/program_options.h src/webserver.h
src/utils.o: src/utils.h
tests/run_tests.o: $(HEADERS) $(SOURCES) $(TESTS) lib/nu_unit/nu_unit.h

submodules:
	git submodule init
	git submodule update

# Executables
bin/webserver: $(OBJECTS) src/webserver_main.o
	#
	#===== Building bin/webserver =====
	$(MKDIRS)
	$(CC) $(OBJECTS) src/webserver_main.o -o bin/webserver

bin/run_tests: $(OBJECTS) tests/run_tests.o
	#
	#===== Building bin/run_tests =====
	$(MKDIRS)
	$(CC) $(OBJECTS) tests/run_tests.o -o bin/run_tests

# Cleaning
clean:
	find . -name "*.o" -exec rm -fv {} \;
	rm -vf bin/*
