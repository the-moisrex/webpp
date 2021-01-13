// Created by moisrex on 1/27/20.

#include "../core/include/webpp/memory/allocator_pack.hpp"
#include "../core/include/webpp/memory/available_memory.hpp"
#include "../core/include/webpp/memory/object.hpp"
#include "../core/include/webpp/memory/std_allocator_pack.hpp"
#include "../core/include/webpp/memory/std_pmr_allocator_pack.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace webpp::alloc;
using namespace webpp::stl;

TEST(MemoryTest, AllocatorPackTest) {
    static_assert(Allocator<stl::pmr::polymorphic_allocator<char>>);
    allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    auto str    = make<string, monotonic_features>(alloc_pack, "hello world", placeholder{});
    using str_t = remove_cvref_t<decltype(str)>;
    static_assert(same_as<str_t, stl::pmr::string>);
    EXPECT_EQ(str, "hello world");

    object::local<string> str2(alloc_pack, "hello world");
    ASSERT_EQ(str2, "hello world");

    // nested allocator-rebinding: both string and vector should use the specified allocator
    object::local<vector<string>> str_vec{alloc_pack};
    using vec_t     = typename remove_cvref_t<decltype(str_vec)>::object_type;
    using vec_str_t = typename vec_t::value_type;
    static_assert(same_as<vec_str_t, stl::pmr::string>, "Nested allocator-rebinding don't work");
    static_assert(same_as<vec_t, stl::pmr::vector<stl::pmr::string>>, "Allocator-rebinding don't work");
}


TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
