/**
 * @file helpers.cc
 * tests/kiwi/http
 *
 * Copyright (c) 2013 Hugo Peixoto.
 * Distributed under the MIT License.
 */
#include "gtest/gtest.h"
#include "kiwi/http/helpers.h"

using kiwi::http::Helpers;

TEST(KiwiHttpHelpers, ShouldPercentDecodeEmptyString)
{
  EXPECT_EQ("", Helpers::percent_decode(""));
}

TEST(KiwiHttpHelpers, ShouldDecodeStringWithoutEncodedCharacters)
{
  std::string s("Hello, is it me you're looking for?");
  EXPECT_EQ(s, Helpers::percent_decode(s));
}

TEST(KiwiHttpHelpers, ShouldDecodeSingleEncodedCharacter)
{
  EXPECT_EQ(" ", Helpers::percent_decode("%20"));
}

TEST(KiwiHttpHelpers, ShouldDecodeCharacterInTheMiddleOfWords)
{
  EXPECT_EQ("Hello, world.", Helpers::percent_decode("Hello,%20world."));
}

TEST(KiwiHttpHelpers, ShouldDecodeMultipleEncodedCharacters)
{
  EXPECT_EQ("Hello, world.", Helpers::percent_decode("Hello%2C%20world."));
}

TEST(KiwiHttpHelpers, ShouldExtractQueryString)
{
  EXPECT_EQ("a=b&c=d", Helpers::query_string("http://x.com/y?a=b&c=d"));
}

TEST(KiwiHttpHelpers, ShouldNotExtractQueryStringIfNotPresent)
{
  EXPECT_EQ("", Helpers::query_string("http://x.com/y"));
}

TEST(KiwiHttpHelpers, ShouldParseEmptyQueryString)
{
  Helpers::Parameters p;
  Helpers::parse_query_string("", p);

  EXPECT_EQ(0, p.size());
}

TEST(KiwiHttpHelpers, ShouldParseQueryStringWithSingleKeyValue)
{
  Helpers::Parameters p;
  Helpers::parse_query_string("key=value", p);

  ASSERT_EQ(1, p.count("key"));
  ASSERT_EQ("value", p["key"]);
}

TEST(KiwiHttpHelpers, ShouldParseQueryStringWithMultipleKeyValues)
{
  Helpers::Parameters p;
  Helpers::parse_query_string("key=value&star=sun", p);

  ASSERT_EQ(1, p.count("key"));
  ASSERT_EQ(1, p.count("star"));
  EXPECT_EQ("value", p["key"]);
  EXPECT_EQ("sun", p["star"]);
}

TEST(KiwiHttpHelpers, ShouldParseQueryStringWithKeysWithoutValues)
{
  Helpers::Parameters p;
  Helpers::parse_query_string("key=value&earth&star=sun", p);

  ASSERT_EQ(1, p.count("key"));
  ASSERT_EQ(1, p.count("star"));
  ASSERT_EQ(1, p.count("earth"));
  EXPECT_EQ("value", p["key"]);
  EXPECT_EQ("sun", p["star"]);
  EXPECT_EQ("", p["earth"]);
}

TEST(KiwiHttpHelpers, ShouldRemoveQueryString)
{
  EXPECT_EQ("http://x.com/y",
    Helpers::without_query_string("http://x.com/y?a=b&c=d"));
}

TEST(KiwiHttpHelpers, ShouldRemoveQueryStringEvenIfThereIsNone)
{
  EXPECT_EQ("http://x.com/y",
    Helpers::without_query_string("http://x.com/y"));
}

