#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <webpp/validation.hpp>

using namespace webpp::is;
using namespace std;

TEST(ValidationsTest, EmptyFunction) {
  EXPECT_TRUE(empty(""));
  EXPECT_FALSE(empty("not empty"));
  string str = "";
  EXPECT_TRUE(empty(str));
  str = "not empty";
  EXPECT_FALSE(empty(str));
}

TEST(ValidationsTest, ContainsFunctions) {
  map<int, string> data;
  data[0] = "hello world";
  data[10] = "testing";

  EXPECT_TRUE(contains_key(data, 10));
  EXPECT_TRUE(contains_value(data, string("hello world")));
  EXPECT_FALSE(contains_key(data, 100));
  EXPECT_FALSE(contains_value(data, string("it's not gonna be there")));
}

TEST(ValidationsTest, TrimmedFunctions) {
  EXPECT_TRUE(trimmed("trimmed string"));
  EXPECT_TRUE(rtrimmed(" right trimmed"));
  EXPECT_TRUE(ltrimmed("left trimmed "));
}
