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
static_assert(View<json_view<default_traits>>);

TEST(TheViews, MustacheView) {
    using namespace mustache;

    mustache_view<default_traits> v;
    v.scheme("My name is {{name}}");
    auto res = v.render({"name", "moisrex"});
    EXPECT_EQ("My name is moisrex", res);
}


TEST(TheViews, ViewManagerTest) {}