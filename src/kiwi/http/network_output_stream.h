/**
 * @file network_output_stream.h
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#ifndef KIWI_HTTP_NETWORK_OUTPUT_STREAM_H_
#define KIWI_HTTP_NETWORK_OUTPUT_STREAM_H_

#include <stdlib.h>

namespace kiwi {
  namespace http {
    class NetworkOutputStream {
      public:
      void set_socket (int a_socket);
      int socket () const;
      int send (const char* a_buffer, size_t a_size);

      protected:
      int file_descriptor_;
    };
  }
}

#endif // KIWI_HTTP_NETWORK_OUTPUT_STREAM_H_

