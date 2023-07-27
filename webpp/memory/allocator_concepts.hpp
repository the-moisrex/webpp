// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_CONCEPTS_HPP
#define WEBPP_ALLOCATOR_CONCEPTS_HPP

#include "../std/tuple.hpp"

#include <memory>

namespace webpp {


    // Name requirement: https://en.cppreference.com/w/cpp/named_req/Allocator
    template <typename A>
    concept AllocatorType = requires {
        typename A::value_type;
        typename stl::allocator_traits<A>::pointer;
        typename stl::allocator_traits<A>::size_type;
        typename stl::allocator_traits<A>::const_pointer;
        typename stl::allocator_traits<A>::void_pointer;
        typename stl::allocator_traits<A>::const_void_pointer;
        requires requires(A                                                     a,
                          A                                                     a1,
                          typename stl::allocator_traits<A>::size_type          n,
                          typename stl::allocator_traits<A>::pointer            p,
                          typename stl::allocator_traits<A>::const_pointer      cp,
                          typename stl::allocator_traits<A>::void_pointer       vp,
                          typename stl::allocator_traits<A>::const_void_pointer cvp,
                          decltype(*p)                                          r) {
            { a.allocate(n) };
            { *p };
            { *cp };
            // {p->m};
            // {cp->m};
            { static_cast<typename stl::allocator_traits<A>::pointer>(vp) };
            { static_cast<typename stl::allocator_traits<A>::const_pointer>(cvp) };
            { stl::pointer_traits<typename stl::allocator_traits<A>::pointer>::pointer_to(r) };
            { a == a1 };
            { a != a1 };
            { A(a) };
            // {A a1 = a};
            // A a(b)};
            { A(stl::move(a)) };
            // {A a1 = stl::move(a)};
            // {A a(stl::move(b))};
        };
    };

    template <typename A>
    concept Allocator = AllocatorType<stl::remove_cvref_t<A>>;

    template <typename T, typename Alloc>
    concept AllocatorOf = Allocator<Alloc> && stl::is_same_v<typename Alloc::value_type, T>;

    // static_assert(Allocator<stl::allocator<int>>, "There's a problem with Allocator concept");

    /**
     * Types of allocator that the core library may need based on the inputs:
     *   1. limited space on stack (with the max limit known)
     *   2. unlimited long-standing on heap
     *   3. default-constructible, general allocators
     *   4. singleton allocators that are passed to us
     *   5. copyable allocators that are passed to us
     */
    template <typename I>
    concept ResourceDescriptor = requires {
        typename I::storage_type; // the resource type
        { I::resource_features }; // the resource features of type alloc::feature_pack
    };

    // one single allocator descriptor which describes an allocator and its features and its resources
    template <typename D>
    concept AllocatorDescriptor = requires {
        typename D::template allocator<char>; // get the allocator itself
        typename D::resources;                // resource types (a list of MemoryResource)
        { D::allocator_features };            // parent features of type alloc::feature_pack
        typename D::default_resource;
        requires ResourceDescriptor<typename D::default_resource>;
    };

    // these are that help other TMPs to extract information out of the two descriptor types.
    namespace alloc::descriptors {

        template <AllocatorDescriptor DescType>
        struct allocator {
            template <typename T>
            using type = typename DescType::template allocator<T>;
        };

        template <AllocatorDescriptor T>
        using resources = typename T::resources;

        template <ResourceDescriptor T>
        using storage = typename T::storage_type;

        template <AllocatorDescriptor T>
        static constexpr auto allocator_features = T::allocator_features;

        template <ResourceDescriptor T>
        static constexpr auto resource_features = T::resource_features;


        template <ResourceDescriptor RD, typename T>
        static inline auto construct_allocator(storage<RD>& res) noexcept {
            return RD::template construct_allocator<T>(res);
        }

        template <ResourceDescriptor RD, typename T>
        static inline auto construct_allocator() noexcept {
            return RD::template construct_allocator<T>();
        }

    } // namespace alloc::descriptors

    namespace details {
        template <typename T>
        struct allocator_descriptor_validator {
            static constexpr bool value = AllocatorDescriptor<T>;
        };

        template <typename T>
        struct allocator_validator {
            static constexpr bool value = Allocator<T>;
        };
    } // namespace details

    // a list of allocator descriptors
    template <typename D>
    concept AllocatorDescriptorList = istl::TupleOf<details::allocator_descriptor_validator, D>;


    template <typename... AllocatorDescriptorsTypes>
    using type_list = stl::tuple<AllocatorDescriptorsTypes...>;




    namespace details {
        /**
         * Extract the allocators (flatten the allocator descriptor list)
         */
        template <AllocatorDescriptorList AllocDescTypes>
        struct allocator_extractor_impl;

        template <template <typename...> typename TupleT, AllocatorDescriptor... AllocDescType>
        struct allocator_extractor_impl<TupleT<AllocDescType...>> {

            template <typename T>
            using type = TupleT<typename alloc::descriptors::allocator<AllocDescType>::template type<T>...>;
        };



        /**
         * Extracts all the "resources descriptors" from the "allocator descriptor list"
         * the type: tuple<ResourceDescriptor, ...>
         */
        template <AllocatorDescriptorList AllocDescTypes>
        struct resource_descriptor_extractor_impl;

        template <template <typename...> typename TupleType, AllocatorDescriptor... AllocDescType>
        struct resource_descriptor_extractor_impl<TupleType<AllocDescType...>>
          : public istl::merge_parameters_type<alloc::descriptors::resources<AllocDescType>...> {};





        /**
         * Extract all the "resources" from an "allocator descriptor list".
         * the type: type_list<Resource>
         */
        template <typename AllocDescTypes>
        struct resource_extractor_impl;

        template <template <typename...> typename TupleType, AllocatorDescriptor... AllocDescType>
        struct resource_extractor_impl<TupleType<AllocDescType...>>
          : public istl::merge_parameters_type<
              typename resource_extractor_impl<alloc::descriptors::resources<AllocDescType>>::type...> {};

        template <template <typename...> typename TupleType, ResourceDescriptor... ResDescType>
        struct resource_extractor_impl<TupleType<ResDescType...>> {
            using type = TupleType<alloc::descriptors::storage<ResDescType>...>;
        };




        template <AllocatorDescriptorList AllocDescTypes>
        struct allocator_flattener_impl;

        template <AllocatorDescriptor... AllocDescType>
        struct allocator_flattener_impl<type_list<AllocDescType...>> {

            template <typename T>
            struct type : public alloc::descriptors::allocator<AllocDescType>::template type<T>... {};
        };



        /**
         * Extract "memory resource descriptors" from an allocator descriptor
         * in form of std::pair<AllocatorDescriptor, ResourceDescriptor>
         */
        template <AllocatorDescriptorList AllocDescType, typename TheSame = AllocDescType>
        struct alloc_res_pair_maker_impl;

        template <template <typename...> typename TupleT, AllocatorDescriptor... AllocDescType>
        struct alloc_res_pair_maker_impl<TupleT<AllocDescType...>>
          : public istl::merge_parameters<typename alloc_res_pair_maker_impl<AllocDescType>::type...> {};

        template <template <typename...> typename TupleT, AllocatorDescriptor AllocDescType>
        struct alloc_res_pair_maker_impl<TupleT<AllocDescType>> {

            template <typename T>
            struct pair_maker;

            template <template <typename...> typename TupleT2, ResourceDescriptor... ResDesc>
            struct pair_maker<TupleT2<ResDesc...>> {
                using type = TupleT2<stl::pair<AllocDescType, ResDesc>...>;
            };

            using type = typename pair_maker<alloc::descriptors::resources<AllocDescType>>::type;
        };

        template <typename ResourceList, typename ResType>
        struct resource_descriptor_finder {
            template <typename T>
            using condition = stl::bool_constant<stl::is_same_v<alloc::descriptors::storage<T>, ResType> ||
                                                 stl::is_same_v<T, ResType>>;

            using type = istl::first_parameter<istl::filter_parameters_t<condition, ResourceList>>;
        };

        template <AllocatorDescriptorList AllocDescList, template <typename> typename AllocType>
        struct allocator_descriptor_finder {
            template <typename T>
            using condition =
              stl::is_same<typename alloc::descriptors::allocator<T>::template type<stl::byte>,
                           AllocType<stl::byte>>;

            using type = istl::first_parameter<istl::filter_parameters_t<condition, AllocDescList>>;
        };



    } // namespace details

    template <AllocatorDescriptor AD>
    static constexpr bool is_resourceless = istl::parameter_count<alloc::descriptors::resources<AD>> == 0;

    template <typename AllocDescType, typename T>
    using allocator_extractor = typename details::allocator_extractor_impl<AllocDescType>::template type<T>;

    template <AllocatorDescriptorList AllocDescTypes>
    using resource_descriptor_extractor =
      typename details::resource_descriptor_extractor_impl<AllocDescTypes>::type;


    template <typename AllocDescTypes>
    using resource_extractor =
      typename istl::filter_parameters<istl::templated_negation<stl::is_void>::template type,
                                       typename details::resource_extractor_impl<AllocDescTypes>::type>::type;


    template <AllocatorDescriptorList AllocDescTypes>
    using allocator_flattener = typename details::allocator_flattener_impl<AllocDescTypes>::type;


    template <AllocatorDescriptorList AllocDescType>
    using alloc_res_pair_maker = typename details::alloc_res_pair_maker_impl<AllocDescType>::type;


    /**
     * Find the matching allocator descriptor in the allocator list based on the specified allocator type
     */
    template <AllocatorDescriptorList AllocDescList, template <typename> typename AllocType>
    using allocator_descriptor_finder =
      typename details::allocator_descriptor_finder<AllocDescList, AllocType>::type;

    /**
     * Find the resource descriptor with ResType as its storage_type
     */
    template <typename ResDescList, typename ResType>
    using resource_descriptor_finder_in_resource_list =
      typename details::resource_descriptor_finder<ResDescList, ResType>::type;

    template <AllocatorDescriptorList AllocDescList, Allocator AllocType, typename ResType>
    using resource_descriptor_finder = resource_descriptor_finder_in_resource_list<
      alloc::descriptors::resources<
        allocator_descriptor_finder<AllocDescList, stl::allocator_traits<AllocType>::template rebind_alloc>>,
      ResType>;

} // namespace webpp

#endif // WEBPP_ALLOCATOR_CONCEPTS_HPP
