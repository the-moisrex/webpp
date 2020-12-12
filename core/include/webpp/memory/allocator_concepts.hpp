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

    template <typename I>
    concept AllocatorInput = requires {
        typename I::type; // the input type
        {I::features};    // the input features of type alloc::feature_pack
    };

    template <typename D>
    concept AllocatorDescriptor = requires {
        typename D::template type<char>; // get the allocator itself
        typename D::inputs;              // input types (a list of AllocatorInput)
        {D::features};                   // parent features of type alloc::feature_pack
    };


    // todo: should we simplify AllocatorPack by making the AllocatorDescriptor the AllocatorPack itself?
    template <typename D>
    concept AllocatorPack = requires {
        typename D::descriptors; // a list of AllocatorDescriptor
    };

} // namespace webpp

#endif // WEBPP_ALLOCATOR_CONCEPTS_HPP
