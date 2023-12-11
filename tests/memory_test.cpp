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

    istl::pmr::dynamic<stl::pmr::string> d2{stl::pmr::get_default_resource()};
    d2 = "hello world";
    EXPECT_EQ(*d2, "hello world");


    struct incomplete_type;

    struct complete_type {
        dynamic<incomplete_type> baby;
        int                      val = 23;
    };

    struct incomplete_type {
        int val = 23;
    };

    stl::allocator<incomplete_type> alloc;
    dynamic<incomplete_type>        normal{alloc, istl::initialize};
    EXPECT_EQ(normal->val, 23);

    complete_type daddy{
      .baby = dynamic<incomplete_type>{alloc, istl::initialize}
    };
    EXPECT_EQ(daddy.baby->val, 23);
    daddy.baby = incomplete_type{.val = 24}; // this constructs the object with the allocator in the type
    EXPECT_EQ(daddy.val, 23);
    EXPECT_EQ(daddy.baby->val, 24);
}
#endif

TYPED_TEST(MemoryTest, PolymorphicTestForDynamicType) {
    using webpp::istl::dynamic;

    struct mother {
        virtual stl::string to_string() = 0;
        virtual ~mother()               = default;
    };

    struct son : mother {
        stl::string to_string() override {
            return "son";
        }
    };

    static int side_effect = 20;

    struct daughter : mother {
        daughter() {
            side_effect = 20;
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

    dynamic<mother> family_member;
    family_member.template emplace<son>(); // replace a son
    EXPECT_EQ(family_member->to_string(), "son");
    family_member = daughter{};            // replace a daughter, using move
    EXPECT_EQ(family_member->to_string(), "daughter");

    // memory leak check
    int const side_effect_now = side_effect;
    family_member.reset();
    EXPECT_EQ(side_effect, side_effect_now + 10);
}
