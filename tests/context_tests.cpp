// Created by moisrex on 7/1/20.
#include "../core/include/webpp/http/interfaces/cgi.h"
#include "../core/include/webpp/http/request.h"
#include "../core/include/webpp/http/routes/context.h"
#include "../core/include/webpp/http/routes/literals.h"
#include "../core/include/webpp/http/routes/methods.h"
#include "../core/include/webpp/http/routes/path.h"
#include "../core/include/webpp/http/routes/path/number.h"
#include "../core/include/webpp/traits/std_traits.h"

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

using context_type = simple_context<basic_request<std_traits, cgi<std_traits, fake_app>>>;
using request_type = basic_request<std_traits, cgi<std_traits, fake_app>>;

struct fake_mommy {
    template <typename TraitsType>
    struct type {
        bool test = true;
    };
};

TEST(Routes, Path) {
    using namespace webpp::routes;
    request_type req;
    context_type ctx{req};

    auto nctx = ctx.template clone<fake_mommy>();
    using nctx_type = decltype(nctx);
    EXPECT_TRUE(nctx.test);
}

