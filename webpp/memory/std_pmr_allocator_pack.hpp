// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
#define WEBPP_STD_PMR_ALLOCATOR_PACK_HPP


// fallback
#include "../std/memory_resource.hpp"
#ifndef webpp_has_memory_resource
#    warning \
      "Using std_allocator_pack as a fallback for std_pmr_allocator_pack due to not having access to <memory_resource> header file."
#    include "std_allocator_pack.hpp"

namespace webpp::stl::pmr {
    using allocator_descriptors = stl::allocator_descriptors;
}
#else
#    include "allocator_pack.hpp"

namespace webpp {

    // add stuff to the stl::pmr namespace
    namespace stl::pmr {
        using namespace std::pmr;

        /**
         * Unfortunately due to the nature of the `new_delete_resource` and the `null_memory_resource`
         * (them being a function not a distinguishable type), we're not able to put them here, so we fall
         * back to `std::allocator`.
         *
         * fixme: see if you can add those two allocators in this as well by adding wrappers for them
         */
        namespace details {

            // std::pmr has one allocator type
            struct polymorphic_allocator_descriptor {
                // but std::pmr has multiple resource types for that one allocator

                template <typename T = byte>
                using allocator = polymorphic_allocator<T>;

                // the resources will inherit these features
                static constexpr alloc::feature_pack allocator_features{alloc::stateful};

                struct default_resource_descriptor {
                    using storage_type = void;
                    static constexpr alloc::feature_pack resource_features{};

                    // construct the allocator based on the resource
                    template <typename T>
                    static inline allocator<T> construct_allocator() noexcept {
                        return {};
                    }
                };

                struct monotonic_buffer_resource_descriptor {
                    using storage_type = monotonic_buffer_resource;
                    static constexpr alloc::feature_pack resource_features{alloc::noop_dealloc,
                                                                           alloc::unsync};

                    // construct the allocator based on the resource
                    template <typename T>
                    static inline allocator<T> construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }
                };

                struct synchronized_pool_resource_descriptor {
                    using storage_type = synchronized_pool_resource;
                    static constexpr alloc::feature_pack resource_features{alloc::sync};

                    // construct the allocator based on the resource
                    template <typename T>
                    static inline allocator<T> construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }
                };

                struct unsynchronized_pool_resource_descriptor {
                    using storage_type = unsynchronized_pool_resource;
                    static constexpr alloc::feature_pack resource_features{};

                    // construct the allocator based on the resource
                    template <typename T>
                    static inline allocator<T> construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }
                };

                //        struct std_allocator_descriptor {
                //            static constexpr alloc::feature_pack features{alloc::stateless, alloc::sync,
                //            alloc::low_locality};
                //            using storage_type = stl::allocator<T>;
                //        };

                // todo: add new_delete_resource
                using resources =
                  type_list<default_resource_descriptor,
                            monotonic_buffer_resource_descriptor,
                            synchronized_pool_resource_descriptor,
                            unsynchronized_pool_resource_descriptor>;

                using default_resource = default_resource_descriptor;
            };

        } // namespace details

        using allocator_descriptors = type_list<details::polymorphic_allocator_descriptor>;
    }     // namespace stl::pmr

} // namespace webpp

#endif
#endif // WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
