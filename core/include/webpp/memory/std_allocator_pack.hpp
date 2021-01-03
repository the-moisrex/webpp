// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_ALLOCATOR_HPP
#define WEBPP_STD_ALLOCATOR_HPP

#include "allocator_pack.hpp"

#include <memory>

namespace webpp {
    namespace stl {
        namespace details {
            struct std_allocator_descriptor_type {
                using resources = type_list<>;
                static constexpr alloc::feature_pack allocator_features{alloc::stateless,
                                                                        alloc::sync,
                                                                        alloc::low_locality};
                template <typename T>
                using allocator = stl::allocator<T>;
            };
        } // namespace details

        using allocator_descriptors = type_list<details::std_allocator_descriptor_type>;
    } // namespace stl

} // namespace webpp

#endif
