
include_directories(../src ../ext/googletest/include)
add_executable(kiwi-http-test kiwi/http/helpers.cc kiwi/http/server.cc kiwi/http/response.cc)
target_link_libraries(kiwi-http-test kiwi-http gtest gtest_main pthread)

add_test(kiwi-http-test kiwi-http-test)

