// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_CONCEPTS_HPP
#define WEBPP_ALLOCATOR_CONCEPTS_HPP

#include "../std/std.hpp"

#include <memory>

namespace webpp {


    // Name requirement: https://en.cppreference.com/w/cpp/named_req/Allocator
    template <typename A>
    concept Allocator = requires {
        typename A::value_type;
        typename stl::allocator_traits<A>::pointer;
        typename stl::allocator_traits<A>::size_type;
        typename stl::allocator_traits<A>::const_pointer;
        typename stl::allocator_traits<A>::void_pointer;
        typename stl::allocator_traits<A>::const_void_pointer;
        requires requires(A                                                     a,
                          A                                                     a1,
                          typename stl::allocator_traits<A>::size_type          n,
                          typename stl::allocator_traits<A>::pointer            p,
                          typename stl::allocator_traits<A>::const_pointer      cp,
                          typename stl::allocator_traits<A>::void_pointer       vp,
                          typename stl::allocator_traits<A>::const_void_pointer cvp,
                          decltype(*p)                                          r) {
            {a.allocate(n)};
            {*p};
            {*cp};
            // {p->m};
            // {cp->m};
            {static_cast<typename stl::allocator_traits<A>::pointer>(vp)};
            {static_cast<typename stl::allocator_traits<A>::const_pointer>(cvp)};
            {stl::pointer_traits<typename stl::allocator_traits<A>::pointer>::pointer_to(r)};
            {a == a1};
            {a != a1};
            {A(a)};
            {a1 = a};
            // A a(b)};
            {A(stl::move(a))};
            {a1 = stl::move(a)};
            // {A a(stl::move(b))};
        };
    };

    template <typename T, typename Alloc>
    concept AllocatorOf = Allocator<Alloc>&& stl::same_as<typename Alloc::value_type, T>;

    // static_assert(Allocator<stl::allocator<int>>, "There's a problem with Allocator concept");

    /**
     * Types of allocator that the core library may need based on the inputs:
     *   1. limited space on stack (with the max limit known)
     *   2. unlimited long-standing on heap
     *   3. default-constructible, general allocators
     *   4. singleton allocators that are passed to us
     *   5. copyable allocators that are passed to us
     */
    template <typename I>
    concept MemoryResource = requires {
        typename I::type; // the input type
        {I::features};    // the input features of type alloc::feature_pack
    };

    template <typename D>
    concept AllocatorDescriptor = requires {
        typename D::template type<char>; // get the allocator itself
        typename D::resources;           // input types (a list of MemoryResource)
        {D::features};                   // parent features of type alloc::feature_pack
    };


    // todo: should we simplify AllocatorPack by making the AllocatorDescriptor the AllocatorPack itself?
    template <typename D>
    concept AllocatorPack = requires {
        typename D::descriptors; // a list of AllocatorDescriptor
    };


    // todo: types to add:
    //       1. make: make<string>(alloc)
    //       2. buffer: stack, arena, ram
    //       3. allocator_pack: a pack of allocator's objects
    //       4. resource_pack: a pack of resources for one type of allocator

    /**
     * Examples of usage:
     * @code
     *
     *   // global (not really)
     *   allocator_pack<std_pmr_allocator_pack> alloc_pack();
     *   alloc_pack.set_upstream(...);
     *
     *   void local_function() {
     *       stack_allocator<1024> arena(alloc_pack); // a resource
     *       for (auto long_str: long_strings) {
     *          resource<basic_string<...>> str(arena, long_str);
     *          str += "; not that long";
     *       }
     *   }
     * @endcode
     */



} // namespace webpp

#endif // WEBPP_ALLOCATOR_CONCEPTS_HPP
