/**
 * @file server.cc
 * tests/kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "gtest/gtest.h"
#include "kiwi/http/server.h"
#include "kiwi/http/response.h"

#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <netinet/tcp.h>
#include <assert.h>

#include <thread>

using kiwi::http::Server;

int connect (uint16_t a_port)
{
  struct sockaddr_in sin;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  sin.sin_port = htons(a_port);
  sin.sin_family = AF_INET;

  connect(sock, (struct sockaddr*) &sin, sizeof(sin));
  return sock;
}

void send (int sock, const std::string& a_text)
{
  send(sock, a_text.c_str(), a_text.size(), 0);
}

std::string receive (int sock)
{
  std::string body;
  char buffer[1024];

  for (int r = 0; (r = recv(sock, buffer, 1024, 0)); body += std::string(buffer, r));

  return body;
}


TEST(KiwiHttpServer, ShouldConstruct)
{
  Server s;
  EXPECT_TRUE(s.construct(10001));
}

TEST(KiwiHttpServer, ShouldConstructTwoServersInDifferentPorts)
{
  Server s1;
  Server s2;

  EXPECT_TRUE(s1.construct(10002));
  EXPECT_TRUE(s2.construct(10003));
}

TEST(KiwiHttpServer, ShouldNotConstructTwoServersInSamePorts)
{
  Server s1;
  Server s2;

  EXPECT_TRUE(s1.construct(10004));
  EXPECT_FALSE(s2.construct(10004));
}

TEST(KiwiHttpServer, ShouldReturnTrueWhenItReceivesData)
{
  kiwi::http::Request* request;
  kiwi::http::Response* response;

  Server server;
  server.construct(10005);

  std::thread sender([](){
    int sock = connect(10005);
    send(sock, "X");
    close(sock);
  });

  EXPECT_TRUE(server.begin(request, response));
  sender.join();
}

TEST(KiwiHttpServer, ShouldHaveNullRequestAndResponseWhenReceivingNonHTTPRequest)
{
  kiwi::http::Request* request;
  kiwi::http::Response* response;

  Server server;
  server.construct(10006);

  std::thread sender([](){
    int sock = connect(10006);
    send(sock, "X");
    close(sock);
  });

  server.begin(request, response);
  EXPECT_TRUE(nullptr == request);
  EXPECT_TRUE(nullptr == response);
  sender.join();
}

TEST(KiwiHttpServer, ShouldHaveRequestAndResponseWhenReceivingFullHTTPRequest)
{
  kiwi::http::Request* request;
  kiwi::http::Response* response;

  Server server;
  server.construct(10007);

  std::thread sender([](){
    int sock = connect(10007);
    send(sock, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n\r\n");
    close(sock);
  });

  server.begin(request, response); // accept connection
  server.begin(request, response); // read string
  server.begin(request, response); // pop it

  EXPECT_TRUE(nullptr != request);
  EXPECT_TRUE(nullptr != response);
  sender.join();
}

TEST(KiwiHttpServer, ShouldSendHTTPResponse)
{
  kiwi::http::Request* request;
  kiwi::http::Response* response;

  Server server;
  server.construct(10008);

  std::string body;

  std::thread sender([&body](){
    int sock = connect(10008);
    send(sock, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");

    body = receive(sock);
    close(sock);
  });

  while (server.begin(request, response)) {
    if (request && response) {
      response->start_body();
      response->body_chunk("Hello", 5);
      response->finish_body();
      server.end(request, response);
      break;
    }
  }

  sender.join();
  EXPECT_EQ(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\n"
    "Hello\r\n"
    "0\r\n\r\n",
    body);
}

TEST(KiwiHttpServer, ShouldHandleMoreThanOneClient)
{
  kiwi::http::Request* request;
  kiwi::http::Response* response;

  Server server;
  server.construct(10009);

  std::string body1;
  std::string body2;

  std::thread sender1([&body1](){
    int sock = connect(10009);
    send(sock, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");

    body1 = receive(sock);
    close(sock);
  });

  std::thread sender2([&body2](){
    int sock = connect(10009);
    send(sock, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");

    body2 = receive(sock);
    close(sock);
  });


  for (int i = 0; server.begin(request, response);) {
    if (request && response) {
      response->start_body();
      response->body_chunk("Hello", 5);
      response->finish_body();
      server.end(request, response);

      ++i;
      if (i == 2) break;
    }
  }

  sender1.join();
  sender2.join();
  EXPECT_EQ(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\n"
    "Hello\r\n"
    "0\r\n\r\n",
    body1);

  EXPECT_EQ(body1, body2);
}

