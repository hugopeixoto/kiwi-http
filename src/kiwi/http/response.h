/**
 * @file response.h
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#ifndef KIWI_HTTP_RESPONSE_H_
#define KIWI_HTTP_RESPONSE_H_

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

    class Response : public CallbackStream {
      public:
      Response ();

      void set_socket (int a_file_descriptor);

      int socket () const;

      void start_body ();
      void finish_body ();
      int body_chunk (const char* a_buffer, size_t a_size);

      protected:
      int fd_;
    };
  }
}

#endif // KIWI_HTTP_RESPONSE_H_

