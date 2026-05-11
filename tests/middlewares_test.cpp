#include "../webpp/middleware/middleware_concepts.hpp"
#include "./common/test.hpp"

using namespace webpp;

struct one_middleware final : middleware_base<one_middleware, middleware_tag> {
    bool triggered = false;

    void operator()(middleware_tag) {
        triggered = true;
    }
};

TEST(MWTest, Basic) {
    middlewares_root root;
    one_middleware   one;
    root += one;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
}

TEST(MWTest, Duplicates) {
    middlewares_root root;
    one_middleware   one;
    one_middleware   two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}
