// Created by moisrex


#include "../core/include/webpp/json/defaultjson.hpp"
#include "../core/include/webpp/json/json_common.hpp"
#include "../core/include/webpp/json/json_concepts.hpp"
#include "./common_pch.hpp"

using namespace webpp;
using namespace webpp::json;

using doc_type    = document<>;
using value_type  = typename doc_type::value_type;
using object_type = typename doc_type::object_type;
static_assert(JSONObject<object_type>);
static_assert(JSONValue<value_type>);
static_assert(JSONDocument<doc_type>);

TEST(JSONTest, Parse) {
    document doc;
    doc.parse("{id: 20}");
    EXPECT_FALSE(doc.is<int>());

    auto id = doc["id"];
    EXPECT_TRUE(is<int>(id));
    EXPECT_EQ(as<int>(id), 20);

    for (auto const& [key, value] : doc.as_object()) {
        EXPECT_EQ(key.as_string(), "id");
        EXPECT_EQ(as<int>(value), 20);
    }
}



TEST(JSONTest, Fields) {
    using namespace std;

    field<int>    user_id{"id"};
    field<string> username{"username"};
    // field<vector<string>> emails{"emails"};

    document doc{R"JSON({
        "username": "the-moisrex",
        "emails":   ["one@example.com", "two@example.com"]
        "id":       666
    })JSON"};

    (user_id, username /*, emails */) = doc;

    // let's first fix other issues then we'd add object
    // object(username, user_id, emails);
}
