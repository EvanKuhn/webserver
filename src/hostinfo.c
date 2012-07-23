#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    puts("Usage: <url>");
    return 1;
  }
  char* hostname = argv[1];
  char ip[100] = {0};
  struct in_addr** addr_list = NULL;
  size_t i=0;
  
  // Look up the host's IP info
  struct hostent* he = gethostbyname(hostname);
  if(!he) {
    herror("gethostbyname");
    return 1;
  }

  printf("Hostname: %s\n", he->h_name);
  printf("Aliases:\n");
  for(i=0; he->h_aliases[i]; ++i) {
    printf("  %s\n", he->h_aliases[i]);
  }
  printf("Type: %i\n", he->h_addrtype);
  printf("Addresses:\n");
  for(i=0; he->h_addr_list[i]; ++i) {
    printf("  %s\n", he->h_addr_list[i]);
  }
  puts(he->h_addr);

	// Cast the h_addr_list to in_addr , since h_addr_list also has the ip address
  // in long format only
	addr_list = (struct in_addr **) he->h_addr_list;
	for(i = 0; addr_list[i] != NULL; i++) {
		// Return the first one;
		strcpy(ip, inet_ntoa(*addr_list[i]) );
	}
	printf("%s resolved to : %s" , hostname , ip);

  return 0;
}

