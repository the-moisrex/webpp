// Created by moisrex on 12/10/19.

#include "../webpp/std/functional.hpp"

#include "../webpp/std/function_ref.hpp"
#include "../webpp/std/memory_resource.hpp"
#include "../webpp/utils/debounce.hpp"
#include "common/tests_common_pch.hpp"



using namespace webpp;
using namespace webpp::istl;

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
    using namespace stl::chrono;

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
    istl::function<int()> func_clone{stl::allocator<stl::byte>()};
    func_clone = func_copy;
    EXPECT_EQ(func_clone(), -369);
    func_copy = nullptr;
    EXPECT_FALSE(bool(func_copy));
    func_copy = stl::move(func_clone);
    EXPECT_EQ(func_copy(), -369);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
    istl::function<int()> func2 = [i = 10ul, big = stl::array<stl::size_t, 100>{}]() mutable {
        ++i;
        big[i % 30] = i;
        return big[i % 30];
    };
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
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
    stl::array<stl::byte, 500>            buff{};
    stl::pmr::monotonic_buffer_resource   res{buff.begin(), buff.size()};
    stl::pmr::polymorphic_allocator const alloc{&res};

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

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
    istl::pmr::function<int()> func2{[i = 10ul, big = stl::array<stl::size_t, 100>{}]() mutable {
                                         ++i;
                                         big[i % 30] = i;
                                         return big[i % 30];
                                     },
                                     alloc};
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
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

    stl::array<stl::byte, 1000>           buf{};
    stl::pmr::monotonic_buffer_resource   res{buf.begin(), buf.size()};
    stl::pmr::polymorphic_allocator const alloc{&res};

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
        istl::pmr::function<int()> caller = [] { // NOLINT(misc-non-private-member-variables-in-classes)
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

int mmmax(int a, int b) {
    return stl::max(a, b);
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

    object_type const one{.value = 101};
    object_type const two{.value = 102};

    view = one;
    EXPECT_EQ(view(), 101);
    view = two;
    EXPECT_EQ(view(), 102);

    function_ref<int(int, int)> view2;
    view2 = &mmmax;
    EXPECT_EQ(view2(10, 15), 15);

    function_ref<int(int, int)> const view3{&mmmax};
    EXPECT_EQ(view3(10, 15), 15);


    struct object_type_constness {
        int operator()() const {
            return 25;
        }
        int operator()() {
            return 26;
        }
    };

    object_type_constness const three;
    object_type_constness       four;
    four = three;
    view = three;
    EXPECT_EQ(view(), 25);
    view = four;
    EXPECT_EQ(view(), 26);
}

TEST(FunctionalTests, MemberFunctionRef) {
    struct const_op {
        constexpr int operator()() const {
            return 23;
        }
    };

    struct non_const_op {
        constexpr int operator()() const {
            return 21;
        }
    };

    struct both_const_op {
        constexpr int operator()() const {
            return 24;
        }

        constexpr int operator()() {
            return 25;
        }

        [[nodiscard]] int call() const { // NOLINT(readability-convert-member-functions-to-static)
            return 1;
        }
    };

    member_function_ref<int()> view;

    const_op op1{};
    view = op1;
    EXPECT_EQ(view(), 23);

    non_const_op op2{};
    view = op2;
    EXPECT_EQ(view(), 21);


    // Lambdas have implicit conversions to the signature, the ones I made above, don't
    //  class __lambda_11_19
    //  {
    //    public:
    //    inline /*constexpr */ int operator()(char, double) const
    //    {
    //      return 0;
    //    }
    //
    //    using retType_11_19 = int (*)(char, double);
    //    inline constexpr operator retType_11_19 () const noexcept
    //    {
    //      return __invoke;
    //    };
    //
    //    private:
    //    static inline /*constexpr */ int __invoke(char __param0, double __param1)
    //    {
    //      return __lambda_11_19{}.operator()(__param0, __param1);
    //    }
    //
    //
    //    public:
    //    // /*constexpr */ __lambda_11_19() = default;
    //
    //  };

    EXPECT_FALSE((stl::is_assignable_v<int (*&)(), const_op>) );
    EXPECT_FALSE((stl::is_assignable_v<int (*&)(), non_const_op>) );
    EXPECT_FALSE((stl::is_assignable_v<int (*&)(), both_const_op>) );
    // view = both_const_op{};
    EXPECT_EQ(view(), 25); // Should it call the non-const version?

    both_const_op const has_both{};
    view = has_both;
    EXPECT_EQ(view(), 24); // calls the const version

    view = &both_const_op::call;
    EXPECT_EQ(view(), 1);



    auto const lambda = [] {
        return 23;
    };
    view = lambda;
    EXPECT_EQ(view(), 23);


    member_function_ref<int()> const view_ctor{lambda};
    EXPECT_EQ(view_ctor(), 23);


    // this lambda is convertible to function pointer, so it should be allowed
    view = [] {
        return 222;
    };
    EXPECT_EQ(view(), 222);
}


////////////////////////////// ChatGPT Made tests (modified) //////////////////////////////


TEST(FunctionalTests, FunctionRefWithLambda) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    function_ref<int(int, int)> const ref(addFunc);
    EXPECT_EQ(ref(2, 3), 5);
}

TEST(FunctionalTests, FunctionRefWithCapture) {
    int  a       = 10;
    int  b       = 20;
    auto addFunc = [&a, &b]() -> int {
        return a + b;
    };
    function_ref<int()> const ref(addFunc);
    a = 5;
    b = 6;
    EXPECT_EQ(ref(), 11);
}

TEST(FunctionalTests, FunctionRefWithCharPointer) {
    auto charLengthFunc = [](const char* str) -> stl::size_t {
        return strlen(str);
    };
    function_ref<int(const char*)> const ref(charLengthFunc);
    EXPECT_EQ(ref("hello"), 5);
}

TEST(FunctionalTests, FunctionRefWithThreeIntParams) {
    auto addThreeFunc = [](int a, int b, int c) -> int {
        return a + b + c;
    };
    function_ref<int(int, int, int)> const ref(addThreeFunc);
    EXPECT_EQ(ref(1, 2, 3), 6);
}

TEST(FunctionalTests, FunctionRefWithMultiplyLambda) {
    auto multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)> const ref(multiplyFunc);
    EXPECT_EQ(ref(10, 20), 200);
}

TEST(FunctionalTests, CopyConstructor) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    function_ref<int(int, int)> const ref1(addFunc);
    function_ref<int(int, int)> const ref2(ref1);
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, CopyAssignment) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    function_ref<int(int, int)> const ref1(addFunc);
    auto                              multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)> ref2(multiplyFunc);
    ref2 = ref1;
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, ModifyFuncAfterAssign) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    auto multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)>       ref1(multiplyFunc);
    function_ref<int(int, int)> const ref2(addFunc);
    // After copy assignment ref1 should point to addFunc
    ref1 = ref2;
    EXPECT_EQ(ref1(2, 3), 5);
}

TEST(FunctionalTests, MoveConstructor) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    function_ref<int(int, int)>       ref1(addFunc);
    function_ref<int(int, int)> const ref2(stl::move(ref1));
    EXPECT_EQ(ref2(2, 3), 5);
}

TEST(FunctionalTests, MoveAssignment) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };
    auto multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)> ref1(multiplyFunc);
    function_ref<int(int, int)> ref2(addFunc);
    ref1 = stl::move(ref2);
    EXPECT_EQ(ref1(2, 3), 5);
    try {
        // Verify that ref2 has been moved
        ref2(2, 3);
        FAIL() << "Expected stl::bad_function_call";
    } catch (const stl::bad_function_call&) {
        SUCCEED();
    }
}

TEST(FunctionalTests, VoidReturnType) {
    int  result   = 0;
    auto voidFunc = [&result]() {
        result = 42;
    };
    function_ref<void()> const ref(voidFunc);
    ref();
    EXPECT_EQ(result, 42);
}

TEST(FunctionalTests, LambdaWithCapturedArgs) {
    int  a       = 10;
    auto addFunc = [&a](int b) -> int {
        return a + b;
    };
    function_ref<int(int)> const ref(addFunc);
    EXPECT_EQ(ref(20), 30);
}

TEST(FunctionalTests, EmptyFunctionRef) {
    function_ref<int()> const ref;
    try {
        // Calling an empty function_ref should cause a bad_function_call exception
        ref();
        FAIL() << "Expected std::bad_function_call";
    } catch (const stl::bad_function_call&) {
        SUCCEED();
    }
}

TEST(FunctionalTests, FunctionRefToPointToAnotherFunctionRef) {
    auto addFunc = [](int a, int b) -> int {
        return a + b;
    };

    // Create a new function_ref that points to addFunc
    function_ref<int(int, int)> const ref1(addFunc);

    // Create another function_ref that points to the first function_ref (ref1)
    function_ref<int(int, int)> const ref2(ref1);

    // Verify that they both produce the same result when called
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, FunctionRefToPointToStdFunction) {
    std::function<int(int, int)> func = [](int a, int b) -> int {
        return a + b;
    };
    function_ref<int(int, int)> const ref(func);
    EXPECT_EQ(ref(2, 3), 5);
}


int addFunc(int a, int b) {
    return a + b;
}

TEST(FunctionalTests, FunctionRefToPointToAnotherFunctionRefFunctionPointer) {
    // Create a new function_ref that points to addFunc function pointer
    function_ref<int(int, int)> const ref1(addFunc);

    // Create another function_ref that points to the first function_ref (ref1)
    function_ref<int(int, int)> const ref2(ref1);

    // Verify that they both produce the same result when called
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, FunctionRefToPointToStdFunctionFunctionPointer) {
    stl::function<int(int, int)>      fptr = addFunc;
    function_ref<int(int, int)> const ref(fptr);
    EXPECT_EQ(ref(2, 3), 5);
}


TEST(FunctionalTests, CopyConstructorFunctionPointer) {
    // Create a new function_ref that points to addFunc function pointer
    function_ref<int(int, int)> const ref1(addFunc);

    // Copy construct another function_ref from the first one (ref1)
    function_ref<int(int, int)> const ref2(ref1);

    // Verify that they both produce the same result when called
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, CopyAssignmentFunctionPointer) {
    // Create a new function_ref that points to addFunc function pointer
    function_ref<int(int, int)> const ref1(addFunc);

    // Create another function_ref that points to multiplyFunc
    auto multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)> ref2(multiplyFunc);

    // Assign ref1 to ref2
    ref2 = ref1;

    // Verify that they both produce the same result when called
    EXPECT_EQ(ref1(2, 3), ref2(2, 3));
}

TEST(FunctionalTests, ModifyFuncAfterAssignFunctionPointer) {
    // Create a new function_ref that points to multiplyFunc
    auto multiplyFunc = [](int a, int b) -> int {
        return a * b;
    };
    function_ref<int(int, int)> ref1(multiplyFunc);

    // Create another function_ref that points to addFunc function pointer
    function_ref<int(int, int)> const ref2(addFunc);

    // After copy assignment ref1 should point to addFunc function pointer
    ref1 = ref2;

    // Verify that they both produce the same result when called
    EXPECT_EQ(ref1(2, 3), 5);
}

TEST(FunctionalTests, MoveConstructorFunctionPointer) {
    // Create a new function_ref that points to addFunc function pointer
    function_ref<int(int, int)> ref1(addFunc);

    // Move construct another function_ref from the first one (ref1)
    function_ref<int(int, int)> const ref2(stl::move(ref1));

    // Verify that the moved function_ref produces the expected result
    EXPECT_EQ(ref2(2, 3), 5);
}




////////////////////////////// ChatGPT Generated Tests for member_function_ref //////////////////////////////


// define a test fixture
class MemberFunctionPtrTest : public ::testing::Test {
  protected:
    struct TestStruct {
        int  x;
        void foo(int n) {
            x = n;
        }
        [[nodiscard]] int bar(int n) const {
            return x + n;
        }
    };
};

// test the basic functionality of member_function_ref
TEST_F(MemberFunctionPtrTest, BasicTest) {
    using MemberFoo         = member_function_ref<void (TestStruct::*)(int)>;
    MemberFoo const foo_ptr = &TestStruct::foo;

    TestStruct obj{0};
    foo_ptr(obj, 42);
    EXPECT_EQ(obj.x, 42);

    using MemberBar         = member_function_ref<int (TestStruct::*)(int) const>;
    MemberBar const bar_ptr = &TestStruct::bar;
    EXPECT_EQ(bar_ptr(obj, 10), 52);
}

// test the copy/move constructor and assignment operators of member_function_ref
TEST_F(MemberFunctionPtrTest, CopyAndMoveTest) {
    using MemberFoo    = member_function_ref<void (TestStruct::*)(int)>;
    MemberFoo foo_ptr1 = &TestStruct::foo;
    MemberFoo foo_ptr2 = foo_ptr1;
    MemberFoo foo_ptr3 = std::move(foo_ptr1);
    foo_ptr1           = &TestStruct::foo;
    foo_ptr2           = foo_ptr1;
    foo_ptr3           = std::move(foo_ptr2);

    TestStruct obj{0};
    foo_ptr1(obj, 42);
    EXPECT_EQ(obj.x, 42);
    foo_ptr3(obj, 10);
    EXPECT_EQ(obj.x, 10);
}


// define a test fixture
class MemberFunctionPtrTest2 : public ::testing::Test {
  protected:
    static int add_func(int a, int b) {
        return a + b;
    }
    static bool is_even(int n) {
        return n % 2 == 0;
    }
};

// test the basic functionality of function_pointer
TEST_F(MemberFunctionPtrTest2, BasicTest) {
    using AddFunc         = member_function_ref<int(int, int)>;
    AddFunc const add_ptr = &MemberFunctionPtrTest2::add_func;

    EXPECT_EQ(add_ptr(2, 3), 5);
    EXPECT_EQ(add_ptr(-1, 1), 0);

    using IsEvenFunc             = member_function_ref<bool (*)(int)>;
    IsEvenFunc const is_even_ptr = &MemberFunctionPtrTest2::is_even;

    EXPECT_TRUE(is_even_ptr(0));
    EXPECT_TRUE(is_even_ptr(-2));
    EXPECT_FALSE(is_even_ptr(7));
}


// define a test fixture
class MemberFunctionRefTest : public ::testing::Test {
  protected:
    struct TestStruct {
        int  x; // NOLINT(misc-non-private-member-variables-in-classes)
        void foo(int n) {
            x = n;
        }
        [[nodiscard]] int bar(int n) const {
            return x + n;
        }
    };
    static int add_func(int a, int b) {
        return a + b;
    }
};

// test the basic functionality of member_function_ref with member function pointer
TEST_F(MemberFunctionRefTest, MemberFunctionPtrTest) {
    using MemberFooPtr         = member_function_ref<void (TestStruct::*)(int)>;
    MemberFooPtr const foo_ptr = &TestStruct::foo;

    TestStruct obj{0};
    foo_ptr(obj, 42);
    EXPECT_EQ(obj.x, 42);

    using MemberBarPtr         = member_function_ref<int (TestStruct::*)(int) const>;
    MemberBarPtr const bar_ptr = &TestStruct::bar;
    EXPECT_EQ(bar_ptr(obj, 10), 52);


    using KindaMemberBarPtr               = member_function_ref<int(int) const>;
    KindaMemberBarPtr const kinda_bar_ptr = &TestStruct::bar;
    EXPECT_EQ(kinda_bar_ptr(obj, 10), 52);


    using KindaMemberBarPtr2 = member_function_ref<int(int) const>;
    KindaMemberBarPtr2 const kinda_bar_ptr2{obj, &TestStruct::bar};
    EXPECT_EQ(kinda_bar_ptr2(10), 52);


    using KindaMemberBarPtr3 = member_function_ref<int(int)>;
    KindaMemberBarPtr3 const kinda_bar_ptr3{obj, &TestStruct::bar};
    EXPECT_EQ(kinda_bar_ptr3(10), 52);
}

// test the basic functionality of member_function_ref with function pointer
TEST_F(MemberFunctionRefTest, FunctionPtrTest) {
    using AddFuncPtr         = member_function_ref<int (*)(int, int)>;
    AddFuncPtr const add_ptr = &MemberFunctionRefTest::add_func;

    EXPECT_EQ(add_ptr(2, 3), 5);
    EXPECT_EQ(add_ptr(-1, 1), 0);
}

// test the basic functionality of member_function_ref with function_ref
TEST_F(MemberFunctionRefTest, FunctionRefTest) {
    auto ddd = [](int& x) {
        x = 42;
    };
    static_assert(stl::is_assignable_v<void (*&)(int&), decltype(ddd)>, "It should be");
    static_assert(stl::is_invocable_r_v<void, decltype(ddd), int&>, "It should be");
    static_assert(
      istl::details::is_specialization_of_mem_fun_ref_v<member_function_ref<void(int&)>, member_function_ref>,
      "It should be");
    static_assert(!istl::details::is_specialization_of_mem_fun_ref_v<stl::remove_cvref_t<decltype(ddd)>,
                                                                     member_function_ref>,
                  "It should not be");


    function_ref<void (*)(int&)> const ref0 = ddd;
    function_ref<void(int&)> const     ref  = [](int& x) {
        x = 42;
    };
    member_function_ref<void(int&)> const mf_ref0     = ddd;
    member_function_ref<void(int&)> const mf_ref      = ref;
    member_function_ref<void(int&)> const mf_conv_cpy = mf_ref;

    int x = 0;
    mf_ref(x);
    EXPECT_EQ(x, 42);

    x = 0;
    mf_ref0(x);
    EXPECT_EQ(x, 42);

    x = 0;
    mf_conv_cpy(x);
    EXPECT_EQ(x, 42);

    x = 0;
    ref0(x);
    EXPECT_EQ(x, 42);
}


// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
