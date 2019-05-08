#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <webpp/validation.h>

TEST(ValidationsTest, EmptyFunction) {
  EXPECT_TRUE(webpp::is::empty(""));
  EXPECT_FALSE(webpp::is::empty("not empty"));
  std::string str = "";
  EXPECT_TRUE(webpp::is::empty(str));
  str = "not empty";
  EXPECT_FALSE(webpp::is::empty(str));
}

TEST(ValidationsTest, ContainsFunctions) {
  std::map<int, std::string> data;
  data[0] = "hello world";
  data[10] = "testing";

  //  EXPECT_TRUE(webpp::is::contains_key(data, 10));
  //  EXPECT_TRUE(webpp::is::contains_value(data, std::string("hello world")));
  //  EXPECT_FALSE(webpp::is::contains_key(data, 100));
  //  EXPECT_FALSE(
  //      webpp::is::contains_value(data, std::string("it's not gonna be
  //      there")));
}
