// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_CONCEPTS_HPP
#define WEBPP_ALLOCATOR_CONCEPTS_HPP

#include "../std/tuple.hpp"

#include <memory>

namespace webpp {


    // Name requirement: https://en.cppreference.com/w/cpp/named_req/Allocator
    template <typename A>
    concept Allocator = requires {
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
            {a.allocate(n)};
            {*p};
            {*cp};
            // {p->m};
            // {cp->m};
            {static_cast<typename stl::allocator_traits<A>::pointer>(vp)};
            {static_cast<typename stl::allocator_traits<A>::const_pointer>(cvp)};
            {stl::pointer_traits<typename stl::allocator_traits<A>::pointer>::pointer_to(r)};
            {a == a1};
            {a != a1};
            {A(a)};
            // {A a1 = a};
            // A a(b)};
            {A(stl::move(a))};
            // {A a1 = stl::move(a)};
            // {A a(stl::move(b))};
        };
    };

    template <typename T, typename Alloc>
    concept AllocatorOf = Allocator<Alloc>&& stl::same_as<typename Alloc::value_type, T>;

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
        typename I::type; // the resource type
        {I::features};    // the resource features of type alloc::feature_pack
    };

    // one single allocator descriptor which describes an allocator and its features and its resources
    template <typename D>
    concept AllocatorDescriptor = requires {
        typename D::template type<char>; // get the allocator itself
        typename D::resources;           // resource types (a list of MemoryResource)
        {D::features};                   // parent features of type alloc::feature_pack
    };

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


    // todo: types to add:
    //       1. make: make<string>(alloc)
    //       2. buffer: stack, arena, ram
    //       3. allocator_pack: a pack of allocator's objects
    //       4. resource_pack: a pack of resources for one type of allocator


    template <typename... AllocatorDescriptorsTypes>
    using type_list = stl::tuple<AllocatorDescriptorsTypes...>;





    /**
     * Extract the allocators (flatten the allocator descriptor list)
     */
    template <AllocatorDescriptorList AllocDescTypes>
    struct allocator_extractor;

    template <AllocatorDescriptor... AllocDescType>
    struct allocator_extractor<type_list<AllocDescType...>> {

        template <typename T>
        using type = type_list<typename AllocDescType::template type<T>...>;
    };




    /**
     * Extracts all of the "resources descriptors" from the "allocator descriptor list"
     * the type: tuple<ResourceDescriptor, ...>
     */
    template <AllocatorDescriptorList AllocDescTypes>
    struct resource_descriptor_extractor;

    template <template <typename...> typename TupleType, AllocatorDescriptor... AllocDescType>
    struct resource_descriptor_extractor<TupleType<AllocDescType...>>
      : public istl::merge_parameters<typename AllocDescType::resources...> {};





    /**
     * Extract all of the "resources" from an "allocator descriptor list".
     * the type: type_list<Resource>
     */
    template <typename AllocDescTypes>
    struct resource_extractor;

    template <template <typename...> typename TupleType, AllocatorDescriptor... AllocDescType>
    struct resource_extractor<TupleType<AllocDescType...>>
      : public istl::merge_parameters<
          typename resource_extractor<typename AllocDescType::resources>::type...> {};

    template <template <typename...> typename TupleType, ResourceDescriptor... ResDescType>
    struct resource_extractor<TupleType<ResDescType...>> {
        using type = TupleType<typename ResDescType::type...>;
    };





    template <AllocatorDescriptorList AllocDescTypes>
    struct allocator_flattener;

    template <AllocatorDescriptor... AllocDescType>
    struct allocator_flattener<type_list<AllocDescType...>> {

        template <typename T>
        struct type : public AllocDescType::template type<T>... {};
    };




    /**
     * Extract "memory resource descriptors" from an allocator descriptor
     * in form of std::pair<AllocatorDescriptor, ResourceDescriptor>
     */
    template <AllocatorDescriptorList AllocDescType, typename TheSame = AllocDescType>
    struct alloc_res_pair_maker;

    template <AllocatorDescriptor... AllocDescType>
    struct alloc_res_pair_maker<type_list<AllocDescType...>>
      : public istl::merge_parameters<typename alloc_res_pair_maker<AllocDescType>::type...>::type {};

    template <AllocatorDescriptor AllocDescType>
    struct alloc_res_pair_maker<type_list<AllocDescType>> {

        template <typename... T>
        struct pair_maker;

        template <ResourceDescriptor... ResDesc>
        struct pair_maker<type_list<ResDesc...>> {
            using type = type_list<std::pair<AllocDescType, ResDesc>...>;
        };

        using type = typename pair_maker<typename AllocDescType::resources>::type;
    };


} // namespace webpp

#endif // WEBPP_ALLOCATOR_CONCEPTS_HPP
