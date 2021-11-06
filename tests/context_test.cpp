// Created by moisrex on 7/1/20.
#include "./common_pch.hpp"
#include "./fake_protocol.hpp"

using namespace webpp;
using namespace webpp::http;

namespace fake {
    struct app {
        HTTPResponse auto operator()(HTTPRequest auto&& req) {
            router _router{[]() {}};
            return _router(req);
        }
    };
} // namespace fake

using request_type = typename fake_proto<std_traits, fake::app>::request_type;
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

    // todo: do we need this?
    // EXPECT_TRUE(static_cast<bool>(Protocol<typename request_type::protocol_type>));

    EXPECT_TRUE(static_cast<bool>(Traits<typename context_type::traits_type>));
    EXPECT_TRUE(static_cast<bool>(HTTPRequest<typename context_type::request_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_copy_constructible_v<context_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_move_constructible_v<context_type>));
    EXPECT_TRUE(static_cast<bool>(Context<context_type>));

    traits::allocator_pack_type<std_traits> alloc_pack;
    context_type                            ctx{alloc_pack};

    auto nctx = ctx.template clone<fake_mommy, string_response>();
    // using nctx_type = decltype(nctx);
    EXPECT_TRUE(nctx.test);
    using context_type2 = simple_context<request_type, extension_pack<string_response, fake_mommy>>;
    auto ctx2           = context_type2{nctx};
    EXPECT_EQ(ctx2.string("test").body.str(), "test");
}
