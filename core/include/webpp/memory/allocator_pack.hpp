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
    // todo: add constructor unifier for allocators
    // todo: add "shared memory" (boost::interprocess)

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

    // common allocator features
    // todo: complete this list
    static constexpr auto monotonic_features   = feature_pack{stateful, noop_dealloc, unsync};
    static constexpr auto sync_pool_features   = feature_pack{sync, stateful};
    static constexpr auto unsync_pool_features = feature_pack{stateful, unsync};
    static constexpr auto general_features     = feature_pack{stateless, sync};



    /**
     * This type ranks each allocator descriptor
     */
    template <feature_pack Features>
    struct ranking_condition {

        template <typename DescriptorPair>
        struct ranker;

        template <typename AllocDescriptor, typename ResDescriptor>
        struct ranker<stl::pair<AllocDescriptor, ResDescriptor>> {
            static constexpr feature_pack asked_features = Features;
            static constexpr feature_pack alloc_features = AllocDescriptor::features;
            static constexpr feature_pack res_features =
              merge_features(alloc_features, ResDescriptor::features);

            static constexpr auto value = ([]() constexpr noexcept->long long int {
                long long int res = 100; // initial value

                // Checking required features first:
                for (auto const fch : required_features) {
                    if (asked_features.is_on(fch)) {
                        if (res_features.is_off(fch) || res_features.is_on(opposite_feature(fch))) {
                            return res > 0 ? res * -1 : res; // abs(res) * -1
                        }
                    }
                }

                for (auto const ft : feature_rates) {
                    const auto fch          = ft.first;
                    const auto opposite_fch = opposite_feature(fch);
                    const auto points       = ft.second;
                    if (asked_features.is_on(fch)) {
                        if (res_features.is_on(fch)) {
                            res += points; // both are on
                        } else if (res_features.is_on(opposite_fch)) {
                            res -= points; // asked for it to be on, but it has the opposite one
                        }
                    } else if (asked_features.is_on(opposite_fch)) {
                        if (res_features.is_on(fch)) {
                            res -= points; // both are on
                        } else if (res_features.is_on(opposite_fch)) {
                            res += points; // asked for it to be on, but it has the opposite one
                        }
                    } else {
                        // the user didn't asked for it, but half a point for those allocators that have this
                        // feature and take half the point for those that have the opposite of this feature
                        if (res_features.is_on(fch)) {
                            res += points / 2;
                        } else if (res_features.is_on(opposite_fch)) {
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
    struct ranker : public ranker<typename alloc_res_pair_maker<DescriptorList>::type, AskedFeatures> {};

    template <istl::Pair... AllocResPairType, feature_pack AskedFeatures>
    struct ranker<type_list<AllocResPairType...>, AskedFeatures> {

        // the ranking should be used on each combination of "allocator" and its "resources";
        // sorting allocators only will not result in the best solution.

        using ranked =
          typename istl::ranked_types<ranking_condition<AskedFeatures>::template ranker, AllocResPairType...>;

        using best_descriptors_pair     = typename ranked::best::type;
        using best_allocator_descriptor = typename best_descriptors_pair::first_type;
        using best_resource_descriptor  = typename best_descriptors_pair::second_type;
    };



    // todo: add "allocator pack" merger mechanism that helps in merging two or more packs of allocators

    namespace details {

        template <feature_pack FPack>
        struct features_filterer {

            template <typename T>
            struct type {
                // check if T has the features in the FPack
                // if the rank > 0
                static constexpr bool value = ranking_condition<FPack>::template ranker<T>::value > 0;
            };
        };
    } // namespace details

    /**
     * Filter allocator "List" based on the FPack.
     */
    template <typename List, feature_pack FPack>
    using filter = istl::filter_parameters<details::features_filterer<FPack>::template type, List>;

    template <typename List>
    using filter_stateless_allocators = istl::filter_parameters<stl::is_default_constructible, List>;

    template <typename List>
    using filter_stateful_allocators =
      istl::filter_parameters<istl::templated_negation<stl::is_default_constructible>::template type, List>;


    struct placeholder {};

    /**
     * The allocator pack type; this will hold a pack of allocators and their resources (if any)
     */
    template <AllocatorDescriptorList AllocDescriptorsType>
    struct allocator_pack {

        // the type is: tuple<AllocatorDescriptor, ...>
        using allocator_descriptors = AllocDescriptorsType;

        // the type is: tuple<ResourceDescriptor, ...>
        using resource_descriptors = typename resource_descriptor_extractor<allocator_descriptors>::type;

        // the type is: tuple<stl::pair<AllocatorDescriptor, ResourceDescriptor>, ...>
        using alloc_res_pairs = typename alloc_res_pair_maker<allocator_descriptors>::type;

        // a tuple of allocators
        template <typename T>
        using allocators_type = typename allocator_extractor<allocator_descriptors>::template type<T>;

        // a tuple of resources (not their descriptors)
        // todo: make these unique (should we?)
        using resources_type = typename resource_extractor<allocator_descriptors>::type;

        template <feature_pack FPack>
        using ranked = ranker<allocator_descriptors, FPack>;

        template <feature_pack FPack>
        using best_allocator_descriptor = typename ranked<FPack>::best_allocator_descriptor;

        template <feature_pack FPack, typename T>
        using best_allocator = typename best_allocator_descriptor<FPack>::template type<T>;

        template <AllocatorDescriptor AllocDescType>
        static constexpr bool has_allocator_descriptor =
          istl::tuple_contains<allocator_descriptors, AllocDescType>::value;

        template <template <typename> typename AllocType>
        static constexpr bool has_templated_allocator =
          istl::tuple_contains<allocators_type<char>, AllocType<char>>::value;

        template <Allocator AllocType>
        static constexpr bool has_allocator =
          istl::tuple_contains<allocators_type<typename AllocType::value_type>, AllocType>::value;

      private:
        [[no_unique_address]] resources_type resources{};

      public:
        allocator_pack(allocator_pack&&) noexcept = default;
        allocator_pack(allocator_pack const&)     = delete; // some resources are not copy-able; so ...

        allocator_pack(resources_type const& res) noexcept : resources{res} {};
        allocator_pack(resources_type&& res) noexcept : resources{stl::move(res)} {};

        template <typename... ResourceType>
        allocator_pack(ResourceType&&... res) noexcept
          : resources{istl::make_tuple_no_order<resources_type, ResourceType...>(
              stl::forward<ResourceType>(res)...)} {}


        template <Allocator AllocType>
        requires(has_allocator<AllocType>) [[nodiscard]] auto get() const noexcept {
            return stl::get<AllocType>(resources);
        }


        template <feature_pack FPack, typename T>
        [[nodiscard]] auto get() const noexcept {
            using the_alloc_type = best_allocator<FPack, T>;
        }

        template <typename T>
        [[nodiscard]] auto local_allocator() const noexcept {
            return get<monotonic_features, T>();
        }

        template <typename T>
        [[nodiscard]] auto general_allocator() const noexcept {
            return get<general_features, T>();
        }


        template <typename T, template <typename> typename AllocType, typename... Args>
        requires(has_templated_allocator<AllocType>) constexpr auto make(Args&&... args) {
            if constexpr (!requires { typename T::allocator_type; }) {
                // doesn't have an allocator, so construct a normal object
                return T{stl::forward<Args>(args)...};
            } else {
                using old_allocator_type = typename T::allocator_type;
                using value_type         = typename old_allocator_type::value_type;
                using selected_allocator = AllocType<value_type>;
                using new_type           = istl::replace_parameter<T, old_allocator_type, selected_allocator>;
                auto const& the_alloc    = get<selected_allocator>();
                if constexpr (istl::tuple_contains<new_type, placeholder>::value) {
                    return new_type{
                      istl::replace_object<placeholder, best_allocator>(stl::forward<Args>(args),
                                                                        the_alloc)...};
                } else if constexpr (requires {
                                         new_type{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
                                     }) {
                    // as the first and second argument
                    return new_type{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
                } else if constexpr (requires { new_type{stl::forward<Args>(args)..., the_alloc}; }) {
                    // as the last argument
                    return new_type{stl::forward<Args>(args)..., the_alloc};
                } else {
                    static_assert(false && sizeof(new_type),
                                  "We don't know how to pass the allocator to the specified type.");
                    return new_type{stl::forward<Args>(args)...};
                }
            }
        }

        template <typename T, Allocator AllocType, typename... Args>
        constexpr auto make(Args&&... args) {
            return make<T, stl::allocator_traits<AllocType>::template rebind, Args...>(
              stl::forward<Args>(args)...);
        }

        template <typename T, feature_pack FPack, typename... Args>
        constexpr auto make(Args&&... args) {
            if constexpr (FPack.empty()) {
                using old_allocator_type = typename T::allocator_type;
                if constexpr (allocator_pack::template has_allocator<old_allocator_type>) {
                    return make<T, old_allocator_type, Args...>(stl::forward<Args>(args)...);
                } else {
                    static_assert(false && sizeof(allocator_pack),
                                  "We don't have an allocator for this type, and you didn't specify "
                                  "the features you'd like your allocator to have so we don't know "
                                  "which allocator to choose.");
                }
            } else {
                using best_choice = typename allocator_pack::template best_allocator_descriptor<FPack>;
                return make<T, best_choice::template type, Args...>(stl::forward<Args>(args)...);
            }
        }


        template <typename T, typename... Args>
        constexpr auto local(Args&&... args) {
            return make<T, monotonic_features, Args...>(stl::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        constexpr auto general(Args&&... args) {
            return make<T, general_features, Args...>(stl::forward<Args>(args)...);
        }
    };



    template <typename T, feature_pack FPack, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make(allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return alloc_pack.template make<T, FPack, Args...>(stl::forward<Args>(args)...);
    }


    // didn't use " = feature_pack{}" as default template parameter because of lack of compiler support at the
    // time of writing this
    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make(allocator_pack<AllocDescType>& alloc_pack, Args&&... args) noexcept {
        return make<T, feature_pack{}, Args...>(alloc_pack, stl::forward<Args>(args)...);
    }




} // namespace webpp::alloc

#endif // WEBPP_ALLOCATOR_PACK_HPP
