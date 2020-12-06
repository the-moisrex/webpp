// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

#include "../logs/log_concepts.hpp"
#include "../memory/allocator_concepts.hpp"
#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"

namespace webpp {


    /**
     * This struct will define any traits related or required by others so the
     * final user can use more customized versions of the classes provided by
     * the webpp project. Included traits:
     *   - [ ] char_type: character type
     *   - [ ] char_traits: character traits
     *         which should satisfy the CharTraits requirements specified in:
     *         https://en.cppreference.com/w/cpp/named_req/CharTraits
     *   - [ ] allocator: allocator type
     *         which should satisfy the Allocator requirements specified in:
     *         https://en.cppreference.com/w/cpp/named_req/Allocator
     *   - [ ] string_view_type = string view type
     *   - [ ] string_type = string type
     *         which should satisfy AllocatorAwareContainer, SequenceContainer
     *         and ContiguousContainer specified in:
     *         https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer
     *         https://en.cppreference.com/w/cpp/named_req/SequenceContainer
     *         https://en.cppreference.com/w/cpp/named_req/ContiguousContainer
     *
     * Traits that might be added in the future:
     *   - [ ] Encryption algorithms and their keys
     *   - [ ] Interfaces
     *   - [ ] Date and Time systems: stl::chrono types
     *
     * Any other custom traits must include the traits above so it can be used
     * through out the templated classes inside the webpp project.
     *
     * The std_traits is just the default standard traits defined in the std
     * library.
     */
    template <typename T>
    concept Traits = requires {
        // char type
        requires istl::CharType<typename T::char_type>;

        // char traits:
        // requires istl::CharTraits<typename T::char_traits>;

        // allocator pack
        requires AllocatorPack<typename T::alloc_pack>;

        // typename T::template allocator<typename T::char_traits>;
        typename T::template string<typename T::char_type,
                                    typename T::alloc_pack::template local<typename T::char_type>>;
        typename T::template string_view<typename T::char_type>;
        typename T::logger_type;
        // todo: add String<typename T::string_type>; without adding a circular dependency
        // todo: add StringView<typename T::string_view_type>; without adding a circular dependency
    };


    /**
     * A traits enabled type is a type that supports everything that a traits type has to offer.
     * This will probably have a run-time cost to instantiate.
     *
     * Two most used are:
     *   - allocators
     *   - loggers
     */
    template <typename T>
    concept EnabledTraits = requires(T t) {
        requires Logger<typename T::logger_type>;
        requires Traits<typename T::traits_type>;
        { t.logger }
        ->Logger;
        t.get_allocator();
    };


    template <typename From, typename To>
    using to_alloc = typename stl::allocator_traits<From>::template rebind_alloc<To>;

    /**
     * Middle man type aliases.
     *   Seriously pro tip in C++20 concepts:
     *   Do NOT use your instances of your concepts (the types that match your concepts) directly if they're
     *   going to be all over the place. You WILL change the concept at some point. Use middle-man type
     *   aliases to extract information out of them.
     *     - from: https://twitter.com/the_moisrex/status/1335540447566049282?s=20
     */
    namespace traits {

        template <Traits TT>
        using char_type = typename TT::char_type;

        template <Traits TT, typename T>
        using local_allocator = typename TT::alloc_pack::template local<T>;

        template <Traits TT>
        using local_char_allocator = local_allocator<TT, char_type<TT>>;

        template <Traits TT, typename T>
        using general_allocator = typename TT::alloc_pack::template general<T>;

        template <Traits TT>
        using general_char_allocator = general_allocator<TT, char_type<TT>>;

        template <Traits TT>
        using alloc_list = typename TT::alloc_pack::list;

        template <Traits TT>
        using string = typename TT::template string<char_type<TT>, general_char_allocator<TT>>;

        template <Traits TT>
        using local_string = typename TT::template string<char_type<TT>, local_char_allocator<TT>>;

        template <Traits TT>
        using string_view = typename TT::template string_view<char_type<TT>>;

        template <Traits TT>
        using logger = typename TT::logger_type;

    } // namespace traits

} // namespace webpp

#endif // WEBPP_TRAITS_H
