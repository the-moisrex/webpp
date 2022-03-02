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

TEST(MemoryTest, PMRAllocatorPackTest) {
    static_assert(Allocator<stl::pmr::polymorphic_allocator<char>>);
    alloc::allocator_pack<stl::allocator_descriptors> alloc_pack;
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
    static_assert(same_as<vec_str_t, stl::pmr::string>, "Nested allocator-rebinding don't work");
    static_assert(same_as<vec_t, stl::pmr::vector<stl::pmr::string>>, "Allocator-rebinding don't work");
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


TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() > 0);
}
