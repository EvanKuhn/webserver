//==============================================================================
// Socket struct types. FOR INTERNAL USE ONLY!
//
// Evan Kuhn 2012-09-09
//==============================================================================
#ifndef SOCKET_STRUCTS_H
#define SOCKET_STRUCTS_H

struct ClientSocket {
  int                fd;       // File descriptor
  struct sockaddr_in addr;     // Address
  char*              data;     // Data array. Holds last-received data.
  size_t             data_len; // Data array length, >= length of data last received.
};

struct ServerSocket {
  int                fd;       // File descriptor
  struct sockaddr_in addr;     // Address
};

#endif // SOCKET_STRUCTS_H