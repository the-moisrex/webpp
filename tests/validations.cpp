#include "../src/validation.h"
#include <gtest/gtest.h>
#include <string>

using namespace webpp::is;
using namespace std;

TEST(ValidationTest, DoesIsEmptyWork) {
  EXPECT_TRUE(empty(""));
  EXPECT_FALSE(empty("not empty"));
  string str = "";
  EXPECT_TRUE(empty(str));
  str = "not empty";
  EXPECT_FALSE(empty(str));
}
