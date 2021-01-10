// Created by moisrex on 1/27/20.

#include "../core/include/webpp/memory/allocator_pack.hpp"
#include "../core/include/webpp/memory/available_memory.hpp"
#include "../core/include/webpp/memory/std_allocator_pack.hpp"
#include "../core/include/webpp/memory/std_pmr_allocator_pack.hpp"
#include "./common_pch.hpp"


using namespace webpp;
using namespace webpp::alloc;
using namespace std;

TEST(MemoryTest, AllocatorPackTest) {
    static_assert(Allocator<stl::pmr::polymorphic_allocator<char>>);
    allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    auto str = make<string, monotonic_features>(alloc_pack, placeholder{});
    using str_t = remove_cvref_t<decltype(str)>;
    static_assert(stl::same_as<str_t, pmr::string>);
}


TEST(MemoryTest, AvailableMemory) {
    EXPECT_TRUE(available_memory() >= 0);
}
