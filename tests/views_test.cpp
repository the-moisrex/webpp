// Created by moisrex on 11/04/22.
#include "../webpp/http/request_body.hpp"
#include "../webpp/http/routes/context.hpp"
#include "../webpp/traits/default_traits.hpp"
#include "../webpp/views/mustache_view.hpp"
#include "../webpp/views/view_concepts.hpp"
#include "../webpp/views/view_manager.hpp"
#include "common/tests_common_pch.hpp"


using namespace webpp;
using namespace webpp::views;

static_assert(View<mustache_view<default_traits>>);
static_assert(View<file_view<default_traits>>);
// static_assert(View<json_view<default_traits>>);
static_assert(ViewManager<view_manager<default_traits>>);


using string_type        = traits::general_string<default_traits>;
using mustache_view_type = mustache_view<default_traits>;
using data_type          = typename mustache_view_type::data_type;
using variable_type      = typename data_type::value_type;
using partial_type       = typename mustache_view_type::partial_type;


TEST(TheViews, MustacheView) {
    enable_owner_traits<default_traits> et;

    mustache_view_type v{et};
    v.scheme("My name is {{name}}");
    string_type str;
    auto        data = object::make_general<data_type>(et);
    data.emplace_back(et, "name", "moisrex");
    v.render(str, data);
    EXPECT_EQ(str, "My name is moisrex");
    data.clear();
    str.clear();
    data.emplace_back(et, "name", "The Moisrex");
    v.render(str, data);
    EXPECT_EQ(str, "My name is The Moisrex");
}


TEST(TheViews, ViewManagerTest) {

    enable_owner_traits<default_traits> et;

    view_manager<default_traits> man{et};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    auto data = object::make_general<data_type>(et);
    data.emplace_back(et, "name", "moisrex");
    const auto res = man.mustache("assets/hello-world", data);
    EXPECT_EQ(res, "Hello, moisrex") << "Check out the logs, it shouldn't be empty if the file was found.\n"
                                     << roots;
}

TEST(TheViews, MustacheViewPartials) {

    enable_owner_traits<default_traits> et;

    view_manager<default_traits> man{et};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    auto data = object::make_general<data_type>(et);
    data.emplace_back(et, "name", "moisrex");
    data.emplace_back(et, "hello-world", partial_type([]() -> string_type {
                          return "Hello, {{name}}";
                      }));
    const auto res = man.mustache("assets/hello-bob", data);
    EXPECT_EQ(res, "Bob says: Hello, moisrex\n");
}


TEST(TheViews, FileView) {
    enable_owner_traits<default_traits> et;

    view_manager<default_traits> man{et};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    const auto res = man.file("assets/hello-world.mustache");
    EXPECT_EQ(res, "Hello, {{name}}") << "Check out the logs, it shouldn't be empty if the file was found.\n"
                                      << roots;
}
