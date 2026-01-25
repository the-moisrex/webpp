#include "../webpp/traits/lbg.hpp"

#include "common/test.hpp"

using namespace webpp;

static constexpr struct context_type : simple_local_registry<context_type> {
    static constexpr registry self{};

    std::string_view request;
    std::string_view response;

    [[nodiscard]] std::string_view req() const noexcept {
        return self->request;
    }

    [[nodiscard]] std::string_view res() const noexcept {
        return self->response;
    }
} context;

TEST(LBGTest, Basic) {
    context_type ctx{.request = "req1", .response = "res1"};
    lbg_scope    scope{context, ctx};
    EXPECT_EQ(context.req(), "req1");
    EXPECT_EQ(context.res(), "res1");
}
