
#include "../webpp/convert/lexical_cast.hpp"
#include "common/tests_common_pch.hpp"

TEST(Convert, LexicalCast) {
    using webpp::lexical::cast;

    EXPECT_EQ(20, cast<int>("20"));
    EXPECT_EQ(-43, cast<int>("-43"));
    // todo: make these two or at least the second one work
    EXPECT_EQ(static_cast<unsigned>(-43), cast<unsigned>(-43)) << "static_casting -43 to unsigned is not 43";
    EXPECT_EQ(static_cast<unsigned>(-43), cast<unsigned>("-43"))
      << "-43 cannot be converted to unsigned int that easily";
    EXPECT_EQ("-43", cast<std::string_view>("-43"));
    EXPECT_EQ("-43", cast<std::string>("-43"));
    EXPECT_EQ("-43", cast<std::string>("-43"));
    EXPECT_EQ("-43", cast<std::string>(-43));

    EXPECT_FLOAT_EQ(-43.3F, cast<float>("-43.3"));
    EXPECT_FLOAT_EQ(0.1233F, cast<float>(".1233"));
    EXPECT_DOUBLE_EQ(0.1233, cast<double>(".1233"));
}
