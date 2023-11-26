// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_ALLOCATOR_HPP
#define WEBPP_STD_ALLOCATOR_HPP

#include "allocator_pack.hpp"

#include <memory>

namespace webpp {
    namespace stl {
        namespace details {
            struct std_allocator_descriptor_type {
                struct default_resource_descriptor {
                    using storage_type = void;
                    static constexpr alloc::feature_pack resource_features{};

                    // construct the allocator based on the resource
                    template <typename T>
                    static inline stl::allocator<T> construct_allocator() noexcept {
                        return {};
                    }
                };

                using resources        = type_list<default_resource_descriptor>;
                using default_resource = default_resource_descriptor;

                static constexpr alloc::feature_pack allocator_features{
                  alloc::stateless,
                  alloc::sync,
                  alloc::low_locality};
                template <typename T>
                using allocator = stl::allocator<T>;
            };
        } // namespace details

        using allocator_descriptors = type_list<details::std_allocator_descriptor_type>;
    }     // namespace stl

} // namespace webpp

#endif
