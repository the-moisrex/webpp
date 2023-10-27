// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_PACK_HPP
#define WEBPP_ALLOCATOR_PACK_HPP

#include "../std/concepts.hpp"
#include "../std/memory.hpp"
#include "../std/type_traits.hpp"
#include "allocator_concepts.hpp"

#include <cstdint>

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

    enum features : stl::uint_fast8_t {
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

    // every feature has to have an opposite feature so the child can overwrite the parent's decision.
    struct feature_pack {
        using value_type = bool;

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        value_type m_sync : 1   = false;
        value_type m_unsync : 1 = false;

        value_type m_noop_dealloc : 1    = false;
        value_type m_no_noop_dealloc : 1 = false;

        value_type m_stateful : 1  = false;
        value_type m_stateless : 1 = false;

        value_type m_high_contention : 1 = false;
        value_type m_low_contention : 1  = false;

        value_type m_high_utilization : 1 = false;
        value_type m_low_utilization : 1  = false;

        value_type m_high_locality : 1 = false;
        value_type m_low_locality : 1  = false;
        // NOLINTEND(misc-non-private-member-variables-in-classes)

        template <typename... FeaturesType>
            requires((stl::same_as<stl::remove_cvref_t<FeaturesType>, features> && ...))
        constexpr feature_pack(FeaturesType... values) {
            set_features(values...);
        }

        constexpr feature_pack(const feature_pack&) noexcept            = default;
        constexpr feature_pack(feature_pack&&) noexcept                 = default;
        constexpr feature_pack& operator=(feature_pack const&) noexcept = default;
        constexpr feature_pack& operator=(feature_pack&&) noexcept      = default;
        constexpr ~feature_pack() noexcept                              = default;

        template <typename... FeaturesType>
            requires((stl::same_as<stl::remove_cvref_t<FeaturesType>, features> && ...))
        constexpr void set_features(FeaturesType... values) noexcept {
            (([this](features feature) {
                 switch (feature) {
                     using enum features;
                     case sync: set_sync(true); break;
                     case unsync: set_sync(false); break;
                     case noop_dealloc: set_noop_dealloc(true); break;
                     case no_noop_dealloc: set_noop_dealloc(false); break;
                     case stateful: set_stateful(true); break;
                     case stateless: set_stateful(false); break;
                     case high_contention: set_high_contention(true); break;
                     case low_contention: set_high_contention(false); break;
                     case high_utilization: set_high_utilization(true); break;
                     case low_utilization: set_high_utilization(false); break;
                     case high_locality: set_high_locality(true); break;
                     case low_locality: set_high_locality(false); break;
                 }
             })(values),
             ...);
        }

        [[nodiscard]] constexpr bool is(features feature) const noexcept {
            switch (feature) {
                using enum features;
                case sync: return is_sync();
                case unsync: return !is_sync();
                case noop_dealloc: return is_noop_dealloc();
                case no_noop_dealloc: return !is_noop_dealloc();
                case stateful: return is_stateful();
                case stateless: return is_stateless();
                case high_contention: return is_high_contention();
                case low_contention: return !is_high_contention();
                case high_utilization: return is_high_utilization();
                case low_utilization: return !is_high_utilization();
                case high_locality: return is_high_locality();
                case low_locality: return !is_high_locality();
            }
            return false;
        }

        [[nodiscard]] constexpr bool specified(features feature) const noexcept {
            switch (feature) {
                using enum features;
                case sync:
                case unsync: return specified_sync();
                case noop_dealloc:
                case no_noop_dealloc: return specified_noop_dealloc();
                case stateful:
                case stateless: return specified_state();
                case high_contention:
                case low_contention: return specified_contention();
                case high_utilization:
                case low_utilization: return specified_utilization();
                case high_locality:
                case low_locality: return specified_locality();
            }
            return false;
        }

        constexpr void set_sync(value_type val) noexcept {
            m_sync   = val;
            m_unsync = !val;
        }

        constexpr void set_noop_dealloc(value_type val) noexcept {
            m_noop_dealloc    = val;
            m_no_noop_dealloc = !val;
        }


        constexpr void set_stateful(value_type val) noexcept {
            m_stateful  = val;
            m_stateless = !val;
        }

        constexpr void set_high_contention(value_type val) noexcept {
            m_high_contention = val;
            m_low_contention  = !val;
        }


        constexpr void set_high_utilization(value_type val) noexcept {
            m_high_utilization = val;
            m_low_utilization  = !val;
        }


        constexpr void set_high_locality(value_type val) noexcept {
            m_high_locality = val;
            m_low_locality  = !val;
        }

        [[nodiscard]] constexpr value_type is_sync() const noexcept {
            // sync is required that's why we don't check the unsync here
            return m_sync;
        }

        [[nodiscard]] constexpr value_type is_noop_dealloc() const noexcept {
            return m_noop_dealloc && !m_no_noop_dealloc;
        }

        [[nodiscard]] constexpr value_type is_stateful() const noexcept {
            return m_stateful; // stateful is a required boolean, that's why we don't check the stateless
        }

        [[nodiscard]] constexpr value_type is_stateless() const noexcept {
            return m_stateless; // stateful is a required boolean, that's why we don't check the stateful
        }

        [[nodiscard]] constexpr value_type is_high_contention() const noexcept {
            return m_high_contention && !m_low_contention;
        }

        [[nodiscard]] constexpr value_type is_high_utilization() const noexcept {
            return m_high_utilization && !m_low_utilization;
        }

        [[nodiscard]] constexpr value_type is_high_locality() const noexcept {
            return m_high_locality && !m_low_locality;
        }



        [[nodiscard]] constexpr value_type specified_sync() const noexcept {
            return m_sync || m_unsync;
        }

        [[nodiscard]] constexpr value_type specified_noop_dealloc() const noexcept {
            return m_noop_dealloc || m_no_noop_dealloc;
        }

        [[nodiscard]] constexpr value_type specified_state() const noexcept {
            return m_stateful || m_stateless;
        }

        [[nodiscard]] constexpr value_type specified_contention() const noexcept {
            return m_high_contention || m_low_contention;
        }

        [[nodiscard]] constexpr value_type specified_utilization() const noexcept {
            return m_high_utilization || m_low_utilization;
        }

        [[nodiscard]] constexpr value_type specified_locality() const noexcept {
            return m_high_locality || m_low_locality;
        }

        [[nodiscard]] constexpr value_type is_empty() const noexcept {
            return !specified_state() && !specified_sync() && !specified_contention() &&
                   !specified_locality() && !specified_noop_dealloc() && !specified_utilization();
        }



        /**
         * Merge two feature packs. One of them is the parent, and the other one is the child.
         * The Child's feature will overwrite the parent's feature if there's a conflict.
         */
        [[nodiscard]] static constexpr feature_pack merge_features(feature_pack parent,
                                                                   feature_pack child) noexcept {
            feature_pack pack;
            if (parent.is_sync() || child.is_sync())
                pack.set_sync(true);
            if (parent.is_noop_dealloc() || child.is_noop_dealloc())
                pack.set_noop_dealloc(true);
            if (parent.is_high_contention() || child.is_high_contention())
                pack.set_high_contention(true);
            if (parent.is_high_utilization() || child.is_high_utilization())
                pack.set_high_utilization(true);
            if (parent.is_high_locality() || child.is_high_locality())
                pack.set_high_locality(true);
            return pack;
        }


        // only for those that are not a required feature; only include one of conflicting features.
        // the feature that if it's present it's usually better should be present here.
        [[nodiscard]] constexpr static long long int rank(features feature) noexcept {
            // NOLINTBEGIN(*-magic-numbers)
            switch (feature) {
                using enum features;
                case sync:
                case unsync: return 200;
                case noop_dealloc:
                case no_noop_dealloc: return 50;
                case stateful:
                case stateless: return 200;
                case high_contention:
                case low_contention: return 20;
                case high_utilization:
                case low_utilization: return 10;
                case high_locality:
                case low_locality: return 30;
            }
            return 0;
            // NOLINTEND(*-magic-numbers)
        }


        [[nodiscard]] constexpr long long int rank(feature_pack asked_features) const noexcept {
            long long int res = 100; // NOLINT(*-magic-numbers)

            // Checking required features first:
            if ((asked_features.specified_state() && specified_state()) ||
                (asked_features.is_sync() && specified_sync())) {
                return res * -1;
            }

            for (features const feature : {noop_dealloc, high_contention, high_utilization, high_locality}) {
                const auto points = rank(feature);
                if (asked_features.specified(feature) && specified(feature)) {
                    res += asked_features.is(feature) == is(feature) ? points : -points;
                } else if (specified(feature)) {
                    // the user didn't ask for it, but half a point for those allocators that have this
                    // feature and take half the point for those that have the opposite of this feature
                    res += asked_features.is(feature) == is(feature) ? (points / 2) : -(points / 2);
                }
            }
            return res;
        }
    };

    // common allocator features
    // todo: complete this list
    inline constexpr auto monotonic_features   = feature_pack{stateful, noop_dealloc, unsync};
    inline constexpr auto sync_pool_features   = feature_pack{sync, stateful};
    inline constexpr auto unsync_pool_features = feature_pack{stateful, unsync};
    inline constexpr auto local_features       = monotonic_features;
    inline constexpr auto general_features     = feature_pack{stateless, sync};



    /**
     * This type ranks each allocator descriptor
     */
    template <feature_pack Features>
    struct ranking_condition {

        template <typename AllocDescriptor>
        struct ranker {
            static constexpr feature_pack asked_features = Features;
            static constexpr feature_pack alloc_features =
              alloc::descriptors::allocator_features<AllocDescriptor>;

            static constexpr auto value = alloc_features.rank(asked_features);
        };

        template <typename AllocDescriptor, typename ResDescriptor>
        struct ranker<stl::pair<AllocDescriptor, ResDescriptor>> {
            static constexpr feature_pack asked_features = Features;
            static constexpr feature_pack alloc_features =
              alloc::descriptors::allocator_features<AllocDescriptor>;
            static constexpr feature_pack res_features =
              feature_pack::merge_features(alloc_features,
                                           alloc::descriptors::resource_features<ResDescriptor>);

            static constexpr auto value = res_features.rank(asked_features);
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
                using type                  = T;
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
        using resource_type = descriptors::storage<typename ranked::best_resource_descriptor>;

        // resource field type can be used as a class field and can be inherited.
        using resource_type_field =
          stl::conditional_t<stl::is_void_v<resource_type>, istl::nothing_type, resource_type>;
        using allocator_type = typename descriptors::allocator<
          typename ranked::best_allocator_descriptor>::template type<value_type>;
        using new_type = replace_allocators<T, stl::allocator_traits<allocator_type>::template rebind_alloc>;
    };

    // specializes the alloc_finder, so it can work with the types that don't support allocators
    template <typename T, feature_pack FPack, AllocatorDescriptorList AllocDescList>
        requires(!requires { typename T::allocator_type; }) // doesn't have allocator_type
    struct alloc_finder<T, FPack, AllocDescList> {
        static constexpr feature_pack features = FPack;
        using ranked                           = ranker<AllocDescList, features>;
        using value_type                       = stl::byte;
        using original_allocator_type          = typename descriptors::allocator<
          typename ranked::best_allocator_descriptor>::template type<value_type>;
        using resource_type = descriptors::storage<typename ranked::best_resource_descriptor>;

        // resource field type can be used as a class field and can be inherited.
        using resource_type_field =
          stl::conditional_t<stl::is_void_v<resource_type>, istl::nothing_type, resource_type>;
        using allocator_type = original_allocator_type;
        using new_type       = T;
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
        using best_allocator =
          typename descriptors::allocator<best_allocator_descriptor<FPack>>::template type<T>;

        template <AllocatorDescriptor AllocDescType>
        static constexpr bool has_allocator_descriptor =
          istl::contains_parameter_of<allocator_descriptors, AllocDescType>;

        template <template <typename> typename AllocType>
        static constexpr bool has_templated_allocator =
          istl::contains_parameter_of<allocators_type<char>, AllocType<char>>;

        template <Allocator AllocType>
        static constexpr bool has_allocator =
          istl::contains_parameter_of<allocators_type<typename AllocType::value_type>, AllocType>;

        template <typename ResourceType>
        static constexpr bool has_resource = istl::contains_parameter_of<resources_type, ResourceType>;

        template <typename ResourceType>
        static constexpr bool has_resource_object =
          istl::contains_parameter_of<filtered_resources_type, ResourceType>;

        template <ResourceDescriptor ResDescType>
        static constexpr bool has_resource_descriptor =
          istl::contains_parameter_of<resource_descriptors, ResDescType>;

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


        template <typename T, Allocator A>
        using unique_ptr_type = std::unique_ptr<T, istl::alloc_deleter<T, A>>;

        template <typename T>
        using local_unique_ptr = unique_ptr_type<T, general_allocator_type<T>>;

        template <typename T>
        using general_unique_ptr = unique_ptr_type<T, general_allocator_type<T>>;

      private:
        [[no_unique_address]] filtered_resources_type resources{};

      public:
        constexpr allocator_pack(allocator_pack&&) noexcept = default;
        constexpr allocator_pack(allocator_pack const&) = default; // some resources are not copy-able; so ...

        constexpr allocator_pack(filtered_resources_type const& res) noexcept : resources{res} {};
        constexpr allocator_pack(filtered_resources_type&& res) noexcept : resources{stl::move(res)} {};

        template <typename... ResourceType>
            requires((has_resource<stl::decay_t<ResourceType>> && ...))
        constexpr allocator_pack(ResourceType&&... res) noexcept
          : resources{istl::make_tuple_no_order<filtered_resources_type, ResourceType...>(
              stl::forward<ResourceType>(res)...)} {}

        constexpr allocator_pack() noexcept  = default;
        constexpr ~allocator_pack() noexcept = default;

        constexpr allocator_pack& operator=(const allocator_pack&) noexcept { // NOLINT(cert-oop54-cpp)
            // do nothing; really
            return *this;
        }
        constexpr allocator_pack& operator=(allocator_pack&&) noexcept = default;


        template <typename ResourceType>
            requires(!ResourceDescriptor<ResourceType> && has_resource_object<ResourceType>)
        [[nodiscard]] constexpr auto& get_resource() noexcept {
            return stl::get<ResourceType>(resources);
        }

        template <ResourceDescriptor ResDescType>
            requires(has_resource_descriptor_object<ResDescType>)
        [[nodiscard]] constexpr auto& get_resource() noexcept {
            return get_resource<descriptors::storage<ResDescType>>();
        }

        template <feature_pack FPack>
        [[nodiscard]] constexpr auto& get_resource() noexcept {
            return get_resource<typename ranker<allocator_descriptors, FPack>::best_resource_descriptor>();
        }


        [[nodiscard]] constexpr auto& local_resource() noexcept {
            return get_resource<local_features>();
        }

        [[nodiscard]] constexpr auto& general_resource() noexcept {
            return get_resource<general_features>();
        }

        template <Allocator AllocType, typename ResType>
            requires(has_allocator<AllocType>)
        [[nodiscard]] constexpr auto get_allocator(ResType& res) noexcept {
            using resource_descriptor = resource_descriptor_finder<allocator_descriptors, AllocType, ResType>;
            using value_type          = typename AllocType::value_type;
            return descriptors::construct_allocator<resource_descriptor, value_type>(res);
        }

        template <Allocator AllocType, typename ResType>
            requires(has_allocator<AllocType>)
        [[nodiscard]] constexpr auto get_allocator() noexcept {
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
        [[nodiscard]] constexpr auto get_allocator(ResDescType& res) noexcept {
            using the_bad_alloc_type = decltype(descriptors::construct_allocator<ResDescType, T>(res));
            // replace allocator types inside T as well
            using new_type =
              replace_allocators<T, stl::allocator_traits<the_bad_alloc_type>::template rebind_alloc>;
            return descriptors::construct_allocator<ResDescType, new_type>(res);
        }

        template <ResourceDescriptor ResDescType, typename T = stl::byte>
            requires(has_resource_descriptor<ResDescType>)
        [[nodiscard]] constexpr auto get_allocator() noexcept {
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


        template <feature_pack FPack, typename T>
        [[nodiscard]] constexpr auto get_allocator() noexcept {
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

        // This one doesn't change the allocator type you passed to it, because you didn't give it your
        // preferred allocator type
        // We will use the default resource
        template <Allocator AllocType>
        [[nodiscard]] constexpr auto get_allocator() noexcept {
            using alloc_traits = stl::allocator_traits<AllocType>;
            using alloc_desc =
              allocator_descriptor_finder<allocator_descriptors, alloc_traits::template rebind_alloc>;

            static_assert(AllocatorDescriptor<alloc_desc>,
                          "We can't find the allocator descriptor for the specified allocator type.");

            static_assert(allocator_pack::template has_allocator<AllocType>,
                          "We don't have an allocator for this type, and you didn't specify "
                          "the features you'd like your allocator to have so we don't know "
                          "which allocator to choose.");

            // using the default resource
            using resource_desc = typename alloc_desc::default_resource;
            return get_allocator<AllocType, resource_desc>();
        }

        template <typename T>
            requires(requires { typename T::allocator_type; })
        [[nodiscard]] constexpr auto get_allocator_for() noexcept {
            using type_allocator = typename T::allocator_type;
            if constexpr (has_allocator<type_allocator>) {
                return get_allocator<type_allocator>();
            } else {
                static_assert(false && sizeof(T),
                              "This allocator pack doesn't support the specified "
                              "allocator, you have to change the allocator; "
                              "use make utility.");
            }
        }

        template <feature_pack FPack, typename T>
            requires(requires { typename T::allocator_type; })
        [[nodiscard]] constexpr auto featured_alloc_for() noexcept {
            using type_allocator = typename T::allocator_type;
            if constexpr (has_allocator<type_allocator>) {
                return get_allocator<FPack, type_allocator>();
            } else {
                static_assert(false && sizeof(T),
                              "This allocator pack doesn't support the specified "
                              "allocator, you have to change the allocator; "
                              "use make utility.");
            }
        }

        template <typename T>
        [[nodiscard]] constexpr auto local_alloc_for() noexcept {
            return featured_alloc_for<local_features, T>();
        }

        template <typename T>
        [[nodiscard]] constexpr auto general_alloc_for() noexcept {
            return featured_alloc_for<general_features, T>();
        }

        template <typename T>
        [[nodiscard]] constexpr auto local_allocator() noexcept {
            return get_allocator<local_features, T>();
        }

        template <typename T>
        [[nodiscard]] constexpr auto general_allocator() noexcept {
            return get_allocator<general_features, T>();
        }


        // todo: you can remove AllocType here
        template <typename T, template <typename> typename AllocType, typename ResDescType, typename... Args>
            requires(
              has_templated_allocator<AllocType> &&
              (ResourceDescriptor<ResDescType> ||
               stl::is_void_v<ResDescType>) ) // the resource might be void if the allocator is resource-less
        [[nodiscard]] constexpr auto make(Args&&... args) {
            if constexpr (!requires { typename T::allocator_type; }) {
                // doesn't have an allocator, so construct a normal object
                return T{stl::forward<Args>(args)...};
            } else {
                using old_allocator_type = typename T::allocator_type;
                using value_type         = typename old_allocator_type::value_type;
                using selected_allocator = AllocType<value_type>;
                using new_type =
                  replace_allocators<T, stl::allocator_traits<selected_allocator>::template rebind_alloc>;
                return this->make<new_type, ResDescType, Args...>(stl::forward<Args>(args)...);
            }
        }

        // construct T with resource descriptor and allocator of T
        template <typename T, ResourceDescriptor ResDescType, typename... Args>
            requires requires { typename T::allocator_type; }
        [[nodiscard]] constexpr auto make(Args&&... args) {
            using alloc_type = typename T::allocator_type;
            using value_type = typename alloc_type::value_type;
            auto the_alloc   = this->get_allocator<ResDescType, value_type>();
            if constexpr (requires { T::create(the_alloc, stl::forward<Args>(args)...); }) {
                return T::create(the_alloc, stl::forward<Args>(args)...);
            } else if constexpr (requires { T::create(stl::forward<Args>(args)..., the_alloc); }) {
                return T::create(stl::forward<Args>(args)..., the_alloc);
            } else if constexpr (istl::contains_parameter_of<type_list<Args...>, placeholder>) {
                return T{istl::replace_object<placeholder, alloc_type, Args>(stl::forward<Args>(args),
                                                                             the_alloc)...};
            } else if constexpr (requires {
                                     T{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
                                 }) {
                // as the first and second argument
                return T{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
            } else if constexpr (requires { T{stl::forward<Args>(args)..., the_alloc}; }) {
                // as the last argument
                return T{stl::forward<Args>(args)..., the_alloc};
            } else {
                static_assert(false && sizeof(T),
                              "We don't know how to pass the allocator to the specified type.");
                return T{stl::forward<Args>(args)...};
            }
        }

        // This one doesn't change the allocator type you passed to it, because you didn't give it your
        // preferred allocator type
        // We will use the default resource
        template <typename T, typename... Args>
            requires requires { typename T::allocator_type; }
        [[nodiscard]] constexpr T make(Args&&... args) {
            using alloc_type   = typename T::allocator_type;
            using alloc_traits = stl::allocator_traits<alloc_type>;
            using alloc_desc =
              allocator_descriptor_finder<allocator_descriptors, alloc_traits::template rebind_alloc>;

            static_assert(AllocatorDescriptor<alloc_desc>,
                          "We can't find the allocator descriptor for the specified allocator type.");

            static_assert(allocator_pack::template has_allocator<alloc_type>,
                          "We don't have an allocator for this type, and you didn't specify "
                          "the features you'd like your allocator to have so we don't know "
                          "which allocator to choose.");

            // using the default resource
            using resource_desc = typename alloc_desc::default_resource;
            return this->make<T, resource_desc, Args...>(stl::forward<Args>(args)...);
        }

        template <typename T, feature_pack FPack, typename... Args>
        [[nodiscard]] constexpr auto make(Args&&... args) {
            if constexpr (FPack.is_empty()) {
                return this->make<T, Args...>(stl::forward<Args>(args)...);
            } else {
                using best_choice        = ranked<FPack>;
                using best_resource_desc = typename best_choice::best_resource_descriptor;
                using best_allocator_desc =
                  alloc::descriptors::allocator<typename best_choice::best_allocator_descriptor>;
                return this->make<T,
                                  best_allocator_desc::template type, // best allocator
                                  best_resource_desc,
                                  Args...>(stl::forward<Args>(args)...);
            }
        }

        // this function will return a std::unique_ptr<T, ...> that's allocated on the specified allocator
        template <typename T, feature_pack FPack, typename... Args>
        [[nodiscard]] constexpr auto allocate_unique(Args&&... args) {
            static_assert(!FPack.is_empty(), "What does it mean?");
            return istl::allocate_unique<T>(get_allocator<FPack, T>(), stl::forward<Args>(args)...);
        }

        // this function will return a std::unique_ptr<T, ...> that's allocated on the specified allocator
        template <typename T, Allocator AllocType, typename... Args>
        [[nodiscard]] constexpr auto allocate_unique(Args&&... args) {
            return istl::allocate_unique<T>(get_allocator<AllocType>(), stl::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        [[nodiscard]] constexpr auto allocate_unique_local(Args&&... args) {
            return this->template allocate_unique<T, local_features, Args...>(stl::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        [[nodiscard]] constexpr auto allocate_unique_general(Args&&... args) {
            return this->template allocate_unique<T, general_features, Args...>(stl::forward<Args>(args)...);
        }


        template <typename T, typename... Args>
        [[nodiscard]] constexpr auto local(Args&&... args) {
            return this->make<T, local_features, Args...>(stl::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        [[nodiscard]] constexpr auto general(Args&&... args) {
            return this->make<T, general_features, Args...>(stl::forward<Args>(args)...);
        }
    };



    template <typename T, feature_pack FPack, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make(allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return alloc_pack.template make<T, FPack, Args...>(stl::forward<Args>(args)...);
    }


    template <typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto local_allocator(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template local_allocator<T>();
    }

    template <typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto general_allocator(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template general_allocator<T>();
    }

    template <typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto allocator_for(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template get_allocator_for<T>();
    }

    template <feature_pack FPack, typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto featured_alloc_for(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template featured_alloc_for<FPack, T>();
    }

    template <feature_pack FPack, typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto featured_alloc(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template get_allocator<FPack, T>();
    }

    template <typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto local_alloc_for(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template local_alloc_for<T>();
    }

    template <typename T, AllocatorDescriptorList AllocDescType>
    static constexpr auto general_alloc_for(allocator_pack<AllocDescType>& alloc_pack) noexcept {
        return alloc_pack.template featured_alloc_for<general_features, T>();
    }

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto allocate_unique_local(allocator_pack<AllocDescType>& alloc_pack,
                                                Args&&... args) noexcept {
        return alloc_pack.template allocate_unique_local<T>(stl::forward<Args>(args)...);
    }

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto allocate_unique_general(allocator_pack<AllocDescType>& alloc_pack,
                                                  Args&&... args) noexcept {
        return alloc_pack.template allocate_unique_general<T>(stl::forward<Args>(args)...);
    }

    // todo: local_allocator_for_container(alloc_pack, obj) -> finds the containers' value types


    // Check if the specified type is an allocator_pack
    template <typename AllocPackType>
    concept AllocatorPack = requires {
        typename stl::remove_cvref_t<AllocPackType>::allocator_descriptors;
        requires stl::same_as<
          stl::remove_cvref_t<AllocPackType>,
          allocator_pack<typename stl::remove_cvref_t<AllocPackType>::allocator_descriptors>>;
    };


    template <typename AllocType, typename AllocPack>
    concept HasAllocator = AllocatorPack<AllocPack> && AllocPack::template has_allocator<AllocType>;

    template <typename T, typename AllocPack>
    concept HasAllocatorFor =
      requires { typename T::allocator_type; } && HasAllocator<typename T::allocator_type, AllocPack>;

} // namespace webpp::alloc

#endif // WEBPP_ALLOCATOR_PACK_HPP
