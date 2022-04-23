// Created by moisrex on 11/04/22.
#include "../core/include/webpp/http/request_body.hpp"
#include "../core/include/webpp/http/routes/context.hpp"
#include "../core/include/webpp/traits/default_traits.hpp"
#include "../core/include/webpp/views/mustache_view.hpp"
#include "../core/include/webpp/views/view_concepts.hpp"
#include "../core/include/webpp/views/view_manager.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::views;

static_assert(View<mustache_view<default_traits>>);
static_assert(View<file_view<default_traits>>);
// static_assert(View<json_view<default_traits>>);
static_assert(ViewManager<view_manager<default_traits>>);

TEST(TheViews, MustacheView) {
    enable_owner_traits<default_traits> et;

    mustache_view<default_traits> v{et};
    v.scheme("My name is {{name}}");
    stl::string str;
    v.render(str, {{"name", "moisrex"}});
    EXPECT_EQ("My name is moisrex", res);
}


TEST(TheViews, ViewManagerTest) {

    enable_owner_traits<default_traits> et;

    view_manager<default_traits> man{et};

    const auto res = et.mustache("assets/hello-world", {{"name", "moisrex"}});
    EXPECT_EQ(res, "Hello, moisrex");
}
