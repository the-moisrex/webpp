// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_ALLOCATOR_HPP
#define WEBPP_STD_ALLOCATOR_HPP

#include "allocator_pack.hpp"

#include <memory>

namespace webpp {

    struct std_allocator_pack {
        struct std_allocator_descriptor {

            template <typename T>
            using type = stl::allocator<T>;

            static constexpr alloc::feature_pack features{alloc::stateless, alloc::sync, alloc::low_locality};
        };

        using descriptor_list = alloc::allocator_list<std_allocator_descriptor>;
    };


    static_assert(AllocatorPack<std_allocator_pack>, "The specified allocator pack is not really one");

} // namespace webpp

#endif
