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

                struct monotonic_buffer_resource_descriptor {
                    using type = monotonic_buffer_resource;
                    static constexpr alloc::feature_pack features{alloc::noop_dealloc, alloc::unsync};

                    // construct the allocator based on the resource
                    static inline polymorphic_allocator<byte>
                    construct_allocator(monotonic_buffer_resource& res) noexcept {
                        return {&res};
                    }

                    static inline monotonic_buffer_resource construct_resource() noexcept {
                        return {};
                    }
                };

                struct synchronized_pool_resource_descriptor {
                    using type = stl::add_pointer_t<stl::pmr::synchronized_pool_resource>;
                    static constexpr alloc::feature_pack features{alloc::sync};
                };

                struct unsynchronized_pool_resource_descriptor {
                    using type = stl::add_pointer_t<stl::pmr::unsynchronized_pool_resource>;
                    static constexpr alloc::feature_pack features{};
                };

                //        struct std_allocator_descriptor {
                //            static constexpr alloc::feature_pack features{alloc::stateless, alloc::sync,
                //            alloc::low_locality};
                //            using type = stl::allocator<T>;
                //        };

                template <typename T = stl::byte>
                using type = stl::pmr::polymorphic_allocator<T>;

                // the resources will inherit these features
                static constexpr alloc::feature_pack features{alloc::stateful};

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
