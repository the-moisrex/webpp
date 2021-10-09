// Created by moisrex


#include "../core/include/webpp/json/json_concepts.hpp"
#include "../core/include/webpp/json/defaultjson.hpp"
#include "./common_pch.hpp"

using namespace webpp;
using namespace webpp::json;

using doc_type = document<>;
static_assert(JSONDocument<doc_type>);
static_assert(JSONObject<typename doc_type::object_type>);
static_assert(JSONValue<typename doc_type::value_type>);

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
