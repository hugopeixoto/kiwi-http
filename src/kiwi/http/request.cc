/**
 * @file request.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "http/request.h"

using kiwi::http::Request;

Request::Request ()
{
  clear();
}

void Request::clear ()
{
  uri_ = "";
  params.clear();
  headers.clear();
}

std::string Request::cookie (const char* a_key) const
{
  const std::string& cookies = headers["Cookie"];
  // TODO (hpeixoto): Implement this correctly.
  return cookies.substr(cookies.find("=") + 1);
}

