/**
 * @file response.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/response.h"
#include "kiwi/http/network_output_stream.h"

#include <stdint.h>
#include <string.h>

using kiwi::http::Response;
using kiwi::http::NetworkOutputStream;

Response::Response ()
{
  chunked = false;
  output_ = new NetworkOutputStream();

  CallbackStream::callback = [this](const char* a_buffer, size_t a_size) {
    if (this->chunked) {
      return this->body_chunk(a_buffer, a_size);
    } else {
      return this->send(a_buffer, a_size);
    }
  };
}

Response::~Response ()
{
  delete output_;
}

// HTTP methods
// high level helpers

void Response::start_body ()
{
  static const char msg[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n";

  send(msg, sizeof(msg) - 1);
  chunked = true;
}

void Response::finish_body ()
{
  static const char msg[] = "0\r\n\r\n";

  finish();
  if (chunked) {
    send(msg, sizeof(msg) - 1);
  }
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

  send(chunk_size + j, sizeof(chunk_size) - j);
  send(a_buffer, a_size);
  send("\r\n", 2);
  return 0;
}

void Response::redirect_to (const char* a_new_location)
{
  static const char prefix[] = "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
  static const char suffix[] = "\r\n\r\n";

  send(prefix, sizeof(prefix) - 1);
  send(a_new_location, strlen(a_new_location));
  send(suffix, sizeof(suffix) - 1);
}

void Response::set_socket (int a_socket)
{
  output_->set_socket(a_socket);
}

int Response::socket () const
{
  return output_->socket();
}

NetworkOutputStream* Response::output ()
{
  return output_;
}

int Response::send (const char* a_buffer, size_t a_size)
{
  return output()->send(a_buffer, a_size);
}

