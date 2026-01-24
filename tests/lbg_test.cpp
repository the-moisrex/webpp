#include "../webpp/traits/lbg.hpp"

#include "common/test.hpp"

using namespace webpp;

static constexpr struct context_type : simple_registry<context_type> {
    std::string_view request;
    std::string_view response;
} context;


TEST(LBGTest, Basic) {
    context_type ctx{.request = "req1", .response = "res1"};
    lbg_scope    scope{context, ctx};
    EXPECT_EQ(context.request, "req1");
    EXPECT_EQ(context.response, "res1");
}
