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



using Types = testing::Types<std_traits, std_pmr_traits, default_dynamic_traits, default_traits>;

template <Traits T>
struct TheViews : testing::Test {
    using traits_type        = T;
    using string_type        = traits::string<traits_type>;
    using mustache_view_type = mustache_view<traits_type>;
    using data_type          = typename mustache_view_type::data_type;
    using variable_type      = typename data_type::value_type;
    using partial_type       = typename mustache_view_type::partial_type;
};

TYPED_TEST_SUITE(TheViews, Types);

TYPED_TEST(TheViews, MustacheView) {
    enable_owner_traits<typename TestFixture::traits_type> etraits;

    typename TestFixture::mustache_view_type view{etraits};
    view.scheme("My name is {{name}}");
    typename TestFixture::string_type str;
    auto                              data = object::make_object<typename TestFixture::data_type>(etraits);
    data.emplace_back(etraits, "name", "moisrex");
    view.render(str, data);
    EXPECT_EQ(str, "My name is moisrex");
    data.clear();
    str.clear();
    data.emplace_back(etraits, "name", "The Moisrex");
    view.render(str, data);
    EXPECT_EQ(str, "My name is The Moisrex");
}

TYPED_TEST(TheViews, ViewManagerTest) {
    enable_owner_traits<typename TestFixture::traits_type> etraits;

    view_manager<typename TestFixture::traits_type> man{etraits};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    auto data = object::make_object<typename TestFixture::data_type>(etraits);
    data.emplace_back(etraits, "name", "moisrex");
    auto const res = man.mustache("assets/hello-world", data);
    EXPECT_EQ(res, "Hello, moisrex") << "Check out the logs, it shouldn't be empty if the file was found.\n"
                                     << roots;
}

TYPED_TEST(TheViews, MustacheViewPartials) {
    enable_owner_traits<typename TestFixture::traits_type> etraits;

    view_manager<typename TestFixture::traits_type> man{etraits};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    auto data = object::make_object<typename TestFixture::data_type>(etraits);
    data.emplace_back(etraits, "name", "moisrex");
    data.emplace_back(etraits,
                      "hello-world",
                      typename TestFixture::partial_type([]() -> typename TestFixture::string_type {
                          return "Hello, {{name}}";
                      }));
    auto const res = man.mustache("assets/hello-bob", data);
    EXPECT_EQ(res, "Bob says: Hello, moisrex\n");
}

TYPED_TEST(TheViews, FileView) {
    enable_owner_traits<typename TestFixture::traits_type> etraits;

    view_manager<typename TestFixture::traits_type> man{etraits};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    auto const res = man.file("assets/hello-world.mustache");
    EXPECT_EQ(res, "Hello, {{name}}")
      << "Check out the logs, it shouldn't be empty if the file was found.\n"
      << roots;
}
