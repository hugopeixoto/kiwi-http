/**
 * @file parser.cc
 * kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "kiwi/http/parser.h"
#include "kiwi/http/method.h"
#include "kiwi/http/helpers.h"

#include "http_parser.h"

#include <string.h>

using kiwi::http::Parser;

/**
 * Not-so-wonderful hack to avoid header tainting
 */
#define SETTINGS() static_cast<http_parser_settings*>(settings_)
#define PARSER() static_cast<http_parser*>(parser_)

typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);
typedef int (*http_cb) (http_parser*);

/**
 * top level parsing functions
 */
int on_message_begin (http_parser* a_parser)
{
  return static_cast<Parser*>(a_parser->data)->on_message_begin();
}
int on_message_complete (http_parser* a_parser)
{
  return static_cast<Parser*>(a_parser->data)->on_message_complete();
}
int on_headers_complete (http_parser* a_parser)
{
  return static_cast<Parser*>(a_parser->data)->on_headers_complete();
}

int on_url (http_parser* a_parser, const char* a_buffer, size_t a_length)
{
  return static_cast<Parser*>(a_parser->data)->on_url(a_buffer, a_length);
}
int on_header_field (http_parser* a_parser, const char* a_buffer, size_t a_length)
{
  return static_cast<Parser*>(a_parser->data)->on_header_field(a_buffer, a_length);
}
int on_header_value (http_parser* a_parser, const char* a_buffer, size_t a_length)
{
  return static_cast<Parser*>(a_parser->data)->on_header_value(a_buffer, a_length);
}
int on_body (http_parser* a_parser, const char* a_buffer, size_t a_length)
{
  return static_cast<Parser*>(a_parser->data)->on_body(a_buffer, a_length);
}

Parser::Parser ()
{
  total_requests_ = 0;
  first_request_ = 0;
  completed_requests_ = 0;

  settings_ = static_cast<http_parser_settings_ptr>(new http_parser_settings());
  parser_ = static_cast<http_parser_ptr>(new http_parser());

  SETTINGS()->on_message_begin    = ::on_message_begin;
  SETTINGS()->on_message_complete = ::on_message_complete;
  SETTINGS()->on_headers_complete = ::on_headers_complete;

  SETTINGS()->on_url = ::on_url;
  SETTINGS()->on_header_field = ::on_header_field;
  SETTINGS()->on_header_value = ::on_header_value;
  SETTINGS()->on_body = ::on_body;

  http_parser_init(PARSER(), HTTP_REQUEST);
  PARSER()->data = this;

  header_buffer_ = new char[2<<12];
  header_buffer_used_ = 0;
  header_buffer_size_ = 2<<12;
}

Parser::~Parser ()
{
  delete PARSER();
  delete SETTINGS();
  delete[] header_buffer_;
}

void Parser::reset ()
{
  http_parser_init(PARSER(), HTTP_REQUEST);
}

bool Parser::feed (const char* a_buffer, size_t a_size)
{
  size_t read = http_parser_execute(PARSER(), SETTINGS(), a_buffer, a_size);
  return read == a_size;

  enum http_errno err = HTTP_PARSER_ERRNO(PARSER());

  if (err != 0) {
      printf("error %d: %s (%s)\n", err, http_errno_name(err), http_errno_description(err));
      return false;
  }

  printf("read %zd of %zd\n", read, a_size);
  return true;
  
  return read == a_size;
}

bool Parser::pop_request (Request*& a_request)
{
  if (completed_requests_ == 0) {
    return false;
  }

  a_request = &requests_[first_request_];
  first_request_ = (first_request_ + 1) % 10;
  --completed_requests_;
  --total_requests_;
  return true;
}

kiwi::http::Request& Parser::current_request ()
{
  return requests_[(first_request_ + total_requests_ - 1) % 10];
}

int Parser::on_message_begin ()
{
  if (total_requests_ == 10) {
    printf("overflow\n");
    return -1;
  }

  ++total_requests_;
  current_request().clear();
  header_buffer_used_ = 0;
  return 0;
}

int Parser::on_message_complete ()
{
  if (buffer_.size() > 0) {
    Helpers::parse_query_string(buffer_, current_request().params);
    if (current_request().params.has_key("_method")) {
      std::string method = current_request().params["_method"];
      if (method == "PUT") {
        current_request().set_method(http::Method::PUT);
      } else if (method == "DELETE") {
        current_request().set_method(http::Method::DELETE);
      }
    }

    current_request().set_body(buffer_);
    buffer_.resize(0);
  }

  ++completed_requests_;
  return 0;
}

int Parser::on_headers_complete ()
{
  add_header();

  // extract query string from URL
  Helpers::parse_query_string(
    Helpers::query_string(current_request().uri()),
    current_request().params);

  current_request().uri().assign(
    Helpers::without_query_string(current_request().uri()));

  current_request().set_keepalive(http_should_keep_alive(PARSER()));

  switch(PARSER()->method) {
    case HTTP_GET:
      current_request().set_method(http::Method::GET);
      break;
    case HTTP_POST:
      current_request().set_method(http::Method::POST);
      break;
    case HTTP_PUT:
      current_request().set_method(http::Method::PUT);
      break;
    case HTTP_DELETE:
      current_request().set_method(http::Method::DELETE);
      break;
    default:
      current_request().set_method(http::Method::GET);
      break;
  }

  return 0;
}

int Parser::on_url (const char* a_buffer, size_t a_length)
{
  current_request().uri().append(a_buffer, a_length);
  return 0;
}

int Parser::on_header_field (const char* a_buffer, size_t a_length)
{
  /*if (a_length + header_buffer_used_ <= header_buffer_size_) {
    memcpy(header_buffer_ + header_buffer_used_, a_buffer, a_length);
    header_buffer_used_ += a_length;
  }*/
  add_header();
  header_field_.append(a_buffer, a_length);
  return 0;
}

int Parser::on_header_value (const char* a_buffer, size_t a_length)
{
  /*if (a_length + header_buffer_used_ <= header_buffer_size_) {
    memcpy(header_buffer_ + header_buffer_used_, a_buffer, a_length);
    header_buffer_used_ += a_length;
  }*/
  header_value_.append(a_buffer, a_length);
  return 0;
}

int Parser::on_body (const char* a_buffer, size_t a_length)
{
  buffer_.append(a_buffer, a_length);
  return 0;
}

void Parser::add_header ()
{
  if (header_value_.size() > 0) {
    current_request().headers[header_field_] = header_value_;
    // printf("%s ===> %s\n", header_field_.c_str(), header_value_.c_str());
    header_field_.resize(0);
    header_value_.resize(0);
  }

}

