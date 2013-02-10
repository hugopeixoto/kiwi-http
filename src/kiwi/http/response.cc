/**
 * @file response.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/response.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>

using kiwi::http::CallbackStream;
using kiwi::http::Response;

Response::Response ()
{
  CallbackStream::callback = [this](const char* a_buffer, size_t a_size) -> int {
    return this->body_chunk(a_buffer, a_size);
  };
}

int Response::socket () const
{
  return fd_;
}

void Response::set_socket (int a_file_descriptor)
{
  fd_ = a_file_descriptor;
}

void Response::start_body ()
{
  static const char msg[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n";

  send(fd_, msg, sizeof(msg) - 1, MSG_NOSIGNAL);
}

void Response::finish_body ()
{
  static const char msg[] = "0\r\n\r\n";

  finish();
  send(fd_, msg, sizeof(msg) - 1, MSG_NOSIGNAL);
}

int Response::body_chunk (const char* a_buffer, size_t a_size)
{
  char chunk_size[8 + 2];

  printf("BodyChunk: %d [%.*s]\n", (int)a_size, a_size, a_buffer);

  for (int i = 0; i < 8; ++i) {
    uint8_t nibble = (a_size >> ((8 - 1 - i) * 4)) & 0xF;
    chunk_size[i] = (nibble < 0xA ? '0' : 'A' - 10) + nibble;
  }

  int j = 0;
  while (chunk_size[j] == '0') ++j;

  chunk_size[8] = '\r';
  chunk_size[8+1] = '\n';

  send(fd_, chunk_size + j, sizeof(chunk_size) - j, MSG_NOSIGNAL);
  send(fd_, a_buffer, a_size, MSG_NOSIGNAL);
  send(fd_, "\r\n", 2, MSG_NOSIGNAL);
  return 0;
}

// Callback Stream helper
//
//

CallbackStream::CallbackStream ()
: std::ostream(static_cast<std::streambuf*>(this)), std::ios(0)
{
  clear();
}

CallbackStream::~CallbackStream ()
{
  finish();
}

void CallbackStream::finish ()
{
  sync();
  clear();
}

void CallbackStream::clear ()
{
  setp(buffer_, buffer_ + sizeof(buffer_) - 1);
}

std::streambuf::int_type CallbackStream::overflow (std::streambuf::int_type ch)
{
  char* end = pptr();
  if (ch != std::streambuf::traits_type::eof()) {
    *end = ch;
    ++end;
  }

  if (callback(pbase(), end - pbase()) == -1) {
    ch = std::streambuf::traits_type::eof();
  } else {
    ch = 0;
  }

  clear();

  return ch;
}

std::streambuf::int_type CallbackStream::sync ()
{
  return (pptr() == pbase() || callback(pbase(), pptr() - pbase()) != -1) ? 0 : -1;
}

