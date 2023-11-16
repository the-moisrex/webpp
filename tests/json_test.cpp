// Created by moisrex


#include "../webpp/json/defaultjson.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace webpp::json;

// NOLINTBEGIN(*-magic-numbers)

using doc_type    = document<>;
using value_type  = typename doc_type::value_type;
using object_type = typename doc_type::object_type;
static_assert(JSONObject<object_type>);
static_assert(JSONValue<value_type>);
static_assert(JSONDocument<doc_type>);

using json_impls = stl::tuple<json::rapidjson::document<>>;

TEST(JSONTest, Parse) {
    document doc;
    EXPECT_TRUE(doc.is_null());
    EXPECT_FALSE(doc.is_object());
    const char* json_str = "{\"id\":20}";
    doc.parse(json_str);
    EXPECT_FALSE(doc.is_null()) << "We have an object now, it shouldn't be null";
    EXPECT_TRUE(doc.is_object()) << "There's an object right there.";
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

    document               doc;
    constexpr stl::string_view json_string = R"JSON({
        "username": "the-moisrex",
        "emails":   ["one@example.com", "two@example.com"],
        "id":       313
    })JSON";
    doc.parse(json_string);

    EXPECT_TRUE(doc.is_object()) << json_string;

    (user_id, username /*, emails */) = doc;

    EXPECT_EQ(*user_id, 313);
    EXPECT_EQ(username, "the-moisrex");

    username = "moisrex";
    doc      = username; // change the username inside the json document

    EXPECT_EQ(username, "moisrex");
    EXPECT_EQ(doc["username"].as_string_view(), "moisrex");

    username = "admin";
    user_id  = 72;

    doc = (username, user_id);

    EXPECT_EQ(doc["username"].as_string(), "admin");
    EXPECT_EQ(doc["id"].as_int32(), 72);

    // let's first fix other issues then we'd add object
    // object(username, user_id, emails);
}

TEST(JSONTest, Create) {
    stl::apply(
      [](JSONDocument auto&&... docs) {
          (..., ([](auto&& doc) {
               ASSERT_NO_FATAL_FAILURE((void) doc["dummy"]);
               doc["page"]     = "/about";
               doc["username"] = "admin";
               doc["id"]       = 313;
               doc["is_admin"] = false;
               doc["friends"]  = stl::array{"Jason", "Jane", "Amy", "Rose"};
               EXPECT_EQ(doc["page"].as_string(), "/about");
               EXPECT_TRUE(doc.contains("friends"));
               EXPECT_TRUE(doc["username"].is_string());

#if feature_support
               doc["info"] = {
                 {"firstname", "Jane"}, // first name
                 {"lastname", "Doe"},   // last name
                 {"id", 313}            // id
               };
               EXPECT_EQ(doc["info"]["firstname"].as_string(), "Jane");
               EXPECT_EQ(doc["info"]["id"].as_int32(), 313);
#endif
           })(stl::forward<decltype(docs)>(docs)));
      },
      json_impls());
}

// NOLINTEND(*-magic-numbers)
