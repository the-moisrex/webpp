// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
#define WEBPP_STD_PMR_ALLOCATOR_PACK_HPP

#include "allocator_pack.hpp"

#include <memory_resource>

namespace webpp {

    /**
     * Unfortunately due to the nature of the `new_delete_resource` and the `null_memory_resource`
     * (them being a function not a distinguishable type), we're not able to put them here, so we fall back
     * to `std::allocator`.
     *
     * fixme: see if you can add those two allocators in this as well by adding wrappers for them
     */
    struct std_pmr_allocator_pack {
        struct monotonic_buffer_resource_descriptor {
            static constexpr alloc::feature_pack features{alloc::noop_dealloc,
                                                          alloc::stateful,
                                                          alloc::unsync};

            template <typename T>
            using type = stl::pmr::monotonic_buffer_resource;
        };

        struct synchronized_pool_resource_descriptor {
            static constexpr alloc::feature_pack features{alloc::sync, alloc::stateful};

            template <typename T>
            using type = stl::pmr::synchronized_pool_resource;
        };

        struct unsynchronized_pool_resource_descriptor {
            static constexpr alloc::feature_pack features{};

            template <typename T>
            using type = stl::pmr::unsynchronized_pool_resource;
        };

        struct std_allocator_descriptor {
            static constexpr alloc::feature_pack features{alloc::stateless, alloc::sync, alloc::low_locality};

            template <typename T>
            using type = stl::allocator<T>;
        };

        // todo: add new_delete_resource
        using descriptor_list = alloc::allocator_list<monotonic_buffer_resource_descriptor,
                                                      synchronized_pool_resource_descriptor,
                                                      unsynchronized_pool_resource_descriptor,
                                                      std_allocator_descriptor>;
    };

    static_assert(AllocatorPack<std_pmr_allocator_pack>, "The specified allocator pack is not really one");

} // namespace webpp

#endif // WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
