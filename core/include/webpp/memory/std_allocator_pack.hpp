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

        template <typename T>
        using local = stl::allocator<T>;

        template <typename T>
        using pool = stl::allocator<T>;
    };

}

#endif
