/**
 * @file server.cc
 * tests/kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "gtest/gtest.h"
#include "kiwi/http/response.h"

class Response : public kiwi::http::CallbackStream {
  public:
  Response ()
  {
    CallbackStream::callback = [this](const char* a_buffer, size_t a_size) -> int {
      buffer += std::string(a_buffer, a_size);
      return a_size;
    };
  }

  std::string buffer;
};

TEST(KiwiHttpResponse, ShouldBodyChunkBytes)
{
  Response r;

  {
    r << "Hello, world";
    r.finish();
    EXPECT_EQ("Hello, world", r.buffer);
    r.buffer = "";
  }

  {
    r << "Hello, world";
    r.finish();
    EXPECT_EQ("Hello, world", r.buffer);
    r.buffer = "";
  }
}

