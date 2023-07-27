// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

#include "../io/io_concepts.hpp"
#include "../logs/log_concepts.hpp"
#include "../memory/allocator_concepts.hpp"
#include "../memory/allocator_pack.hpp"
#include "../memory/object.hpp"
#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"

namespace webpp {

    /**
     * Included traits:
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
     *   - [ ] JSON types
     *   - [ ] Error Types
     *   - [ ] Unicode String Type
     *
     * Proposed types:
     *   - [ ] Encryption algorithms and their keys
     *   - [ ] Date and Time systems: stl::chrono types
     *   - [ ] String formatting types (fmt/printf/...)
     *   - [ ] Config file types
     *   - [ ] Cache types
     *   - [ ] Task Scheduling types
     *   - [ ] Image processing types
     *   - [ ] char_traits: character traits (removed)
     *         which should satisfy the CharTraits requirements specified in:
     *         https://en.cppreference.com/w/cpp/named_req/CharTraits
     *   - [ ] char_type: character type (removed)
     *
     * Rules:
     *   CharType:
     *     The `char_type` has to be an integral type. This char_type has nothing to do with the `string`,
     *     and `string_view`'s `value_type`s. Those strings might have a different character type which might
     *     even be not directly an integral type.
     *
     *   Allocators:
     *     Allocators are required to be in a pack and single-allocator-for-all is not usually the best
     *     solution for everywhere. This is the reason behind `TraitsPack` and `Traits` being two different
     *     things.
     *
     * Pack:
     *   A Pack is a series of types that potentially do the same thing and have the same APIs but they might
     *   perform better in specific situations. For example you can have an "ASCII String" type which is good
     *   for places where only ASCII characters are going to be handled and also have a "Unicode String" type
     *   that is required for working with Unicode strings. Even though the "Unicode String" type can handle
     *   the "ASCII String"'s job, it's probably going to have performance issues.
     *
     * Features:
     *   So in order to solve this problem, I came up with this "Packaging" the types and giving them
     *   "Features". Each type has a set of features that will help the other types in the project to choose
     *   the best type based on those features.
     *
     * Possible Features:
     *   Every package does have a different set of "Possible Features". For example String types require
     *   different type of features than Allocator types.
     *
     * Required Features:
     *   These are the features that is "required" while searching for the best feature in the list of
     *   available types in the pack.
     *
     * Optional Features:
     *   These are the features that are not required but if the type has it, it's going to get a better
     *   "rank" while searching.
     *
     * Feature Rank:
     *   While searching for the best type, they will be ranked to see which one has the best rank.
     *
     *
     * Types:
     *
     *   string<AllocatorType>:
     *     Usage: ASCII strings only
     *     The AllocatorType has nothing to do with "allocator_descriptors". Even though
     *     you probably should use the AllocatorType, but you are free not to use them in your
     *     type and we're okay with that. For example you might use QString without using any allocator; but
     *     you have to then use a wrapper for QString to make sure the constructors are a match to the
     *     std::basic_string's constructors.
     *
     *   string_view:
     *     Usage: ASCII strings only
     *     A string_view match for the `string` type above.
     *
     *   logger_type:
     *     The logging system.
     *
     *   allocator_descriptors:
     *     A Pack of allocators.
     *
     *   json:
     *     todo: implement this
     *
     *   error:
     *     The error system to use. Possible solutions:
     *       - Exceptions
     *       - Error Code solutions
     *     todo: find common syntax that can be used to implement all of them and write a concept for it
     *
     *   unicode_string:
     *   unicode_string_view:
     *     todo: implement these two
     */
    template <typename T>
    concept Traits = requires {
        requires AllocatorDescriptorList<typename T::allocator_descriptors>; // allocator pack
        requires Logger<typename T::logger_type>;                            // logger type
        // requires ThreadPool<typename T::thread_pool>;   // thread pool

        typename T::string_view;
        typename T::template string<typename alloc::allocator_pack<typename T::allocator_descriptors>::
                                      template general_allocator_type<typename T::string_view::value_type>>;

        // todo: add String<typename T::string_type>; without adding a circular dependency
        // todo: add StringView<typename T::string_view_type>; without adding a circular dependency

        // typename T::execution_context;
        // requires io::ExecutionContext<typename T::execution_context>;
    };


    /**
     * A traits enabled type is a type that supports everything that a traits type has to offer.
     * This will probably have a run-time cost to instantiate.
     */
    template <typename T>
    concept EnabledTraits = requires(stl::remove_cvref_t<T> t) {
        requires alloc::AllocatorPack<typename stl::remove_cvref_t<T>::allocator_pack_type>;
        requires Logger<typename stl::remove_cvref_t<T>::logger_type>;
        requires Traits<typename stl::remove_cvref_t<T>::traits_type>;
        t.logger;
        t.alloc_pack;
        // { t.io } -> io::IOScheduler;
        stl::remove_cvref_t<T>::is_resource_owner;
        t.get_traits();
    };


    template <typename TraitsType, typename T>
    concept EnabledTraitsOf =
      EnabledTraits<T> && requires { requires stl::same_as<typename T::traits_type, TraitsType>; };

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
        using allocator_descriptors = typename TT::allocator_descriptors;

        template <Traits TT>
        using allocator_pack_type = alloc::allocator_pack<typename TT::allocator_descriptors>;

        template <Traits TT, typename T = stl::byte>
        using local_allocator = typename allocator_pack_type<TT>::template local_allocator_type<T>;

        template <Traits TT, typename T = stl::byte>
        using general_allocator = typename allocator_pack_type<TT>::template general_allocator_type<T>;

        template <Traits TT>
        using string_view = typename TT::string_view;

        template <Traits TT>
        using char_type = istl::char_type_of_t<string_view<TT>>;

        template <Traits TT>
        using local_string_allocator = local_allocator<TT, char_type<TT>>;

        template <Traits TT>
        using general_string_allocator = general_allocator<TT, char_type<TT>>;

        template <Traits TT, Allocator AllocType>
        using string = typename TT::template string<AllocType>;

        template <Traits TT>
        using general_string = string<TT, general_string_allocator<TT>>;

        template <Traits TT>
        using local_string = string<TT, local_string_allocator<TT>>;

        template <Traits TT>
        using logger = typename TT::logger_type;

        // you can get the allocator type and stuff from here
        template <Traits TT, typename T, alloc::feature_pack FPack = alloc::general_features>
        using type_traits = alloc::alloc_finder<T, FPack, allocator_descriptors<TT>>;

        template <Traits TT, typename T, alloc::feature_pack FPack = alloc::general_features>
        using replace_allocators = typename type_traits<TT, T, FPack>::new_type;

        template <Traits TT, typename T>
        using generalify_allocators = replace_allocators<TT, T, alloc::general_features>;

        template <Traits TT, typename T>
        using localify_allocators = replace_allocators<TT, T, alloc::local_features>;


        template <Traits TT, typename T>
        using general_object = object::general<T, allocator_descriptors<TT>>;

    } // namespace traits

} // namespace webpp

#endif // WEBPP_TRAITS_H
