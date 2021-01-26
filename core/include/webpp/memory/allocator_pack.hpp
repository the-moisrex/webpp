// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_PACK_HPP
#define WEBPP_ALLOCATOR_PACK_HPP

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
    static constexpr auto local_features       = monotonic_features;
    static constexpr auto general_features     = feature_pack{stateless, sync};



    /**
     * This type ranks each allocator descriptor
     */
    template <feature_pack Features>
    struct ranking_condition {


        static constexpr long long int value_generator(feature_pack TheFeatures,
                                                       feature_pack AskedFeatures) noexcept {
            long long int res = 100; // initial value

            // Checking required features first:
            for (auto const fch : required_features) {
                if (AskedFeatures.is_on(fch)) {
                    if (TheFeatures.is_off(fch) || TheFeatures.is_on(opposite_feature(fch))) {
                        return res > 0 ? res * -1 : res; // abs(res) * -1
                    }
                }
            }

            for (auto const ft : feature_rates) {
                const auto fch          = ft.first;
                const auto opposite_fch = opposite_feature(fch);
                const auto points       = ft.second;
                if (AskedFeatures.is_on(fch)) {
                    if (TheFeatures.is_on(fch)) {
                        res += points; // both are on
                    } else if (TheFeatures.is_on(opposite_fch)) {
                        res -= points; // asked for it to be on, but it has the opposite one
                    }
                } else if (AskedFeatures.is_on(opposite_fch)) {
                    if (TheFeatures.is_on(fch)) {
                        res -= points; // both are on
                    } else if (TheFeatures.is_on(opposite_fch)) {
                        res += points; // asked for it to be on, but it has the opposite one
                    }
                } else {
                    // the user didn't asked for it, but half a point for those allocators that have this
                    // feature and take half the point for those that have the opposite of this feature
                    if (TheFeatures.is_on(fch)) {
                        res += points / 2;
                    } else if (TheFeatures.is_on(opposite_fch)) {
                        res -= points / 2;
                    }
                }
            }

            return res;
        }


        template <typename AllocDescriptor>
        struct ranker {
            static constexpr feature_pack asked_features = Features;
            static constexpr feature_pack alloc_features =
              alloc::descriptors::allocator_features<AllocDescriptor>;

            static constexpr auto value = value_generator(alloc_features, asked_features);
        };

        template <typename AllocDescriptor, typename ResDescriptor>
        struct ranker<stl::pair<AllocDescriptor, ResDescriptor>> {
            static constexpr feature_pack asked_features = Features;
            static constexpr feature_pack alloc_features =
              alloc::descriptors::allocator_features<AllocDescriptor>;
            static constexpr feature_pack res_features =
              merge_features(alloc_features, alloc::descriptors::resource_features<ResDescriptor>);

            static constexpr auto value = value_generator(res_features, asked_features);
        };
    };

    /**
     * This type ranks the allocators, but you need to pass it the "allocator descriptors" which includes
     * the allocator features.
     */
    template <typename DescriptorList, feature_pack AskedFeatures>
    struct ranker : public ranker<alloc_res_pair_maker<DescriptorList>, AskedFeatures> {};

    template <template <typename...> typename TupleT,
              istl::Pair... AllocResPairType,
              feature_pack AskedFeatures>
    struct ranker<TupleT<AllocResPairType...>, AskedFeatures> {

        // the ranking should be used on each combination of "allocator" and its "resources";
        // sorting allocators only will not result in the best solution.

        using ranked =
          typename istl::ranked_types<ranking_condition<AskedFeatures>::template ranker, AllocResPairType...>;

        using best_descriptors_pair     = typename ranked::best::type;
        using best_allocator_descriptor = typename best_descriptors_pair::first_type;
        using best_resource_descriptor  = typename best_descriptors_pair::second_type;
        static constexpr bool has_resource =
          !stl::is_void_v<typename descriptors::storage<best_resource_descriptor>>;
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



    namespace details {
        template <template <typename> typename AllocType>
        struct allocator_replacer {
            template <typename T>
            struct replacer {
                static constexpr bool value = false;
                using type                  = void;
            };

            template <Allocator T>
            struct replacer<T> {
                static constexpr bool value = true;
                using value_type            = typename T::value_type;
                using type                  = AllocType<value_type>;
            };
        };
    } // namespace details

    template <typename T, template <typename> typename AllocType>
    using replace_allocators =
      istl::recursive_parameter_replacer<T, details::allocator_replacer<AllocType>::template replacer>;


    template <typename T, feature_pack FPack, AllocatorDescriptorList AllocDescList>
    struct alloc_finder {
        static constexpr feature_pack features = FPack;
        using ranked                           = ranker<AllocDescList, features>;
        using original_allocator_type          = typename T::allocator_type;
        using value_type                       = typename original_allocator_type::value_type;
        using resource_type  = descriptors::storage<typename ranked::best_resource_descriptor>;
        using allocator_type = typename descriptors::allocator<
          typename ranked::best_allocator_descriptor>::template type<value_type>;
        using new_type = replace_allocators<T, stl::allocator_traits<allocator_type>::template rebind_alloc>;
    };

    struct placeholder {};

    /**
     * The allocator pack type; this will hold a pack of allocators and their resources (if any)
     */
    template <AllocatorDescriptorList AllocDescriptorsType>
    struct allocator_pack {

        // the type is: tuple<AllocatorDescriptor, ...>
        using allocator_descriptors = AllocDescriptorsType;

        // the type is: tuple<ResourceDescriptor, ...>
        using resource_descriptors = resource_descriptor_extractor<allocator_descriptors>;

        // the type is: tuple<stl::pair<AllocatorDescriptor, ResourceDescriptor>, ...>
        using alloc_res_pairs = alloc_res_pair_maker<allocator_descriptors>;

        // a tuple of allocators
        template <typename T>
        using allocators_type = allocator_extractor<allocator_descriptors, T>;

        // a tuple of resources (not their descriptors)
        using resources_type = resource_extractor<allocator_descriptors>;

        // only the default constructible ones are here:
        using filtered_resources_type =
          typename istl::filter_parameters<stl::is_default_constructible, resources_type>::type;

        template <feature_pack FPack>
        using ranked = ranker<allocator_descriptors, FPack>;

        template <feature_pack FPack>
        using best_allocator_descriptor = typename ranked<FPack>::best_allocator_descriptor;

        template <feature_pack FPack, typename T>
        using best_allocator = typename best_allocator_descriptor<FPack>::template type<T>;

        template <AllocatorDescriptor AllocDescType>
        static constexpr bool has_allocator_descriptor =
          istl::contains_parameter<allocator_descriptors, AllocDescType>;

        template <template <typename> typename AllocType>
        static constexpr bool has_templated_allocator =
          istl::contains_parameter<allocators_type<char>, AllocType<char>>;

        template <Allocator AllocType>
        static constexpr bool has_allocator =
          istl::contains_parameter<allocators_type<typename AllocType::value_type>, AllocType>;

        template <typename ResourceType>
        static constexpr bool has_resource = istl::contains_parameter<resources_type, ResourceType>;

        template <typename ResourceType>
        static constexpr bool has_resource_object =
          istl::contains_parameter<filtered_resources_type, ResourceType>;

        template <ResourceDescriptor ResDescType>
        static constexpr bool has_resource_descriptor =
          istl::contains_parameter<resource_descriptors, ResDescType>;

        template <ResourceDescriptor ResDescType>
        static constexpr bool has_resource_descriptor_object =
          has_resource_object<alloc::descriptors::storage<ResDescType>>;

        template <typename T>
        using local_allocator_type = typename descriptors::allocator<
          typename ranked<local_features>::best_allocator_descriptor>::template type<T>;

        template <typename T>
        using general_allocator_type = typename descriptors::allocator<
          typename ranked<general_features>::best_allocator_descriptor>::template type<T>;

        using local_resource_type =
          descriptors::storage<typename ranked<local_features>::best_resource_descriptor>;

      private:
        [[no_unique_address]] filtered_resources_type resources{};

      public:
        allocator_pack(allocator_pack&&) noexcept = default;
        allocator_pack(allocator_pack const&)     = delete; // some resources are not copy-able; so ...

        allocator_pack(filtered_resources_type const& res) noexcept : resources{res} {};
        allocator_pack(filtered_resources_type&& res) noexcept : resources{stl::move(res)} {};

        template <typename... ResourceType>
        allocator_pack(ResourceType&&... res) noexcept
          : resources{istl::make_tuple_no_order<filtered_resources_type, ResourceType...>(
              stl::forward<ResourceType>(res)...)} {}

        allocator_pack() noexcept = default;


        template <typename ResourceType>
        requires(!ResourceDescriptor<ResourceType> && has_resource_object<ResourceType>)
          [[nodiscard]] auto& get_resource() noexcept {
            return stl::get<ResourceType>(resources);
        }

        template <ResourceDescriptor ResDescType>
        requires(has_resource_descriptor_object<ResDescType>) [[nodiscard]] auto& get_resource() noexcept {
            return get_resource<descriptors::storage<ResDescType>>();
        }

        template <feature_pack FPack>
        [[nodiscard]] auto& get_resource() noexcept {
            return get_resource<typename ranker<allocator_descriptors, FPack>::best_resource_descriptor>();
        }


        [[nodiscard]] auto& local_resource() noexcept {
            return get_resource<local_features>();
        }

        [[nodiscard]] auto& general_resource() noexcept {
            return get_resource<general_features>();
        }

        template <Allocator AllocType, typename ResType>
        requires(has_allocator<AllocType>) [[nodiscard]] auto get_allocator(ResType& res) noexcept {
            using resource_descriptor = resource_descriptor_finder<allocator_descriptors, AllocType, ResType>;
            using value_type          = typename AllocType::value_type;
            return descriptors::construct_allocator<resource_descriptor, value_type>(res);
        }

        template <Allocator AllocType, typename ResType>
        requires(has_allocator<AllocType>) [[nodiscard]] auto get_allocator() noexcept {
            using resource_descriptor = resource_descriptor_finder<allocator_descriptors, AllocType, ResType>;
            using value_type          = typename AllocType::value_type;
            if constexpr (!stl::is_void_v<descriptors::storage<resource_descriptor>>) {
                return descriptors::construct_allocator<resource_descriptor, value_type>(
                  get_resource<resource_descriptor>());
            } else {
                // for allocators that have no resource type
                return descriptors::construct_allocator<resource_descriptor, value_type>();
            }
        }

        /**
         * Get an allocator based on the specified resource
         */
        template <ResourceDescriptor ResDescType, typename T = stl::byte>
        [[nodiscard]] auto get_allocator(ResDescType& res) noexcept {
            using the_bad_alloc_type = decltype(descriptors::construct_allocator<ResDescType, T>(res));
            // replace allocator types inside T as well
            using new_type =
              replace_allocators<T, stl::allocator_traits<the_bad_alloc_type>::template rebind_alloc>;
            return descriptors::construct_allocator<ResDescType, new_type>(res);
        }

        template <ResourceDescriptor ResDescType, typename T = stl::byte>
        requires(has_resource_descriptor<ResDescType>) [[nodiscard]] auto get_allocator() noexcept {
            using res_type = alloc::descriptors::storage<ResDescType>;
            if constexpr (has_resource<res_type>) {
                auto& res                = get_resource<res_type>();
                using the_bad_alloc_type = decltype(descriptors::construct_allocator<ResDescType, T>(res));
                // replace allocator types inside T as well
                using new_type =
                  replace_allocators<T, stl::allocator_traits<the_bad_alloc_type>::template rebind_alloc>;
                return descriptors::construct_allocator<ResDescType, new_type>(res);
            } else {
                // The allocator doesn't have a resource
                using the_bad_alloc_type = decltype(descriptors::construct_allocator<ResDescType, T>());
                // replace allocator types inside T as well
                using new_type =
                  replace_allocators<T, stl::allocator_traits<the_bad_alloc_type>::template rebind_alloc>;
                return descriptors::construct_allocator<ResDescType, new_type>();
            }
        }


        template <feature_pack FPack, typename T = stl::byte>
        [[nodiscard]] auto get_allocator() noexcept {
            using the_ranked = ranked<FPack>;
            using best_allocator_template =
              typename descriptors::allocator<typename the_ranked::best_allocator_descriptor>;
            // replace allocators inside T with the new best allocator type
            using new_type            = replace_allocators<T, best_allocator_template::template type>;
            using best_allocator_type = typename best_allocator_template::template type<new_type>;
            if constexpr (the_ranked::has_resource) {
                using best_resource_desc = typename the_ranked::best_resource_descriptor;
                if constexpr (has_resource_descriptor_object<best_resource_desc>) {
                    auto& res = get_resource<best_resource_desc>();
                    return descriptors::construct_allocator<best_resource_desc, new_type>(res);
                } else {
                    // for allocators with no resource
                    return descriptors::construct_allocator<best_resource_desc, new_type>();
                }
            } else {
                // we don't have a resource
                return best_allocator_type{};
            }
        }

        template <typename T>
        [[nodiscard]] auto local_allocator() noexcept {
            return get_allocator<local_features, T>();
        }

        template <typename T>
        [[nodiscard]] auto general_allocator() noexcept {
            return get_allocator<general_features, T>();
        }


        // todo: you can remove AllocType here
        template <typename T, template <typename> typename AllocType, typename ResDescType, typename... Args>
        requires(
          has_templated_allocator<AllocType> &&
          (ResourceDescriptor<ResDescType> ||
           stl::is_void_v<ResDescType>) ) // the resource might be void if the allocator is resource-less
          constexpr auto make(Args&&... args) {
            if constexpr (!requires { typename T::allocator_type; }) {
                // doesn't have an allocator, so construct a normal object
                return T{stl::forward<Args>(args)...};
            } else {
                using old_allocator_type = typename T::allocator_type;
                using value_type         = typename old_allocator_type::value_type;
                using selected_allocator = AllocType<value_type>;
                using new_type =
                  replace_allocators<T, stl::allocator_traits<selected_allocator>::template rebind_alloc>;
                auto the_alloc = this->get_allocator<ResDescType, value_type>();
                if constexpr (istl::contains_parameter<type_list<Args...>, placeholder>) {
                    return new_type{
                      istl::replace_object<placeholder, selected_allocator, Args>(stl::forward<Args>(args),
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

        template <typename T, Allocator AllocType, ResourceDescriptor ResDescType, typename... Args>
        constexpr auto make(Args&&... args) {
            return this
              ->make<T, stl::allocator_traits<AllocType>::template rebind_alloc, ResDescType, Args...>(
                stl::forward<Args>(args)...);
        }

        template <typename T, feature_pack FPack, typename... Args>
        constexpr auto make(Args&&... args) {
            if constexpr (FPack.empty()) {
                using old_allocator_type = typename T::allocator_type;
                if constexpr (allocator_pack::template has_allocator<old_allocator_type>) {
                    return this->make<T, old_allocator_type, Args...>(stl::forward<Args>(args)...);
                } else {
                    static_assert(false && sizeof(allocator_pack),
                                  "We don't have an allocator for this type, and you didn't specify "
                                  "the features you'd like your allocator to have so we don't know "
                                  "which allocator to choose.");
                }
            } else {
                using best_choice        = ranked<FPack>;
                using best_resource_desc = typename best_choice::best_resource_descriptor;
                using best_allocator =
                  alloc::descriptors::allocator<typename best_choice::best_allocator_descriptor>;
                return this->make<T,
                                  best_allocator::template type, // best allocator
                                  best_resource_desc,
                                  Args...>(stl::forward<Args>(args)...);
            }
        }


        template <typename T, typename... Args>
        constexpr auto local(Args&&... args) {
            return this->make<T, local_features, Args...>(stl::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        constexpr auto general(Args&&... args) {
            return this->make<T, general_features, Args...>(stl::forward<Args>(args)...);
        }
    };



    template <typename T, feature_pack FPack, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make(allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return alloc_pack.template make<T, FPack, Args...>(stl::forward<Args>(args)...);
    }


    template <typename AllocPackType>
    concept AllocatorPack = requires {
        typename AllocPackType::allocator_descriptors;
        requires stl::same_as<AllocPackType, allocator_pack<typename AllocPackType::allocator_descriptors>>;
    };


} // namespace webpp::alloc

#endif // WEBPP_ALLOCATOR_PACK_HPP
