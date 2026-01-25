#include "../webpp/traits/lbg.hpp"

#include "common/test.hpp"

using namespace webpp;

static constexpr struct context_type {
    std::string_view request;
    std::string_view response;

    [[nodiscard]] std::string_view req() const noexcept {
        auto& self = simple_local_registry<context_type>::instance();
        return self->request;
    }

    [[nodiscard]] std::string_view res() const noexcept {
        auto& self = simple_local_registry<context_type>::instance();
        return self->response;
    }
} context;

TEST(LBGTest, Basic) {
    context_type ctx{.request = "req1", .response = "res1"};
    lbg_scope    scope{context, ctx};
    EXPECT_EQ(context.req(), "req1");
    EXPECT_EQ(context.res(), "res1");
}
