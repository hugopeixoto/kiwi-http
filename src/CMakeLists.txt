
include_directories(. ../ext/http-parser)

add_library(kiwi-http kiwi/http/helpers.cc kiwi/http/parser.cc kiwi/http/request.cc kiwi/http/server.cc kiwi/http/response.cc kiwi/http/callback_stream.cc kiwi/http/network_output_stream.cc)
target_link_libraries(kiwi-http http-parser)

