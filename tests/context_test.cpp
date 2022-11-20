// Created by moisrex on 7/1/20.
#include "common_pch.hpp"
#include "fake_protocol.hpp"

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
static_assert(HTTPRequest<request_type>, "fake_request should be a http request");
using context_type = simple_context<request_type>;

struct fake_mommy {
    struct my_context_extension {
        template <Traits TraitsType>
        struct type {
            bool test             = true;
            type()                = default;
            type(type const&)     = default;
            type(type&&) noexcept = default;
        };
    };

    using context_extensions = extension_pack<my_context_extension>;
};

TEST(Routes, ContextTests) {

    // todo: do we need this?
    // EXPECT_TRUE(static_cast<bool>(Protocol<typename request_type::protocol_type>));

    EXPECT_TRUE(static_cast<bool>(Traits<typename context_type::traits_type>));
    EXPECT_TRUE(static_cast<bool>(HTTPRequest<typename context_type::request_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_copy_constructible_v<context_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_move_constructible_v<context_type>));
    EXPECT_TRUE(static_cast<bool>(Context<context_type>));

    traits::allocator_pack_type<std_traits> alloc_pack;
    context_type                            ctx{alloc_pack};

    auto nctx = ctx.template clone<typename fake_mommy::my_context_extension, string_response>();
    // using nctx_type = decltype(nctx);
    EXPECT_TRUE(nctx.test);

    using context_type2 = simple_context<request_type, extension_pack<string_response, fake_mommy>>;
    EXPECT_TRUE(static_cast<bool>(Traits<typename context_type2::traits_type>));
    EXPECT_TRUE(static_cast<bool>(HTTPRequest<typename context_type2::request_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_copy_constructible_v<context_type2>));
    EXPECT_TRUE(static_cast<bool>(std::is_move_constructible_v<context_type2>));
    EXPECT_TRUE(static_cast<bool>(Context<context_type2>));

    context_type2 ctx2{nctx};
    auto          res = ctx2.string("test");
    EXPECT_EQ(res.body, "test") << res.body.str();
    EXPECT_TRUE(ctx2.test);
}
