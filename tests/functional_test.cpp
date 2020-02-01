// Created by moisrex on 12/10/19.
#include <array>
#include <gtest/gtest.h>
#include <webpp/utils/functional.h>

using namespace webpp;

void test(int limit) {
    static auto i = 0;
    i++;
    EXPECT_TRUE(i < limit - 1) << "i is: " << i << "; limit: " << limit;
}

struct MyCallable {
    void operator()(int limit) {
        static auto i = 0;
        i++;
        EXPECT_TRUE(i < limit - 1) << "i is: " << i << "; limit: " << limit;
    }
};

TEST(FunctionalTests, DebouncedFunctions) {
    using namespace std::chrono;

    constexpr auto limit = 1000;

    constexpr auto checking_deduction_for_function_pointers = debounce_t(test);
    constexpr auto debounced_test = debounce_t(milliseconds(10), test);
    for (int i = 0; i < limit; i++)
        debounced_test(limit);

    // lambdas

    constexpr auto lambda_test = debounce_t([](int limit) {
        static auto i = 0;
        i++;
        EXPECT_LT(i, limit);
    });

    for (int i = 0; i < limit; i++)
        lambda_test(limit);

    // class

    constexpr debounce_t<MyCallable> debounced_class;
    for (int i = 0; i < limi; i++)
        debounced_class(limit);
}
