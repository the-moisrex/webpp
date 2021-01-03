// Created by moisrex on 12/3/20.

#ifndef WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
#define WEBPP_STD_PMR_ALLOCATOR_PACK_HPP

#include "allocator_pack.hpp"

#include <memory_resource>

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

                // for internal usage; todo: should we use allocator_type instead of "type"?
                using allocator_type = allocator<>;

                // the resources will inherit these features
                static constexpr alloc::feature_pack allocator_features{alloc::stateful};

                struct monotonic_buffer_resource_descriptor {
                    using storage_type = monotonic_buffer_resource;
                    static constexpr alloc::feature_pack resource_features{alloc::noop_dealloc, alloc::unsync};

                    // construct the allocator based on the resource
                    static inline allocator_type construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }

                    static inline storage_type construct_resource() noexcept {
                        return {};
                    }
                };

                struct synchronized_pool_resource_descriptor {
                    using storage_type = synchronized_pool_resource;
                    static constexpr alloc::feature_pack resource_features{alloc::sync};

                    // construct the allocator based on the resource
                    static inline allocator_type construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }
                };

                struct unsynchronized_pool_resource_descriptor {
                    using storage_type = unsynchronized_pool_resource;
                    static constexpr alloc::feature_pack resource_features{};

                    // construct the allocator based on the resource
                    static inline allocator_type construct_allocator(storage_type& res) noexcept {
                        return {&res};
                    }
                };

                //        struct std_allocator_descriptor {
                //            static constexpr alloc::feature_pack features{alloc::stateless, alloc::sync,
                //            alloc::low_locality};
                //            using storage_type = stl::allocator<T>;
                //        };

                // todo: add new_delete_resource
                using resources = type_list<monotonic_buffer_resource_descriptor,
                                            synchronized_pool_resource_descriptor,
                                            unsynchronized_pool_resource_descriptor>;
            };

        } // namespace details

        using allocator_descriptors = type_list<details::polymorphic_allocator_descriptor>;
    } // namespace stl::pmr

} // namespace webpp

#endif // WEBPP_STD_PMR_ALLOCATOR_PACK_HPP
