#include "../webpp/middleware/middleware_concepts.hpp"
#include "./common/test.hpp"

using namespace webpp;

struct one_middleware final : middleware_base<one_middleware> {
    bool triggered = false;

    void operator()(middleware_tag) {
        triggered = true;
    }
};

TEST(MWTest, Basic) {
    middlewares_root root;
    one_middleware   one;
    root.register_middleware(&one);
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
}
