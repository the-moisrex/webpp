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
        template <alloc::feature_pack FPack>
        struct ranker {

            template <typename T>
            struct cond {
                using value_type = stl::allocator_traits<T>::value_type;

                static constexpr long long int require_def_ctor = FPack.is_on(alloc::default_ctor);
                static constexpr long long int require_sync     = FPack.is_on(alloc::sync);
                static constexpr long long int is_monotonic =
                  stl::same_as<T, stl::pmr::monotonic_buffer_resource>;
                static constexpr long long int is_sync =
                  stl::same_as<T, stl::pmr::synchronized_pool_resource>;
                static constexpr long long int is_unsync =
                  stl::same_as<T, stl::pmr::unsynchronized_pool_resource>;
                static constexpr long long int is_std_alloc = stl::same_as<T, stl::allocator<value_type>>;

                static constexpr long long int value = ([]() constexpr noexcept {
                    // only std::allocator in this pack supports this feature
                    if (require_def_ctor && !is_std_alloc) {
                        return -1;
                    }

                    // unsync and monotonic cannot be used in multi-threaded environments
                    if (require_sync && (is_monotonic || is_unsync))
                        return -1;

                    // only monotonic supports noop de-alloc
                    long long int noop_dealloc = 10 * FPack.is_on(alloc::noop_dealloc) * is_monotonic;

                    return noop_dealloc;
                })();
            };

            template <typename T>
            using type = typename istl::ranked_types<
              cond, stl::pmr::monotonic_buffer_resource, stl::pmr::synchronized_pool_resource,
              stl::pmr::unsynchronized_pool_resource, stl::allocator<T>>::best::type;
        };

        struct monotonic_buffer_resource_descriptor {

            template <typename T>
            using type = stl::pmr::monotonic_buffer_resource;

            static constexpr alloc::feature_pack features{alloc::noop_dealloc};
        };

        struct synchronized_pool_resource_descriptor {

            template <typename T>
            using type = stl::pmr::synchronized_pool_resource;

            static constexpr alloc::feature_pack features{alloc::sync};
        };

        struct unsynchronized_pool_resource_descriptor {

            template <typename T>
            using type = stl::pmr::unsynchronized_pool_resource;

            static constexpr alloc::feature_pack features{};
        };

        struct std_allocator_descriptor {

            template <typename T>
            using type = stl::allocator<T>;

            static constexpr alloc::feature_pack features{alloc::default_ctor, alloc::sync};
        };

        using list = alloc::allocator_list<monotonic_buffer_resource_descriptor,
                                    synchronized_pool_resource_descriptor,
                                    unsynchronized_pool_resource_descriptor,
                                    std_allocator_descriptor>;

        template <typename T>
        using local = stl::pmr::monotonic_buffer_resource;

        template <typename T>
        using pool = stl::pmr::synchronized_pool_resource;
    };

} // namespace webpp

#endif // WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
