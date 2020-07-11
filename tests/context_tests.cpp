// Created by moisrex on 7/1/20.
#include "../core/include/webpp/http/interfaces/cgi.hpp"
#include "../core/include/webpp/http/request.hpp"
#include "../core/include/webpp/http/routes/context.hpp"
#include "../core/include/webpp/http/routes/literals.hpp"
#include "../core/include/webpp/http/routes/methods.hpp"
#include "../core/include/webpp/http/routes/path.hpp"
#include "../core/include/webpp/http/routes/path/number.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"

#include <gtest/gtest.h>
#include <string>
#include <utility>

using namespace webpp;
using namespace webpp::routes;

struct fake_app {

    router<> _router{

    };

    Response auto operator()(Request auto&& req) {
        return _router(req);
    }
};

using request_type = basic_request<std_traits, cgi<std_traits, fake_app>>;
using context_type = simple_context<request_type>;

struct fake_mommy {
    template <typename TraitsType>
    struct type {
        bool test = true;
    };
};

TEST(Routes, PathTests) {
    using namespace webpp::routes;
    request_type req;
    context_type ctx{req};

    auto nctx = ctx.template clone<fake_mommy>();
    using nctx_type = decltype(nctx);
    EXPECT_TRUE(nctx.test);
}

