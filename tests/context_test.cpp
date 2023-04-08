// Created by moisrex on 7/1/20.
#include "common_pch.hpp"
#include "fake_protocol.hpp"

using namespace webpp;
using namespace webpp::http;

namespace fake {
    struct app {
        HTTPResponse auto operator()(HTTPRequest auto&& req) {
            static_router _router{[]() {}};
            return _router(req);
        }
    };
} // namespace fake


using fake_protocol = fake_proto<default_traits, fake::app, extension_pack<string_body>>;
using request_type  = typename fake_protocol::request_type;
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

    fake_protocol server1;
    request_type  req{server1};
    context_type  ctx{req};

    auto nctx = ctx.template clone<typename fake_mommy::my_context_extension, string_body>();
    // using nctx_type = stl::remove_cvref_t<decltype(nctx)>;
    EXPECT_TRUE(nctx.test);


    using request_type2 =
      typename fake_proto<default_traits, fake::app, extension_pack<string_body, fake_mommy>>::request_type;
    using context_type2 = simple_context<request_type2>;
    EXPECT_TRUE(static_cast<bool>(Traits<typename context_type2::traits_type>));
    EXPECT_TRUE(static_cast<bool>(HTTPRequest<typename context_type2::request_type>));
    EXPECT_TRUE(static_cast<bool>(std::is_copy_constructible_v<context_type2>));
    EXPECT_TRUE(static_cast<bool>(std::is_move_constructible_v<context_type2>));
    EXPECT_TRUE(static_cast<bool>(Context<context_type2>));

    // static_assert(stl::same_as<context_type2, nctx_type>,
    //               "Both should produce the same type for the copying below to work");

    fake_protocol               server;
    context_type2::request_type req2{server};
    context_type2               ctx2{req2};
    auto                        res = ctx2.string("test");
    EXPECT_EQ(res.body.as_string(), "test") << res.body.as_string();
    EXPECT_TRUE(ctx2.test);
}
