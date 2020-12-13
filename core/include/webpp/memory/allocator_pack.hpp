// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_PACK_HPP
#define WEBPP_ALLOCATOR_PACK_HPP

#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../utils/flags.hpp"
#include "allocator_concepts.hpp"

namespace webpp::alloc {

    // todo: see if you need to add low&high "variation" and "density" or not
    // todo: add "singleton" and "limited_space" as a feature
    // todo: add "low_/high_fragmentation"
    // todo: add "prefer_same_size" (pool)
    // todo: add multi-pool
    // todo: add "local/arena" (add a "buffer" type or a "stack" type)

    // https://cdn2-ecros.pl/event/codedive/files/presentations/2018/code%20dive%202018%20-%20Andreas%20Weis%20-%20Taming%20dynamic%20memory%20-%20An%20introduction%20to%20custom%20allocators%20in%20C%2B%2B.pdf
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0089r0.pdf

    // every feature has to have an opposite feature so the child can overwrite the parent's decision.
    enum features : unsigned short {
        sync,
        unsync,

        noop_dealloc,
        no_noop_dealloc,

        stateful,
        stateless,

        high_contention,
        low_contention,

        high_utilization,
        low_utilization,

        high_locality,
        low_locality
    };

    // only for those that are not a required feature; only include one of conflicting features.
    // the feature that if it's present it's usually better should be present here.
    static constexpr stl::array<stl::pair<features, long long int>, 4> feature_rates{{
      {noop_dealloc, 50},
      {high_locality, 30},
      {high_contention, 20},
      {high_utilization, 10},
    }};

    static constexpr stl::array<stl::pair<features, features>, 6> conflicting_features{{
      {sync, unsync},
      {noop_dealloc, no_noop_dealloc},
      {stateful, stateless},
      {high_contention, low_contention},
      {high_utilization, low_utilization},
      {high_locality, low_locality},
    }};

    // whenever used, these features are considered required and if the allocator don't have it, its rank is
    // going to be -100
    static constexpr stl::array<features, 3> required_features{sync, stateful, stateless};

    using feature_pack = flags::manager<features>;

    /**
     * Get the opposite feature of the input feature; the one that overwrite it if it's specified in the child
     */
    static constexpr features opposite_feature(features fch) noexcept {
        for (auto conflicting_fch : conflicting_features) {
            if (conflicting_fch.first == fch)
                return conflicting_fch.second;
            if (conflicting_fch.second == fch)
                return conflicting_fch.first;
        }
        return fch; // there's something wrong, we should not reach this part
    }

    /**
     * Merge two feature packs. One of them is the parent, and the other one is the child.
     * The Child's feature will overwrite the parent's feature if there's a conflict.
     */
    static constexpr feature_pack merge_features(feature_pack parent, feature_pack child) noexcept {
        feature_pack pack;
        for (auto conflicting_pack : conflicting_features) {
            features parent_feature =
              parent.is_on(conflicting_pack.first) ? conflicting_pack.first : conflicting_pack.second;
            features opposite_parent_feature =
              parent_feature == conflicting_pack.first ? conflicting_pack.second : conflicting_pack.first;

            // fixing mistakes
            if (parent.is_on(conflicting_pack.first) && parent.is_on(conflicting_pack.second)) {
                // daddy is stupid, whatever the child says
                if (child.is_on(conflicting_pack.first)) {
                    pack.on(conflicting_pack.first);
                } else if (child.is_on(conflicting_pack.second)) {
                    pack.on(conflicting_pack.second);
                } else {
                    // do not add it when they are both making the same mistake
                }
                continue;
            } else if (child.is_on(conflicting_pack.first) && child.is_on(conflicting_pack.second)) {
                // the child is stupid, whatever the daddy says
                if (parent.is_on(conflicting_pack.first)) {
                    pack.on(conflicting_pack.first);
                } else if (parent.is_on(conflicting_pack.second)) {
                    pack.on(conflicting_pack.second);
                } else {
                    // do not add it when they are both making the same mistake
                }
                continue;
            }

            // if parent has this set of feature
            if (parent.is_on(conflicting_pack.first) || parent.is_on(conflicting_pack.second)) {

                if (child.is_on(opposite_parent_feature)) // overwrite what the parent says
                    pack.on(opposite_parent_feature);
                else // both are saying the same thing or the child has nothing to say about it, so inherit it
                     // from daddy
                    pack.on(parent_feature);

            } else { // daddy says nothing, so let's see what the kid says about it
                if (child.is_on(conflicting_pack.first)) {
                    pack.on(conflicting_pack.first);
                } else if (child.is_on(conflicting_pack.second)) {
                    pack.on(conflicting_pack.second);
                } else {
                    // the kid says nothing as well, so just forget it
                }
            }
        }
        return pack;
    }

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
                long long int res = 100; // initial value

                // Checking required features first:
                for (auto const fch : required_features) {
                    if (asked_features.is_on(fch)) {
                        if (alloc_features.is_off(fch) || alloc_features.is_on(opposite_feature(fch))) {
                            return res * -1;
                        }
                    }
                }


                for (auto const ft : feature_rates) {
                    const auto fch          = ft.first;
                    const auto opposite_fch = opposite_feature(fch);
                    const auto points       = ft.second;
                    if (asked_features.is_on(fch)) {
                        if (alloc_features.is_on(fch)) {
                            res += points; // both are on
                        } else if (alloc_features.is_on(opposite_fch)) {
                            res -= points; // asked for it to be on, but it has the opposite one
                        }
                    } else if (asked_features.is_on(opposite_fch)) {
                        if (alloc_features.is_on(fch)) {
                            res -= points; // both are on
                        } else if (alloc_features.is_on(opposite_fch)) {
                            res += points; // asked for it to be on, but it has the opposite one
                        }
                    } else {
                        // the user didn't asked for it, but half a point for those allocators that have this
                        // feature and take half the point for those that have the opposite of this feature
                        if (alloc_features.is_on(fch)) {
                            res += points / 2;
                        } else if (alloc_features.is_on(opposite_fch)) {
                            res -= points / 2;
                        }
                    }
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

        // the ranking should be used on each combination of "allocator" and its "inputs"; sorting allocators
        // only will not result in the best solution.

        using best_descriptor = typename istl::ranked_types<ranking_condition<AskedFeatures>::template ranker,
                                                            AllocatorDescriptor...>::best::type;

        // get the allocator type that gets passed to the STL containers and such
        template <typename T>
        using type = typename best_descriptor::template type<T>;

        // todo: get the best input
    };

    // requires an allocator pack (which includes "allocator descriptor list")
    template <typename AllocatorPack, feature_pack AskedFeatures>
    requires(requires { typename AllocatorPack::descriptors; }) struct ranker<AllocatorPack, AskedFeatures>
      : ranker<typename AllocatorPack::descriptors, AskedFeatures> {};


    // todo: add "allocator pack" merger mechanism that helps in merging two or more packs of allocators

} // namespace webpp::alloc

#endif // WEBPP_ALLOCATOR_PACK_HPP
