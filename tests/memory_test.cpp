// Created by moisrex on 1/27/20.

#include "../webpp/std/memory.hpp"

#include "../webpp/memory/available_memory.hpp"
#include "../webpp/memory/object.hpp"
#include "../webpp/std/memory_resource.hpp"
#include "../webpp/std/string.hpp"
#include "../webpp/traits/enable_traits.hpp"
#include "../webpp/traits/std_pmr_traits.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "common/tests_common_pch.hpp"

#include <vector>

using namespace webpp;

#ifdef __cpp_lib_polymorphic_allocator

using Types = testing::Types<std_pmr_traits, std_traits>;

template <class T>
struct MemoryTest : testing::Test {};

TYPED_TEST_SUITE(MemoryTest, Types);

TYPED_TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() > 0);
}

TYPED_TEST(MemoryTest, Concepts) {
    class incomplete_class;
    using alloc_type = traits::allocator_type_of<TypeParam, int>;
    using dync_type  = istl::dynamic<int, alloc_type>;
    static_assert(stl::is_copy_assignable_v<dync_type>, "It should be copyable");
    static_assert(stl::is_copy_constructible_v<dync_type>, "It should be copyable");
    static_assert(stl::copyable<dync_type>, "It should be copyable");
    static_assert(stl::movable<dync_type>, "It should be movable");
    static_assert(istl::implicitly_default_constructible<dync_type>, "It should be constructible");
    static_assert(!istl::explicitly_default_constructible<dync_type>,
                  "It should be constructible only implicitly");

    using inc_alloc_type = traits::allocator_type_of<TypeParam, incomplete_class>;
    using inc_dync_type  = istl::dynamic<incomplete_class, inc_alloc_type>;
    static_assert(stl::is_copy_assignable_v<inc_dync_type>, "It should be copyable");
    static_assert(stl::is_copy_constructible_v<inc_dync_type>, "It should be copyable");
    static_assert(stl::copyable<inc_dync_type>, "It should be copyable");
    static_assert(stl::movable<inc_dync_type>, "It should be movable");
    static_assert(istl::implicitly_default_constructible<inc_dync_type>, "It should be constructible");
    static_assert(!istl::explicitly_default_constructible<inc_dync_type>,
                  "It should be constructible only implicitly");
}

// TYPED_TEST(MemoryTest, LocalAllocTest) {
//     using traits_type = TypeParam;
//     enable_owner_traits<traits_type> etraits;
//     static_assert(Allocator<stl::pmr::polymorphic_allocator<char>>);
//     auto str    = object::make_local_like<stl::string>(etraits, "hello world");
//     using str_t = stl::remove_cvref_t<decltype(str)>;
//     static_assert(stl::same_as<typename str_t::type, traits::local_string<traits_type>>);
//     EXPECT_EQ(str, "hello world");
//
//     auto str2 = object::make_local_like<stl::string>(etraits, "hello world");
//     ASSERT_EQ(str2, "hello world");
//
//     // nested allocator-rebinding: both string and vector should use the specified allocator
//     [[maybe_unused]] auto str_vec    = object::make_local_like<stl::vector<stl::string>>(etraits);
//     using vec_t     = typename stl::remove_cvref_t<decltype(str_vec)>::type;
//     using vec_str_t = typename vec_t::value_type;
//     static_assert(stl::same_as<vec_str_t, stl::pmr::string>, "Nested allocator-rebinding don't work");
//     static_assert(stl::same_as<vec_t, stl::pmr::vector<stl::pmr::string>>, "Allocator-rebinding don't
//     work");
// }

TYPED_TEST(MemoryTest, DynamicType) {
    using istl::dynamic;

    EXPECT_TRUE((stl::uses_allocator_v<dynamic<int, stl::allocator<stl::byte>>, stl::allocator<stl::byte>>) );

    // I disabled the default ctor.

    // EXPECT_TRUE((stl::is_default_constructible_v<dynamic<bool>>) );
    // EXPECT_TRUE((stl::is_default_constructible_v<dynamic<int>>) );
    EXPECT_TRUE((stl::is_move_assignable_v<dynamic<int>>) );
    EXPECT_TRUE((stl::is_assignable_v<dynamic<int>, int>) );
    EXPECT_TRUE((stl::is_copy_constructible_v<dynamic<int>>) );
    EXPECT_TRUE((stl::is_move_constructible_v<dynamic<int>>) );

    // EXPECT_TRUE((stl::is_default_constructible_v<istl::pmr::dynamic<bool>>) );
    // EXPECT_TRUE((stl::is_default_constructible_v<istl::pmr::dynamic<int>>) );
    EXPECT_TRUE((stl::is_move_assignable_v<istl::pmr::dynamic<int>>) );
    EXPECT_TRUE((stl::is_assignable_v<istl::pmr::dynamic<int>, int>) );
    EXPECT_TRUE((stl::is_copy_constructible_v<istl::pmr::dynamic<int>>) );
    EXPECT_TRUE((stl::is_move_constructible_v<istl::pmr::dynamic<int>>) );

    // EXPECT_TRUE((stl::is_default_constructible_v<dynamic<stl::string>>) );
    EXPECT_TRUE((stl::is_move_assignable_v<dynamic<stl::string>>) );
    EXPECT_TRUE((stl::is_assignable_v<dynamic<stl::string>, stl::string>) );
    EXPECT_TRUE((stl::is_copy_constructible_v<dynamic<stl::string>>) );
    EXPECT_TRUE((stl::is_move_constructible_v<dynamic<stl::string>>) );


    // EXPECT_TRUE((stl::is_default_constructible_v<dynamic<stl::pmr::string>>) );
    EXPECT_TRUE((stl::is_move_assignable_v<dynamic<stl::pmr::string>>) );
    EXPECT_TRUE((stl::is_assignable_v<dynamic<stl::pmr::string>, stl::pmr::string>) );
    EXPECT_TRUE((stl::is_copy_constructible_v<dynamic<stl::pmr::string>>) );
    EXPECT_TRUE((stl::is_move_constructible_v<dynamic<stl::pmr::string>>) );


    dynamic<int> d1{stl::allocator<int>()};
    d1 = 20;
    EXPECT_EQ(*d1, 20);
    d1 = 23;
    EXPECT_EQ(*d1, 23);
    *d1 = 25;
    EXPECT_EQ(*d1, 25);

    istl::pmr::dynamic<stl::pmr::string> d2{stl::allocator_arg, {stl::pmr::get_default_resource()}};
    // d2 = "hello world";
    // EXPECT_EQ(*d2, "hello world");
    *d2 = "hello world 2";
    EXPECT_EQ(*d2, "hello world 2");


    struct incomplete_type;

    struct complete_type {
        dynamic<incomplete_type> baby;
        int                      val = 23;
    };

    struct incomplete_type {
        int val = 23;
    };

    stl::allocator<incomplete_type> alloc;
    dynamic<incomplete_type>        normal{alloc};
    EXPECT_EQ(normal->val, 23);

    complete_type daddy{.baby = dynamic<incomplete_type>{alloc}};
    EXPECT_EQ(daddy.baby->val, 23);
    daddy.baby = incomplete_type{.val = 24}; // this constructs the object with the allocator in the type
    EXPECT_EQ(daddy.val, 23);
    EXPECT_EQ(daddy.baby->val, 24);
}

TYPED_TEST(MemoryTest, DynamicTypeBasicTest) {
    using istl::dynamic;
    dynamic one{1};
    dynamic two{2};
    EXPECT_EQ(*one, 1);
    EXPECT_EQ(*two, 2);

    static constexpr stl::string_view long_3s = "3333333333333333333333333333333333333333333333333333333333";
    stl::array<char, 1024>            data{};
    stl::pmr::monotonic_buffer_resource res{data.data(), data.size()};

    // use the pmr string's allocator (both type and it's pass it down)
    dynamic<stl::pmr::string> three{stl::allocator_arg, {&res}, long_3s};
    EXPECT_EQ(*three, long_3s);
    EXPECT_EQ((stl::pmr::string{data.data(), three->size()}), long_3s);
}

#endif

TYPED_TEST(MemoryTest, PolymorphicTestForDynamicType) {
    using webpp::istl::dynamic;

    struct mother {
        virtual stl::string to_string() = 0;
        virtual ~mother()               = default;
    };

    struct son : mother {
        int value  = 12;
        int value2 = 13;

        stl::string to_string() override {
            return "son";
        }
    };

    static int side_effect = 20;

    struct daughter : mother {
        int value = 20;

        daughter() {
            side_effect = value;
        }

        stl::string to_string() override {
            return "daughter";
        }

        ~daughter() override {
            side_effect += 10;
        }
    };

    using alloc_t = stl::allocator<stl::byte>;
    EXPECT_TRUE((stl::is_constructible_v<dynamic<mother>, dynamic<daughter>>) );
    EXPECT_TRUE((stl::is_constructible_v<dynamic<mother, alloc_t>, dynamic<mother, alloc_t>, alloc_t>) );
    EXPECT_TRUE((stl::is_constructible_v<dynamic<mother, alloc_t>, dynamic<daughter, alloc_t>, alloc_t>) );
    EXPECT_TRUE((stl::is_constructible_v<dynamic<mother, alloc_t>, daughter, alloc_t>) );

    dynamic<mother> family_member{istl::no_init};
    family_member.template emplace<son>(); // replace a son
    EXPECT_EQ(family_member->to_string(), "son");
    EXPECT_EQ(family_member.template as<son>().value2, 13);
    family_member = daughter{};            // replace a daughter, using move
    EXPECT_EQ(family_member->to_string(), "daughter");
    EXPECT_EQ(family_member.template as<daughter>().value, 20);

    // memory leak check
    int const side_effect_now = side_effect;
    family_member.reset();
    EXPECT_EQ(side_effect, side_effect_now + 10);


    dynamic<mother> girl{daughter{}};
    EXPECT_EQ(girl->to_string(), "daughter");
    EXPECT_EQ(girl.template as<daughter>().value, 20);

    dynamic<mother> boy{stl::allocator_arg, {}, son{}};
    EXPECT_EQ(boy->to_string(), "son");
    EXPECT_EQ(boy.template as<son>().value, 12);


    boy = girl;
    EXPECT_EQ(boy->to_string(), "daughter");
    EXPECT_EQ(boy.template as<daughter>().value, 20);
}
