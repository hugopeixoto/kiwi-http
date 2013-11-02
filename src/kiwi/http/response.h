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
#include "kiwi/http/callback_stream.h"

namespace kiwi {
  namespace http {
    class NetworkOutputStream;

    class Response : public CallbackStream {
      public:
      Response ();
      ~Response ();

      // High level HTTP helpers
      void start_body ();
      void finish_body ();
      int body_chunk (const char* a_buffer, size_t a_size);

      void redirect_to (const char* a_new_location);

      void set_socket (int a_socket);
      int socket () const;

      protected:
      NetworkOutputStream* output ();
      int send (const char* a_buffer, size_t a_size);

      bool chunked;
      NetworkOutputStream* output_;
    };
  }
}

#endif // KIWI_HTTP_RESPONSE_H_

