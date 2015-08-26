/**
 * @file helpers.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/helpers.h"

static char hex_value[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
  -1,0xA,0xB,0xC,0xD,0xE, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

using kiwi::http::Helpers;

std::string Helpers::percent_decode (const std::string& a_string)
{
  std::string result;
  result.reserve(a_string.size());

  for (size_t i = 0; i < a_string.size(); ++i) {
    if (a_string[i] == '%' &&
        (i + 2) < a_string.size() &&
        hex_value[(uint8_t)a_string[i+1]] >= 0 &&
        hex_value[(uint8_t)a_string[i+2]] >= 0) {
      result += (hex_value[(uint8_t)a_string[i + 1]] << 4) | (hex_value[(uint8_t)a_string[i + 2]]);
      i += 2;
    } else {
      result += a_string[i];
    }
  }

  return result;
}

std::string Helpers::query_string (const std::string& a_string)
{
  auto it = a_string.find('?');

  if (it == std::string::npos) {
    return "";
  } else {
    return a_string.substr(it + 1);
  }
}

std::string Helpers::without_query_string (const std::string& a_string)
{
  auto it = a_string.find('?');

  if (it == std::string::npos) {
    return a_string;
  } else {
    return a_string.substr(0, it);
  }
}

void Helpers::parse_query_string (
  const std::string& a_query_string,
  Parameters& a_params)
{
  for (size_t i = 0; i < a_query_string.size();) {
    size_t j = a_query_string.find_first_of("&=", i);
    if (j == std::string::npos) {
      a_params[percent_decode(a_query_string.substr(i))] = "";
      i = a_query_string.size();
    } else {
      if (a_query_string[j] == '&') {
        a_params[percent_decode(a_query_string.substr(i, j - i))] = "";
        i = j + 1;
      } else {
        size_t k = a_query_string.find_first_of("&", j + 1);
        if (k == std::string::npos) {
          a_params[percent_decode(a_query_string.substr(i, j - i))] = percent_decode(a_query_string.substr(j + 1));
          i = a_query_string.size();
        } else {
          a_params[percent_decode(a_query_string.substr(i, j - i))] = percent_decode(a_query_string.substr(j + 1, k - (j + 1)));
          i = k + 1;
        }
      }
    }
  }
}
