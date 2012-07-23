#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "string.h"
#include "utils.h"

//==============================================================================
// Main function
//==============================================================================
void print_string(string* str) {
  printf("string size=%zu cap=%zu buf='%s'\n",
	 string_size(str),
	 string_capacity(str),
	 string_cstr(str));
}

void test_string() {
  string* str = string_new();
  print_string(str);
  
  string_append_cstr(str, "asdf");
  print_string(str);

  string_append_cstr(str, ". i like coffee");
  print_string(str);

  string_append_cstr(str, ", too.");
  print_string(str);

  string_append_cstr(str, " yum!");
  print_string(str);
}

void test_utils() {
  const int BUFSIZE = 256;
  char buf[BUFSIZE];

  // Read a line
  printf("Say something> ");
  if(!read_line(stdin, buf, BUFSIZE)) {
    fprintf(stderr, "Error reading input from stdin\n");
    return;
  }
  printf("Read from stdin: %s\n", buf);

  // Read words until we enter 'quit'
  printf("Enter words ('quit' to exit)> ");
  while(1) {
    if(read_word(stdin, buf, BUFSIZE)) {
      printf("read_word() got: \"%s\"\n", buf);
    }
    else {
      printf("read_word() failed\n");
    }
    downcase(buf);
    if(strcmp(buf, "quit") == 0) {
      break;
    }
  }
}

int main(int argc, char** argv) {
  printf("Hi there\n");
  test_string();
  test_utils();
  printf("Bye!");
  return 0;
}
