#include "common/Parse.hpp"

#include <gtest/gtest.h>

TEST(TestParse, strings) {
    std::string data = "hello";
    std::string value;
    char const* beg = data.data();
    char const* end = data.data() + data.size();

    EXPECT_TRUE(auto_parse(beg, end, value));
    EXPECT_EQ(data, value);
    EXPECT_EQ(beg, end);
}

TEST(TestParse, unsigned_int) {
    std::string data = "1123abc";
    int value = 0;

    char const* beg = data.data();
    char const* end = data.data() + data.size();

    EXPECT_TRUE(auto_parse(beg, end, value));
    EXPECT_EQ(1123, value);
    EXPECT_EQ(beg, data.data() + 4);
}

TEST(TestParse, negative_int) {
    std::string data = "-123abc";
    int value = 0;

    char const* beg = data.data();
    char const* end = data.data() + data.size();

    EXPECT_TRUE(auto_parse(beg, end, value));
    EXPECT_EQ(-123, value);
    EXPECT_EQ(beg, data.data() + 4);
}

TEST(TestParse, SimpleTokenizer) {
    std::string data = "hi:2:u:4tw";
    SimpleTokenizer tok(data, ':');

    std::string str;
    int num;

    EXPECT_TRUE(tok.extract(str));
    EXPECT_EQ("hi", str);

    EXPECT_TRUE(tok.extract(num));
    EXPECT_EQ(2, num);

    EXPECT_TRUE(tok.extract(str));
    EXPECT_EQ("u", str);

    // the whole token "4tw" can't be parsed as an int, we should fail
    EXPECT_FALSE(tok.extract(num));

    // the previous call failed, it should not have advanced the read ptr.
    // we should get the whole string now
    EXPECT_TRUE(tok.extract(str));
    EXPECT_EQ("4tw", str);

    // end of string!
    EXPECT_FALSE(tok.extract(str));
    // end of string again! valgrind will tell us if we accidentally did
    // something dumb and tried to read past the end.
    EXPECT_FALSE(tok.extract(str));
}

TEST(TestParse, SimpleTokenizer_next_delim_and_skip) {
    std::string data = "abc,def,ghi";
    SimpleTokenizer tok(data, ',');
    EXPECT_EQ(data.data() + 3, tok.next_delim());
    EXPECT_EQ(1u, tok.skip(1));
    EXPECT_EQ(data.data() + 7, tok.next_delim());
    EXPECT_EQ(1u, tok.skip(1));
    EXPECT_EQ(data.data() + 11, tok.next_delim());

    EXPECT_EQ(0u, tok.skip(1));
    EXPECT_EQ(0u, tok.skip(2));
    EXPECT_EQ(0u, tok.skip(3));
}
