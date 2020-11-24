// Created by moisrex on 12/10/19.

#include "./common_pch.hpp"

// clang-format off
#include webpp_include(utils/functional)
#include webpp_include(utils/debounce)
// clang-format on


using namespace webpp;

void test(int limit) {
    static auto i = 0;
    i++;
    EXPECT_TRUE(i < limit - 1) << "i is: " << i << "; limit: " << limit;
}

struct ConstMyCallable {
    void operator()(int limit) const {
        static auto i = 0;
        i++;
        EXPECT_TRUE(i < limit - 1) << "i is: " << i << "; limit: " << limit;
    }
};

struct MyCallable {
    int  i = 0;
    auto operator()(int limit) {
        i++;
        EXPECT_TRUE(i < limit - 1) << "i is: " << i << "; limit: " << limit;
        return i;
    }
};

TEST(FunctionalTests, DebouncedFunctions) {
    using namespace std::chrono;

    constexpr auto limit = 1000;

    auto checking_deduction_for_function_pointers = debounce(test);
    auto debounced_test = debounce(milliseconds(10), test);
    for (int i = 0; i < limit; i++)
        debounced_test(limit);

    // lambdas

    auto lambda_test = debounce([](int limit) {
        static auto i = 0;
        i++;
        EXPECT_LT(i, limit);
    });

    for (int i = 0; i < limit; i++)
        lambda_test(limit);

    // class

    debounce_t<MyCallable> debounced_class(milliseconds(1));
    for (int i = 0; i < limit; i++) {
        auto res = debounced_class(limit);
        EXPECT_LT(res, limit) << res;
    }

    const debounce<ConstMyCallable> const_debounced_class;
    for (int i = 0; i < limit; i++)
        const_debounced_class(limit);
}


TEST(FunctionalTests, TrailingMode) {
    using dtype = debounce_type;

    debounce_trailing debounced_lambda([] {

    });
}
