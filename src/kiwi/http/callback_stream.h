/**
 * @file callback_stream.h
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#ifndef KIWI_HTTP_CALLBACK_STREAM_H_
#define KIWI_HTTP_CALLBACK_STREAM_H_

#include <stdlib.h>
#include <sstream>
#include <functional>

namespace kiwi {
  namespace http {
    class CallbackStream : private std::streambuf, public std::ostream {
      public:
      typedef std::function<int(const char*, size_t)> Callback;
      CallbackStream ();
      ~CallbackStream ();

      void finish ();

      protected:
      // std::streambuf implementation
      std::streambuf::int_type overflow (std::streambuf::int_type c);
      std::streambuf::int_type sync ();
      void clear ();

      char buffer_[4096];

      Callback callback;
    };
  }
}

#endif // KIWI_HTTP_CALLBACK_STREAM_H_

