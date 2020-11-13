// Created by moisrex on 7/1/20.
#include "./fake_protocol.hpp"

#include "./common_pch.hpp"
#include <string>
#include <utility>

using namespace webpp;

struct fake_app {
    Response auto operator()(Request auto&& req) {
        router _router{[]() {}};
        return _router(req);
    }
};

using request_type = typename fake_proto<std_traits, fake_app>::request_type;
using context_type = simple_context<request_type>;

struct fake_mommy {
    template <typename TraitsType>
    struct type {
        bool test             = true;
        type()                = default;
        type(type const&)     = default;
        type(type&&) noexcept = default;
    };
};

TEST(Routes, PathTests) {

    EXPECT_TRUE(static_cast<bool>(Protocol<typename fake_request_type::interface_type>));

    EXPECT_TRUE(static_cast<bool>(Traits<typename fake_context_type::traits_type>));
    EXPECT_TRUE(static_cast<bool>(Request<typename fake_context_type::request_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_copy_constructible_v<fake_context_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_move_constructible_v<fake_context_type>));
    EXPECT_TRUE(static_cast<bool>(Context<fake_context_type>));

    context_type ctx{};

    auto nctx       = ctx.template clone<fake_mommy, string_response>();
    using nctx_type = decltype(nctx);
    EXPECT_TRUE(nctx.test);
    using context_type2 = simple_context<request_type, extension_pack<string_response, fake_mommy>>;
    auto ctx2 = context_type2{};
    EXPECT_EQ(ctx2.string("test").body.str(), "test");
}
