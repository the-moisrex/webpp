// Created by moisrex


#include "../core/include/webpp/json/defaultjson.hpp"
#include "./common_pch.hpp"

using namespace webpp;
using namespace webpp::json;

TEST(JSONTest, Parse) {
    document doc;
    doc.parse("{id: 20}");
    EXPECT_FALSE(doc.is_int());

    EXPECT_EQ(doc.size(), 1);

    auto id = doc["id"].as_object();
    EXPECT_TRUE(id.is_int());
    EXPECT_EQ(id.as_int(), 20);

    auto [key, value] = doc;
    EXPECT_EQ(key.as_string(), "id");
    EXPECT_EQ(value.as_int(), 20);
}
