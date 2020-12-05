// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_ALLOCATOR_HPP
#define WEBPP_STD_ALLOCATOR_HPP

#include "allocator_pack.hpp"

#include <memory>

namespace webpp {

    struct std_allocator_pack {
        template <alloc::feature_pack FPack>
        struct ranker {
            template <typename T>
            using type = stl::allocator<T>;
        };

        struct std_allocator_descriptor {

            template <typename T>
            using type = stl::allocator<T>;

            static constexpr alloc::feature_pack features{alloc::requires_default_ctor, alloc::requires_sync};
        };

        using list = alloc::allocator_list<std_allocator_descriptor>;

        template <typename T>
        using local = stl::allocator<T>;

        template <typename T>
        using pool = stl::allocator<T>;
    };


    static_assert(AllocatorPack<std_allocator_pack>, "The specified allocator pack is not really one");

} // namespace webpp

#endif
