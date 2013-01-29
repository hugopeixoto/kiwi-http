/**
 * @file helpers.h
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#ifndef KIWI_HTTP_HELPERS_H_
#define KIWI_HTTP_HELPERS_H_

#include <string>
#include <map>

namespace kiwi {
  namespace http {
    class Helpers {
      public:
      typedef std::map<std::string, std::string> Parameters;

      static std::string percent_decode (const std::string& a_string);

      static std::string query_string (const std::string& a_uri);
      static std::string without_query_string (const std::string& a_uri);

      static void parse_query_string (
        const std::string& a_query_string,
        Parameters& a_params);
    };
  }
}

#endif // KIWI_HTTP_HELPERS_H_
