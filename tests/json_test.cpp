// Created by moisrex


#include "./common_pch.hpp"
#include "../core/include/webpp/json/default_json.hpp"

using namespace webpp;
using namespace webpp::json;

TEST(JSONTest, Parse) {
    document doc;
    doc.parse("{id: 20}");
    EXPECT_TRUE(doc.is_object());
    EXPECT_EQ(doc["id"].template as<int>(), 20);
}
