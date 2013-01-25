/**
 * @file response.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "http/response.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>

using kiwi::http::Response;

void Response::start_body ()
{
  static const char msg[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n";

  send(fd_, msg, sizeof(msg) - 1, 0);
}

void Response::finish_body ()
{
  static const char msg[] = "0\r\n\r\n";

  sync();
  send(fd_, msg, sizeof(msg) - 1, 0);
}

int Response::body_chunk (const char* a_buffer, size_t a_size)
{
  char chunk_size[8 + 2];

  for (int i = 0; i < 8; ++i) {
    uint8_t nibble = (a_size >> ((8 - 1 - i) * 4)) & 0xF;
    chunk_size[i] = (nibble < 0xA ? '0' : 'A' - 10) + nibble;
  }

  int j = 0;
  while (chunk_size[j] == '0') ++j;

  chunk_size[8] = '\r';
  chunk_size[8+1] = '\n';

  send(fd_, chunk_size + j, sizeof(chunk_size) - j, 0);
  send(fd_, a_buffer, a_size, 0);
  send(fd_, "\r\n", 2, 0);
  return 0;
}

