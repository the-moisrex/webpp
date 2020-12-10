// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_PACK_HPP
#define WEBPP_ALLOCATOR_PACK_HPP

#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../utils/flags.hpp"
#include "allocator_concepts.hpp"

namespace webpp::alloc {

    // todo: see if you need to add low&high "variation" and "density" or not
    enum features : unsigned short {
        sync,
        unsync,
        noop_dealloc,
        stateful,
        stateless,
        high_contention,
        low_contention,
        high_utilization,
        low_utilization,
        high_locality,
        low_locality
    };

    using feature_pack = flags::manager<features>;

    template <typename... AllocatorDescriptors>
    using allocator_list = stl::tuple<AllocatorDescriptors...>;

    // common allocator features
    // todo: complete this list
    static constexpr auto monotonic        = feature_pack{stateful, noop_dealloc, unsync};
    static constexpr auto sync_pool        = feature_pack{sync, stateful};
    static constexpr auto unsync_pool      = feature_pack{stateful, unsync};
    static constexpr auto general_features = feature_pack{stateless, sync};



    /**
     * This type ranks each allocator descriptor
     */
    template <feature_pack Features>
    struct ranking_condition {

        template <typename AllocDescriptor>
        struct ranker {
            static constexpr feature_pack alloc_features = AllocDescriptor::features;
            static constexpr feature_pack asked_features = Features;

            static constexpr auto value = ([]() constexpr noexcept->long long int {
                // Checking required features first:
                if ((asked_features.is_on(sync) && alloc_features.is_off(sync)) ||
                    (asked_features.is_on(sync) && alloc_features.is_on(unsync)) ||
                    (asked_features.is_on(noop_dealloc) && alloc_features.is_off(noop_dealloc)) ||
                    (asked_features.is_on(stateless) && alloc_features.is_off(stateless)) ||
                    (asked_features.is_on(stateless) && alloc_features.is_on(stateful))) {
                    return -1;
                }

                // Checking mistakes for asked features:
                if ((asked_features.is_on(high_contention) && asked_features.is_on(low_contention)) ||
                    (asked_features.is_on(high_locality) && asked_features.is_on(low_locality)) ||
                    (asked_features.is_on(high_utilization) && asked_features.is_on(low_utilization)) ||
                    (asked_features.is_on(unsync) &&
                     (asked_features.is_on(high_contention) || asked_features.is_on(low_contention)))) {
                    return -1;
                }

                // Checking mistakes for this allocator features
                if ((alloc_features.is_on(high_contention) && alloc_features.is_on(low_contention)) ||
                    (alloc_features.is_on(high_locality) && alloc_features.is_on(low_locality)) ||
                    (alloc_features.is_on(high_utilization) && alloc_features.is_on(low_utilization)) ||
                    (alloc_features.is_on(unsync) &&
                     (alloc_features.is_on(high_contention) || alloc_features.is_on(low_contention)))) {
                    return -1;
                }

                long long int res = 100; // initial value

                if (asked_features.is_on(noop_dealloc)) {
                    if (alloc_features.is_on(noop_dealloc))
                        res += 30;
                }

                if (asked_features.is_on(high_locality)) {
                    if (alloc_features.is_on(high_locality))
                        res += 20;
                    else if (alloc_features.is_on(low_locality))
                        res -= 20;
                }

                if (asked_features.is_on(high_contention)) {
                    if (alloc_features.is_on(high_contention))
                        res += 10;
                    else if (alloc_features.is_on(low_contention))
                        res -= 10;
                }

                if (asked_features.is_on(high_utilization)) {
                    if (alloc_features.is_on(high_utilization))
                        res += 10;
                    else if (alloc_features.is_on(low_utilization))
                        res -= 10;
                }

                return res;
            })();
        };
    };

    /**
     * This type ranks the allocators, but you need to pass it the "allocator descriptors" which includes
     * the allocator features.
     */
    template <typename DescriptorList, feature_pack AskedFeatures>
    struct ranker;

    // requires an "allocator descriptor list"
    template <typename... AllocatorDescriptor, feature_pack AskedFeatures>
    struct ranker<allocator_list<AllocatorDescriptor...>, AskedFeatures> {

        using best_descriptor = typename istl::ranked_types<ranking_condition<AskedFeatures>::template ranker,
                                                            AllocatorDescriptor...>::best::type;

        template <typename T>
        using type = typename best_descriptor::template type<T>;
    };

    // requires an allocator pack (which includes "allocator descriptor list")
    template <typename AllocatorPack, feature_pack AskedFeatures>
    requires(requires {
        typename AllocatorPack::descriptor_list;
    }) struct ranker<AllocatorPack, AskedFeatures>
      : ranker<typename AllocatorPack::descriptor_list, AskedFeatures> {};


    // todo: add "allocator pack" merger mechanism that helps in merging two or more packs of allocators

} // namespace webpp::alloc

#endif // WEBPP_ALLOCATOR_PACK_HPP
