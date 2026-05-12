#include "../webpp/middleware/middleware_concepts.hpp"
#include "./common/test.hpp"

using namespace webpp;

struct one_middleware final : middleware_base<one_middleware, middleware_tag> {
    bool triggered = false;

    void operator()(middleware_tag) {
        triggered = true;
        // don't call next
    }
};

struct two_middleware final : middleware_base<two_middleware, close_tag> {
    bool triggered = false;

    void operator()(close_tag) {
        triggered = true;
    }
};

struct three_middleware final : middleware_base<three_middleware, middleware_tag> {
    bool triggered = false;

    void operator()(middleware_tag) {
        triggered = true;
        next(on_middleware);
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
    two_middleware   one;
    two_middleware   two;
    root += one;
    root += two;
    root(on_close);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, Duplicates2) {
    middlewares_root root;
    three_middleware one;
    three_middleware two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, DuplicatesEvent) {
    middlewares_root root;
    one_middleware   one;
    one_middleware   two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_FALSE(two.triggered);
}
