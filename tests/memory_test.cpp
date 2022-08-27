// Created by moisrex on 1/27/20.

#include "../core/include/webpp/memory/allocator_pack.hpp"
#include "../core/include/webpp/memory/available_memory.hpp"
#include "../core/include/webpp/memory/object.hpp"
#include "../core/include/webpp/memory/std_allocator_pack.hpp"
#include "../core/include/webpp/memory/std_pmr_allocator_pack.hpp"
#include "../core/include/webpp/std/memory_resource.hpp"
#include "../core/include/webpp/std/string.hpp"
#include "common_pch.hpp"

#include <vector>

using namespace webpp;

#ifdef webpp_has_memory_resource

TEST(MemoryTest, PMRAllocatorPackTest) {
    static_assert(Allocator<stl::pmr::polymorphic_allocator<char>>);
    alloc::allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    auto str    = alloc::make<stl::string, alloc::local_features>(alloc_pack, "hello world");
    using str_t = stl::remove_cvref_t<decltype(str)>;
    static_assert(stl::same_as<str_t, stl::pmr::string>);
    EXPECT_EQ(str, "hello world");

    auto str2 = object::make_local<stl::string>(alloc_pack, "hello world");
    ASSERT_EQ(str2, "hello world");

    // nested allocator-rebinding: both string and vector should use the specified allocator
    auto str_vec    = object::make_local<stl::vector<stl::string>>(alloc_pack);
    using vec_t     = typename stl::remove_cvref_t<decltype(str_vec)>::object_type;
    using vec_str_t = typename vec_t::value_type;
    static_assert(stl::same_as<vec_str_t, stl::pmr::string>, "Nested allocator-rebinding don't work");
    static_assert(stl::same_as<vec_t, stl::pmr::vector<stl::pmr::string>>, "Allocator-rebinding don't work");
}

TEST(MemoryTest, STDAllocatorPackTest) {
    alloc::allocator_pack<stl::allocator_descriptors> alloc_pack;
    auto str    = alloc::make<stl::pmr::string, alloc::local_features>(alloc_pack, "hello world");
    using str_t = stl::remove_cvref_t<decltype(str)>;
    static_assert(stl::same_as<str_t, stl::string>);
    EXPECT_EQ(str, "hello world");

    auto str2 = object::make_local<stl::pmr::string>(alloc_pack, "hello world");
    ASSERT_EQ(str2, "hello world");

    // nested allocator-rebinding: both string and vector should use the specified allocator
    auto str_vec    = object::make_local<stl::pmr::vector<stl::pmr::string>>(alloc_pack);
    using vec_t     = typename stl::remove_cvref_t<decltype(str_vec)>::object_type;
    using vec_str_t = typename vec_t::value_type;
    static_assert(stl::same_as<vec_str_t, stl::string>, "Nested allocator-rebinding don't work");
    static_assert(stl::same_as<vec_t, stl::vector<stl::string>>, "Allocator-rebinding don't work");
}

#endif

TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() > 0);
}

TEST(MemoryTest, DynamicType) {
    using webpp::istl::dynamic;

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

    complete_type daddy{.baby = stl::allocator<incomplete_type>()};
    EXPECT_EQ(daddy.baby->val, 23);
    daddy.baby = incomplete_type{.val = 24}; // this constructs the object with the allocator in the type
    EXPECT_EQ(daddy.val, 23);
    EXPECT_EQ(daddy.baby->val, 24);
}

TEST(MemoryTest, PolymorphicTestForDynamicType) {
    using webpp::istl::dynamic;

    struct mother {
        virtual stl::string to_string() = 0;
    };

    struct son : mother {
        stl::string to_string() override {
            return "son";
        }
    };
    struct daughter : mother {
        stl::string to_string() override {
            return "daughter";
        }
    };

    dynamic<mother> family_member{stl::allocator<mother>()};
    family_member.template emplace<son>(); // replace a son
    EXPECT_EQ(family_member->to_string(), "son");
    family_member = daughter{}; // replace a daughter, using move
    EXPECT_EQ(family_member->to_string(), "daughter");
}
