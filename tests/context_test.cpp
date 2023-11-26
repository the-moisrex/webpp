// Created by moisrex on 7/1/20.
#include "../webpp/http/routes/context.hpp"

#include "../webpp/http/bodies/string.hpp"
#include "common/fake_protocol.hpp"
#include "common/tests_common_pch.hpp"

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

using fake_protocol = fake_proto<fake::app>;
using request_type  = typename fake_protocol::request_type;
static_assert(HTTPRequest<request_type>, "fake_request should be a http request");
using context_type = simple_context<request_type>;

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


    using request_type2 = typename fake_proto<fake::app>::request_type;
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
    context_type2 const         ctx2{req2};
    auto                        res = ctx2.create_response("test");
    EXPECT_EQ(res.body.as_string(), "test") << res.body.as_string();
}
