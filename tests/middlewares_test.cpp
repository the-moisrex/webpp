#include "../webpp/middleware/middleware_concepts.hpp"
#include "./common/test.hpp"

using namespace webpp;

struct middlware_one final : middleware_base<middlware_one> {
    bool triggered = false;

    void operator()() {
        triggered = true;
        // don't call next
    }
};

// Actually, this is a hook, not a middleware
struct hook_two final : event_base<hook_two, close_tag> {
    bool triggered = false;

    void operator()(close_tag) {
        triggered = true;
    }
};

struct middleware_three final : middleware_base<middleware_three> {
    bool triggered = false;

    void operator()() {
        triggered = true;
        next();
    }
};

TEST(MWTest, Basic) {
    middlewares_root root;
    middlware_one    one;
    root += one;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
}

TEST(MWTest, Duplicates) {
    middlewares_root root;
    hook_two         one;
    hook_two         two;
    root += one;
    root += two;
    root(on_close);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, Duplicates2) {
    middlewares_root root;
    middleware_three one;
    middleware_three two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, DuplicatesEvent) {
    middlewares_root root;
    middlware_one    one;
    middlware_one    two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_FALSE(two.triggered);
}
