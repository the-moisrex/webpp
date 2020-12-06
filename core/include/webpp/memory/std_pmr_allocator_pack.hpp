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

    struct std_allocator_pack {
        struct monotonic_buffer_resource_descriptor {
            static constexpr alloc::feature_pack features{alloc::noop_dealloc};

            template <typename T>
            using type = stl::pmr::monotonic_buffer_resource;
        };

        struct synchronized_pool_resource_descriptor {
            static constexpr alloc::feature_pack features{alloc::requires_sync};

            template <typename T>
            using type = stl::pmr::requires_synchronized_pool_resource;
        };

        struct unsynchronized_pool_resource_descriptor {
            static constexpr alloc::feature_pack features{};

            template <typename T>
            using type = stl::pmr::unsynchronized_pool_resource;
        };

        struct std_allocator_descriptor {
            static constexpr alloc::feature_pack features{alloc::requires_default_ctor, alloc::requires_sync};

            template <typename T>
            using type = stl::allocator<T>;
        };

        using descriptor_list =
          alloc::allocator_list<monotonic_buffer_resource_descriptor, synchronized_pool_resource_descriptor,
                                unsynchronized_pool_resource_descriptor, std_allocator_descriptor>;

        // todo add new_delete_resource
        template <typename T>
        using list =
          alloc::allocator_list<stl::pmr::monotonic_buffer_resource, stl::pmr::synchronized_pool_resource,
                                stl::pmr::unsynchronized_pool_resource>;

        template <alloc::feature_pack FPack>
        struct ranker {

            template <typename T>
            struct cond {
                static constexpr alloc::feature_pack alloc_features = T::features;

                static constexpr long long int value = ([]() constexpr noexcept {
                    // only std::allocator in this pack supports this feature
                    if (FPack.is_on(alloc::requires_default_ctor) &&
                        alloc_features.is_off(alloc::requires_default_ctor))
                        return -1;

                    // unsync and monotonic cannot be used in multi-threaded environments
                    if (FPack.is_on(alloc::requires_sync) && alloc_features.is_off(alloc::requires_sync))
                        return -1;

                    // only monotonic supports noop de-alloc
                    long long int noop_dealloc =
                      10 * FPack.is_on(alloc::noop_dealloc) * alloc_features.is_on(alloc::noop_dealloc);

                    return noop_dealloc;
                })();
            };

            template <typename T>
            using type = typename istl::ranked_types<
              cond, monotonic_buffer_resource_descriptor, synchronized_pool_resource_descriptor,
              unsynchronized_pool_resource_descriptor, std_allocator_descriptor>::best::type::type<T>;
        };

        template <typename T>
        using local = stl::pmr::monotonic_buffer_resource;

        template <typename T>
        using general = stl::pmr::requires_synchronized_pool_resource;
    };

} // namespace webpp

#endif // WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
