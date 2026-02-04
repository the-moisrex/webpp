#include "../webpp/traits/dynamic_scoping.hpp"

#include "common/test.hpp"

using namespace webpp;

static constexpr struct context_type : global_binding<context_type> {
    static constexpr global_binding self{};

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
    dynamic_scope    scope{ctx};
    EXPECT_EQ(context.req(), "req1");
    EXPECT_EQ(context.res(), "res1");
}

TEST(LBGTest, Nested) {
    // layer 1:
    {
        context_type ctx1{.request = "req1", .response = "res1"};
        dynamic_scope    scope1{ctx1};
        EXPECT_EQ(context.req(), "req1");
        EXPECT_EQ(context.res(), "res1");

        // layer 2:
        {
            context_type ctx2{.request = "req2", .response = "res2"};
            dynamic_scope    scope2{ctx2};
            EXPECT_EQ(context.req(), "req2");
            EXPECT_EQ(context.res(), "res2");
        }

        // back to layer 1:
        EXPECT_EQ(context.req(), "req1");
        EXPECT_EQ(context.res(), "res1");
    }
}
