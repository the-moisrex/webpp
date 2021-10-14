// Created by moisrex


#include "../core/include/webpp/json/defaultjson.hpp"
#include "../core/include/webpp/json/json_concepts.hpp"
#include "../core/include/webpp/json/json_common.hpp"
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
    EXPECT_FALSE(doc.is<int>());

    EXPECT_EQ(doc.size(), 1);

    auto id = doc["id"].as_object();
    EXPECT_TRUE(is<int>(id));
    EXPECT_EQ(as<int>(id), 20);

    auto [key, value] = doc;
    EXPECT_EQ(key.as_string(), "id");
    EXPECT_EQ(value.as_int(), 20);
}



TEST(JSONTest, Fields) {
    using namespace std;

    field<int>            user_id{"id"};
    field<string>         username{"username"};
    field<vector<string>> emails{"emails"};

    document doc{R"JSON({
        "username": "the-moisrex",
        "emails":   ["one@example.com", "two@example.com"]
        "id":       666
    })JSON"};

    (user_id, username, emails) = doc;


    object(username, user_id, emails);
}