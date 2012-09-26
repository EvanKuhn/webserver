#include "http_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HttpResponse {
  char*  buf;     // Character buffer
  size_t buflen;  // Size of character buffer
  size_t txtlen;  // Length of string stored withing character buffer
};

// This helper function ensures that there is room for at least 'len' characters
// within the character buffer (excluding the null terminator). If not, it will
// reallocate space.
void http_response_ensure_space_for(HttpResponse* res, size_t len) {
  if(res->buf == NULL) {
    res->buf = malloc(len + 1);
    res->buflen = len + 1;
    bzero(res->buf, len + 1);
  }
  else {
    const size_t available = res->buflen - res->txtlen - 1;
    if(available < len) {
      size_t new_buflen = res->buflen * 2;
      if(new_buflen < len) new_buflen = res->buflen + len + 1;
      res->buf = realloc(res->buf, new_buflen);
      res->buflen = new_buflen;
      bzero(res->buf + res->txtlen, new_buflen - res->txtlen);
    }
  }
}

HttpResponse* http_response_new() {
  HttpResponse* res = malloc(sizeof(HttpResponse));
  res->buf = 0;
  res->buflen = 0;
  res->txtlen = 0;
  return res;
}

void http_response_free(HttpResponse* res) {
  if(res->buf) free(res->buf);
  free(res);
}

void http_response_clear(HttpResponse* res) {
  if(res->buf) {
    free(res->buf);
    res->buf = NULL;
    res->buflen = 0;
    res->txtlen = 0;
  }
}

void http_response_set_status(HttpResponse* res, enum EHttpVersion ver, enum EHttpStatus status) {
  http_response_ensure_space_for(res, 1024);
  const size_t printed = sprintf(res->buf + res->txtlen,
                                 "%s %i %s\r\n",
                                 http_version_to_string(ver),
                                 status,
                                 http_status_to_string(status));
  res->txtlen += printed;
}

void http_response_add_header(HttpResponse* res, const char* key, const char* value) {
  http_response_ensure_space_for(res, 1024);
  const size_t printed = sprintf(res->buf + res->txtlen, "%s: %s\r\n", key, value);
  res->txtlen += printed;
}

void http_response_set_body(HttpResponse* res, const char* body) {
  http_response_ensure_space_for(res, 2 + strlen(body));
  const size_t printed = sprintf(res->buf + res->txtlen, "\r\n%s", body);
  res->txtlen += printed;
}

const char* http_response_string(const HttpResponse* res) {
  return (res->buf ? res->buf : "");
}

size_t http_response_length(const HttpResponse* res) {
  return res->txtlen;
}
