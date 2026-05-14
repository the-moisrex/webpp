#include "../webpp/middleware/middleware_concepts.hpp"
#include "./common/test.hpp"

using namespace webpp;

struct middleware_one final : middleware<middleware_one> {
    bool triggered = false;

    void operator()() {
        triggered = true;
        // don't call next
    }
};

// Actually, this is a hook, not a middleware
struct hook_two final : event<hook_two, close_tag> {
    bool triggered = false;

    void operator()(close_tag) {
        triggered = true;
    }
};

struct middleware_three final : middleware<middleware_three> {
    bool triggered = false;

    void operator()() {
        triggered = true;
        next();
    }
};

struct middleware_four final : event<middleware_four, close_tag, middleware_tag> {
    bool mw_triggered    = false;
    bool close_triggered = false;

    void operator()() {
        mw_triggered = true;
        next(on_middleware);
    }

    void operator()(close_tag) {
        close_triggered = true;
    }
};

// Two-way middleware: pre -> next -> post
struct twoway_mw final : middleware<twoway_mw> {
    bool pre_called  = false;
    bool post_called = false;
    int  stage       = 0;

    void pre() {
        pre_called = true;
        stage      = 1;
    }

    void post() {
        post_called = true;
        stage       = 3;
    }
};

struct onion_mw final : middleware<onion_mw> {
    int* stage  = nullptr;
    bool called = false;

    void operator()() {
        called = true;
        if (stage) {
            // should be called after pre() and before post()
            EXPECT_EQ(*stage, 1);
            *stage = 2;
        }
        next();
    }
};

TEST(MWTest, Basic) {
    events_root    root;
    middleware_one one;
    root += one;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
}

TEST(MWTest, Duplicates) {
    events_root root;
    hook_two    one;
    hook_two    two;
    root += one;
    root += two;
    root(on_close);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, Duplicates2) {
    events_root      root;
    middleware_three one;
    middleware_three two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, DuplicatesEvent) {
    events_root    root;
    middleware_one one;
    middleware_one two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_FALSE(two.triggered);
}

TEST(MWTest, Multi) {
    events_root    root;
    middleware_one one;
    hook_two       two;
    root += one;
    root += two;
    root(on_middleware);
    EXPECT_TRUE(one.triggered);
    EXPECT_FALSE(two.triggered);
    root(on_close);
    EXPECT_TRUE(one.triggered);
    EXPECT_TRUE(two.triggered);
}

TEST(MWTest, MultiType) {
    events_root     root;
    middleware_four one;
    root += one;
    root(on_middleware);
    EXPECT_TRUE(one.mw_triggered);
    EXPECT_FALSE(one.close_triggered);
    root(on_close);
    EXPECT_TRUE(one.mw_triggered);
    EXPECT_TRUE(one.close_triggered);
}

TEST(MWTest, TwoWayPrePostStyle) {
    events_root root;
    twoway_mw   one;
    onion_mw    two;
    two.stage = &one.stage;

    root += one;
    root += two;

    root(on_middleware);
    EXPECT_TRUE(one.pre_called);
    EXPECT_TRUE(one.post_called);
    EXPECT_TRUE(two.called);
    EXPECT_EQ(one.stage, 3);
}
