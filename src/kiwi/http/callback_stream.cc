/**
 * @file callback_stream.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/response.h"
#include "kiwi/http/network_output_stream.h"

#include <stdint.h>
#include <string.h>

using kiwi::http::CallbackStream;

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

