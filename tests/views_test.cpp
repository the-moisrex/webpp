// Created by moisrex on 11/04/22.
#include "../webpp/http/request_body.hpp"
#include "../webpp/http/routes/context.hpp"
#include "../webpp/views/mustache_view.hpp"
#include "../webpp/views/view_concepts.hpp"
#include "../webpp/views/view_manager.hpp"
#include "./common/test.hpp"


using namespace webpp;
using namespace webpp::views;

static_assert(View<mustache_view<char, stl::allocator<char>>>);
static_assert(View<file_view<char, stl::allocator<char>>>);
// static_assert(View<json_view<default_traits>>);
static_assert(ViewManager<view_manager<char, stl::allocator<char>>>);


using mustache_view_type = mustache_view<char, stl::allocator<char>>;
using data_type          = typename mustache_view_type::data_type;

TEST(TheViews, MustacheView) {
    mustache_view_type view;
    view.scheme("My name is {{name}}");
    stl::string str;
    data_type   data{};
    data.emplace_back("name", "moisrex");
    view.render(str, data);
    EXPECT_EQ(str, "My name is moisrex");
    data.clear();
    str.clear();
    data.emplace_back("name", "The Moisrex");
    view.render(str, data);
    EXPECT_EQ(str, "My name is The Moisrex");
}

TEST(TheViews, ViewManagerTest) {
    view_manager<char, stl::allocator<char>> man{};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    data_type data;
    data.emplace_back("name", "moisrex");
    auto const res = man.mustache("assets/hello-world", data);
    EXPECT_EQ(res, "Hello, moisrex") << "Check out the logs, it shouldn't be empty if the file was found.\n" << roots;
}

TEST(TheViews, MustacheViewPartials) {
    view_manager<char, stl::allocator<char>> man{};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    data_type data;
    data.emplace_back("name", "moisrex");
    data.emplace_back("hello-world", typename mustache_view_type::partial_type([]() -> stl::string {
                          return "Hello, {{name}}";
                      }));
    auto const res = man.mustache("assets/hello-bob", data);
    EXPECT_EQ(res, "Bob says: Hello, moisrex\n");
}

TEST(TheViews, FileView) {
    view_manager<char, stl::allocator<char>> man{};
    man.view_roots.emplace_back("../tests/assets");
    man.view_roots.emplace_back("../tests");
    man.view_roots.emplace_back("./tests");
    man.view_roots.emplace_back("./tests/assets");

    std::string roots;
    for (auto const& root : man.view_roots) {
        roots += std::filesystem::absolute(root).lexically_normal().string() + ", ";
    }

    auto const res = man.file("assets/hello-world.mustache");
    EXPECT_EQ(res, "Hello, {{name}}") << "Check out the logs, it shouldn't be empty if the file was found.\n" << roots;
}
