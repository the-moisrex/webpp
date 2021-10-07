// Created by moisrex


#include "../core/include/webpp/json/defaultjson.hpp"
#include "./common_pch.hpp"

using namespace webpp;
using namespace webpp::json;

TEST(JSONTest, Parse) {
    document doc;
    doc.parse("{id: 20}");
    EXPECT_TRUE(doc.is_object());
    EXPECT_EQ(doc["id"].as<int>(), 20);
}
