// Created by moisrex on 12/17/20.

#include "../webpp/std/type_traits.hpp"

#include "../webpp/std/concepts.hpp"
#include "../webpp/std/optional.hpp"
#include "../webpp/std/tuple.hpp"
#include "common/tests_common_pch.hpp"

#include <cmath>
#include <functional>
#include <list>
#include <map>
#include <string>

#if __cpp_lib_memory_resource
#    include <memory_resource>
#endif

using namespace std;
using namespace webpp::istl;

// NOLINTBEGIN(*-magic-numbers)

///////////////////////////////////////////// replace_parameter /////////////////////////////////////////////

///////////////////////////////////////////
// changing std::string's allocator ///////
///////////////////////////////////////////
#if __cpp_lib_memory_resource
using new_str_t = replace_parameter<string, std::allocator<char>, pmr::polymorphic_allocator<char>>;

static_assert(std::is_same_v<new_str_t, pmr::string>);


using should_be_void = replace_parameter<string, std::allocator<int>, pmr::polymorphic_allocator<char>>;

// static_assert(is_void_v<should_be_void>);

#endif


///////////////////// Tuple ///////////////////


template <typename... T>
struct fake_tuple {};

// tuple_filter
static_assert(is_same_v<filter_parameters_t<is_void, fake_tuple<int, void, int, void, double, void>>,
                        fake_tuple<void, void, void>>);



#if __cpp_lib_memory_resource
using t1  = tuple<int, string, vector<int>, int>;
using t2  = replace_parameter<t1, int, double>;
using t3  = recursively_replace_parameter<t1, int, double>;
using t4  = recursively_replace_templated_parameter<t1, allocator, pmr::polymorphic_allocator>;
using t5  = tuple<vector<string>>;
using t6  = recursively_replace_templated_parameter<t5, allocator, pmr::polymorphic_allocator>;
using t7  = replace_templated_parameter<t1, vector, list>;
using t8  = tuple<map<vector<list<string>>, vector<string>>>;
using t9  = recursively_replace_templated_parameter<t8, allocator, pmr::polymorphic_allocator>;
using t10 = tuple<map<string, string>>;
using t11 = recursively_replace_templated_parameter<t10, allocator, pmr::polymorphic_allocator>;
using t12 = tuple<const allocator<char>>;
using t13 = recursively_replace_templated_parameter<t12, allocator, pmr::polymorphic_allocator>;
using t14 = tuple<const tuple<allocator<char>>>;
using t15 = recursively_replace_templated_parameter<t14, allocator, pmr::polymorphic_allocator>;

static_assert(is_same_v<t2, tuple<double, string, vector<int>, double>>);
static_assert(is_same_v<t3, tuple<double, string, vector<double>, double>>);
static_assert(is_same_v<t4, tuple<int, pmr::string, pmr::vector<int>, int>>);
static_assert(is_same_v<t6, tuple<pmr::vector<pmr::string>>>);
static_assert(is_same_v<t7, tuple<int, string, list<int>, int>>);
static_assert(is_same_v<t13, tuple<const pmr::polymorphic_allocator<char>>>);
static_assert(is_same_v<t15, tuple<const tuple<pmr::polymorphic_allocator<char>>>>);
static_assert(is_same_v<t11, tuple<pmr::map<pmr::string, pmr::string>>>);
static_assert(is_same_v<t9, tuple<pmr::map<pmr::vector<pmr::list<pmr::string>>, pmr::vector<pmr::string>>>>);


template <typename T>
struct int_replacer {
    static constexpr bool value = is_same_v<T, int>;
    using type                  = double;
};

using t16 = recursive_parameter_replacer<t1, int_replacer>;
static_assert(is_same_v<t16, tuple<double, string, vector<double>, double>>);




using ut = unique_parameters<tuple<void, void, int, int, int>>;
static_assert(is_same_v<ut, tuple<void, int>>);


#endif

/// Optional ///

static_assert(Optional<optional<int>>, "Optional doesn't work properly");



/// Last Type ///

template <typename T>
using is_not_integral = negation<is_integral<T>>;

using only_int   = typename last_type<int, double>::template remove<tuple>;
using only_ints  = typename last_type<int, short, string>::template remove_if<tuple, is_not_integral>;
using only_arith = typename last_type<int, double, short, string>::template remove<tuple>;
using only_arith_long =
  typename last_type<int, unsigned, unsigned short, long, long long, unsigned long, double, short, string>::
    template remove<tuple>;

static_assert(is_same_v<only_int, tuple<int>>, "remove bug");
static_assert(is_same_v<only_arith, tuple<int, double, short>>, "remove bug");
static_assert(is_same_v<only_arith_long,
                        tuple<int, unsigned, unsigned short, long, long long, unsigned long, double, short>>,
              "remove bug");
static_assert(is_same_v<only_ints, tuple<int, short>>, "remove if bug");

using one_int = typename last_type<int, double, int, string>::template remove_limit<tuple, 1>;
static_assert(is_same_v<one_int, tuple<int>>, "remove limit bug");



/// ituple


TEST(TypeTraits, ITupleTest) {

    // 1.3 should be converted to 1 because it's int
    auto const tup  = ituple<int, double>{1.3, 1.1};
    auto const tup3 = tup.structured<3>();
    auto const tup1 = tup.structured<1>();

    // structured binding works both ways
    auto const [mi_int, mi_double, mi_nothing] = tup3;
    auto const [mi_int1]                       = tup1;

    static_assert(is_same_v<int, remove_cvref_t<decltype(mi_int)>>, "it should be int");
    static_assert(is_same_v<double, remove_cvref_t<decltype(mi_double)>>, "it should be double");
    static_assert(is_same_v<nothing_type, remove_cvref_t<decltype(mi_nothing)>>, "it should be nothing");

    EXPECT_EQ(1, get<0>(tup));
    EXPECT_EQ(1.1, get<1>(tup));
    EXPECT_EQ(1, get<0>(tup3));
    EXPECT_EQ(1.1, get<1>(tup3));
    EXPECT_EQ(1, mi_int);
    EXPECT_EQ(1.1, mi_double);

    EXPECT_EQ(1, mi_int1);
    EXPECT_EQ(1, tuple_size_v<decltype(tup1)>);
}


template <template <typename...> typename Tup = ituple>
struct iterable_type {
    using vec      = list<Tup<int, double>>;
    using vec_iter = typename vec::iterator;
    using iterator = ituple_iterator<vec_iter>;

  private:
    vec data;

  public:
    iterable_type() {
        data.push_back(Tup{1, 1.1});
        data.push_back(Tup{2, 1.2});
        data.push_back(Tup{3, 1.3});
    }

    iterator begin() {
        return data.begin();
    }
    iterator end() {
        return data.end();
    }
};

TEST(TypeTraits, ITupleIteratorTest) {
    iterable_type<ituple> vecs;

    static_assert(
      is_same_v<remove_cvref_t<decltype(*vecs.begin())>, ituple<int, double, default_ituple_options<0>>>,
      "should return ituple no matter what");

    int    i = 0;
    double d = 1.0;
    for (auto [int_val, double_val] : vecs) {
        d += 0.1;
        ++i;
        static_assert(is_same_v<remove_cvref_t<decltype(int_val)>, int>, "it should be int");
        static_assert(is_same_v<remove_cvref_t<decltype(double_val)>, double>, "it should be double");
        EXPECT_EQ(int_val, i);
        double const zero = std::fabs(double_val - d);
        EXPECT_LE(zero, numeric_limits<double>::epsilon());
    }
}

TEST(TypeTraits, ITupleIteratorTestWithTuple) {
    iterable_type<tuple> vecs;

    int    i = 0;
    double d = 1.0;
    for (auto [int_val, double_val] : vecs) {
        d += 0.1; // NOLINT(*-magic-numbers)
        ++i;
        static_assert(is_same_v<remove_cvref_t<decltype(int_val)>, int>, "it should be int");
        static_assert(is_same_v<remove_cvref_t<decltype(double_val)>, double>, "it should be double");
        EXPECT_EQ(int_val, i);
        double const zero = std::fabs(double_val - d);
        EXPECT_LE(zero, numeric_limits<double>::epsilon());
    }
}


TEST(TypeTraits, OneOfTest) {
    EXPECT_TRUE(bool(one_of<int, double, int>));
    EXPECT_FALSE(bool(one_of<float, double, int>));
    EXPECT_FALSE(bool(one_of<float, short, unsigned, double, int>));
    EXPECT_TRUE(bool(one_of<float, short, unsigned, int, double, int>));
}


struct one {};
struct two {};
struct three {};
struct four {};
TEST(TypeTraits, InvocableInOrder) {
    using std::function;
    using webpp::istl::invocable_inorder_v;

    EXPECT_TRUE(bool(invocable_inorder_v<function<void()>>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(int)>, int>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(int, double)>, int, double>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(int, int)>, int, int, int>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(double, int)>, int, double, int>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(one, one)>, one, one, two>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(one, one)>, two, one, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(one, two)>, two, one, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(one, two, three)>, two, three, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(four, one, two, three)>, two, four, three, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(four, two, three, one)>, two, four, three, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(two, four, three, one)>, two, four, three, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(two, two, three, one)>, two, four, two, three, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<function<void(two, four, two, one)>, two, four, two, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<void(two, four, two, one), two, four, two, one>));
    EXPECT_FALSE(bool(invocable_inorder_v<void(two, two, four), two, two, one>));
    EXPECT_TRUE(bool(invocable_inorder_v<void(two, two const&, four), two, two&, one, four&>));
    EXPECT_TRUE(bool(invocable_inorder_v<void(two const&, four const&), two&, one, four&&>));
    EXPECT_FALSE(bool(invocable_inorder_v<void(two&, four const&), two const&, one, four&&>));

    auto const ok = [](one, two, three) -> bool {
        return true;
    };
    EXPECT_TRUE(invoke_inorder(ok, three{}, two{}, one{}));
    EXPECT_TRUE(invoke_inorder(ok, one{}, two{}, three{}));
    EXPECT_TRUE(invoke_inorder(ok, three{}, one{}, two{}));
}


TEST(TypeTraits, SameAsAllTest) {
    EXPECT_TRUE(bool(same_as_all<>));
    EXPECT_FALSE(bool(same_as_all<one>));
    EXPECT_TRUE(bool(same_as_all<one, one>));
    EXPECT_TRUE(bool(same_as_all<one, two, three, one, two, three>));
    EXPECT_TRUE(bool(cvref_as<one const&, two, three const&, one, two, three>));
    EXPECT_TRUE(bool(cvref_as<one const&, one&&>));
    EXPECT_TRUE(bool(cvref_as<one const&, two, three const&, one, two, three const>));
    EXPECT_TRUE(bool(cvref_as<one const&, two, three const&, one, two, three&&>));
    EXPECT_FALSE(bool(same_as_all<one, two, three, one, four, three>));
    EXPECT_FALSE(bool(cvref_as<one const&, four, three const&, one, two, three>));
    EXPECT_FALSE(bool(same_as_all<one, two, three, one, one, three>));
    EXPECT_FALSE(bool(cvref_as<one const&, one, three const&, one, two, three>));
}

template <typename T>
using is_one = is_same<T, one>;


TEST(TypeTraits, IndexesIfTest) {
    EXPECT_TRUE(bool(same_as<indexes_if<is_one, two, one, three>, index_sequence<1>>));
    EXPECT_TRUE(bool(same_as<indexes_if<is_one, one, one, four, three, two, three>, index_sequence<0, 1>>));
    EXPECT_TRUE(
      bool(same_as<indexes_if<is_one, two, one, four, three, two, one, three>, index_sequence<1, 5>>));
}


TEST(TypeTraits, RmoeveUnsignedTest) {
    EXPECT_TRUE(bool(same_as<char, remove_unsigned_t<char>>));
    EXPECT_TRUE(bool(same_as<char, remove_unsigned_t<unsigned char>>));
    EXPECT_TRUE(bool(same_as<char volatile, remove_unsigned_t<volatile unsigned char>>));
    EXPECT_TRUE(bool(same_as<char const volatile, remove_unsigned_t<unsigned char const volatile>>));
    EXPECT_TRUE(bool(same_as<char const volatile&, remove_unsigned_t<unsigned char const volatile&>>));
}

// NOLINTEND(*-magic-numbers)
