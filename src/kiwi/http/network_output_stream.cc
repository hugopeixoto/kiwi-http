/**
 * @file network_output_stream.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/network_output_stream.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <string.h>

using kiwi::http::NetworkOutputStream;

void NetworkOutputStream::set_socket (int a_socket)
{
  file_descriptor_ = a_socket;
}

int NetworkOutputStream::socket () const
{
  return file_descriptor_;
}

int NetworkOutputStream::send (const char* a_buffer, size_t a_size)
{
  return ::send(file_descriptor_, a_buffer, a_size, MSG_NOSIGNAL);
}

