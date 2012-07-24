SHELL=/bin/sh
CC=gcc
CFLAGS = -c -std=c99 -Wall -Isrc -Ilib
LDFLAGS=
SOURCES=src/core/string.c tests/test_string.c
OBJECTS=$(SOURCES:.c=.o)

clean:
	find . -name "*.o" -exec rm -fv {} \;
	rm -vf bin/*

bin/test_string: $(OBJECTS)
	$(CC) $(OBJECTS) -o bin/test_string

src/core/string.o: src/core/string.c
	$(CC) $(CFLAGS) $< -o src/core/string.o

tests/test_string.o: tests/test_string.c
	$(CC) $(CFLAGS) $< -o tests/test_string.o


# These directives place the object files in a separate 'build' dir
#clean:
#	rm -rf build
#	rm -vf bin/*

# Compile all source files into object files
#compile: $(OBJECTS)

# Create the 'build' directory for the object file, then compile
#.c.o:
#	mkdir -p build/$(@D)
#	$(CC) $(CFLAGS) $< -o build/$@

#tests: $(OBJECTS) test_string

#test_string:
#	$(CC) build/src/core/string.o build/tests/test_string.o -o bin/test_string


# These directives place the object files next to the source files
#clean:
#	find . -name "*.o" -exec rm -fv {} \;
#	rm -vf bin/*

# Compile all source files into object files
#compile: $(OBJECTS)

#.c.o:
#	$(CC) $(CFLAGS) $< -o $@

#tests: $(OBJECTS) test_string

#test_string:
#	$(CC) src/core/string.o tests/test_string.o -o bin/test_string


