// Created by moisrex on 12/10/19.

#include "../core/include/webpp/utils/functional.hpp"

#include "../core/include/webpp/std/functional.hpp"
#include "../core/include/webpp/std/memory_resource.hpp"
#include "../core/include/webpp/utils/debounce.hpp"
#include "common_pch.hpp"



using namespace webpp;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

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
    auto debounced_test                           = debounce(milliseconds(10), test);
    for (int i = 0; i < limit; i++)
        debounced_test(limit);

    // lambdas

    auto lambda_test = debounce([](int _limit) {
        static auto i = 0;
        i++;
        EXPECT_LT(i, _limit);
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
    // using dtype = debounce_type;

    debounce_trailing debounced_lambda([] {

    });
}


TEST(FunctionalTests, FunctionWithSTDAllocators) {
    istl::function<int()> func = [i = 0]() mutable {
        return ++i;
    };
    EXPECT_EQ(1, func());
    EXPECT_EQ(2, func());
    EXPECT_EQ(3, func());
    func = [] {
        return 2;
    };
    EXPECT_EQ(2, func());
    EXPECT_EQ(2, func());
    func = [i = 0]() mutable {
        return ++i;
    };
    EXPECT_EQ(1, func());
    EXPECT_EQ(2, func());
    EXPECT_EQ(3, func());
    func = +[] {
        return -369;
    };
    EXPECT_EQ(-369, func());

    istl::function<int()> func_copy = func;
    EXPECT_EQ(-369, func());
    EXPECT_EQ(func_copy(), -369);
    istl::function<int()> func_clone{std::allocator<stl::byte>()};
    func_clone = func_copy;
    EXPECT_EQ(func_clone(), -369);
    func_copy = nullptr;
    EXPECT_FALSE(bool(func_copy));
    func_copy = stl::move(func_clone);
    EXPECT_EQ(func_copy(), -369);

    istl::function<int()> func2 = [i = 10ul, big = stl::array<stl::size_t, 100>{}]() mutable {
        ++i;
        big[i % 30] = i;
        return big[i % 30];
    };
    EXPECT_EQ(11, func2());
    EXPECT_EQ(-369, func());
    // EXPECT_NE(func, func2);
    func = func2;
    // EXPECT_EQ(func, func2);
    EXPECT_EQ(12, func2());
    EXPECT_EQ(12, func());
    EXPECT_EQ(13, func());
}


TEST(FunctionalTests, FunctionWithPMRAllocators) {
    stl::array<stl::byte, 500>          buff{};
    stl::pmr::monotonic_buffer_resource res{buff.begin(), buff.size()};
    stl::pmr::polymorphic_allocator     alloc{&res};

    istl::pmr::function<int()> func{[i = 0]() mutable {
                                        return ++i;
                                    },
                                    alloc};
    EXPECT_EQ(1, func());
    EXPECT_EQ(2, func());
    EXPECT_EQ(3, func());
    func = [] {
        return 2;
    };
    EXPECT_EQ(2, func());
    EXPECT_EQ(2, func());
    func = [i = 0]() mutable {
        return ++i;
    };
    EXPECT_EQ(1, func());
    EXPECT_EQ(2, func());
    EXPECT_EQ(3, func());
    func = +[] {
        return -369;
    };
    EXPECT_EQ(-369, func());

    istl::pmr::function<int()> func_copy = func;
    EXPECT_EQ(-369, func());
    EXPECT_EQ(func_copy(), -369);
    istl::pmr::function<int()> func_clone;
    func_clone = func_copy;
    EXPECT_EQ(func_clone(), -369);
    func_copy = nullptr;
    EXPECT_FALSE(bool(func_copy));
    func_copy = stl::move(func_clone);
    EXPECT_EQ(func_copy(), -369);

    istl::pmr::function<int()> func2{[i = 10ul, big = stl::array<stl::size_t, 100>{}]() mutable {
                                         ++i;
                                         big[i % 30] = i;
                                         return big[i % 30];
                                     },
                                     alloc};
    EXPECT_EQ(11, func2());
    EXPECT_EQ(-369, func());
    // EXPECT_NE(func, func2);
    func = func2;
    // EXPECT_EQ(func, func2);
    EXPECT_EQ(12, func2());
    EXPECT_EQ(12, func());
    EXPECT_EQ(13, func());
}

TEST(FunctionalTests, DoubleFreeFunction) {
    stl::function<int()> func = [] {
        return 100;
    };

    func.~function();
    func.~function();
    func.~function();
    func.~function();

    istl::function<int()> ifunc = [] {
        return 100;
    };

    stl::array<stl::byte, 1000>         buf{};
    stl::pmr::monotonic_buffer_resource res{buf.begin(), buf.size()};
    stl::pmr::polymorphic_allocator     alloc{&res};

    istl::pmr::function<int()> ifunc2{stl::allocator_arg_t{}, alloc, [i = 9]() mutable {
                                          return ++i;
                                      }};
    EXPECT_EQ(10, ifunc2());
    EXPECT_EQ(11, ifunc2());
    ifunc2 = ifunc;
    EXPECT_EQ(100, ifunc2());
    ifunc2.~function();
    ifunc2.~function();
    ifunc2.~function();

    ifunc.~function();
    ifunc.~function();
    ifunc.~function();
    ifunc.~function();

    stl::pmr::vector<istl::pmr::function<int()>> vec;
    vec.emplace_back([i = 0]() mutable {
        return ++i;
    });
    struct item_type {
        istl::pmr::function<int()> caller = [] {
            return 20;
        };
        int operator()() {
            return caller();
        }
    };
    item_type item;
    item.caller = item_type{};
    EXPECT_EQ(20, item());
    vec.emplace_back(istl::pmr::function<long()>(item));
    EXPECT_EQ(vec.back()(), 20);
}

TEST(FunctionalTests, FunctionRefTests) {
    function_ref<int()> view;

    function_ref<int()> const view40{[]() noexcept(false) -> int {
        return 40;
    }};

    auto ret20 = [] {
        return 20;
    };
    view = ret20;
    EXPECT_EQ(view40(), 40);
    EXPECT_EQ(view(), 20);

    view = [] {
        return 30;
    };

    EXPECT_EQ(view(), 30);

    struct object_type {
        int value; // NOLINT(misc-non-private-member-variables-in-classes)

        int operator()() const {
            return value;
        }
    };

    object_type one{.value = 101};
    object_type two{.value = 102};

    view = one;
    EXPECT_EQ(view(), 101);
    view = two;
    EXPECT_EQ(view(), 102);
}


// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
