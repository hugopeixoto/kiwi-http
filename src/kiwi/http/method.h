/**
 * @file method.h
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#ifndef KIWI_HTTP_METHOD_H_
#define KIWI_HTTP_METHOD_H_

#include <stdint.h>

namespace kiwi {
  namespace http {
    enum class Method : uint8_t {
      GET,
      POST,
      PUT,
      DELETE,
      HEAD
    };
  }
}

#endif // KIWI_HTTP_METHOD_H_

