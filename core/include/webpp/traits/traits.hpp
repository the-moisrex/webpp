// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

#include "../logs/log_concepts.hpp"
#include "../memory/allocator_concepts.hpp"
#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"

namespace webpp {

    /**
     * Thread pool class helps to implement a vector/list of threads and push
     * tasks into this thread pool.
     *
     * List of features I'd like to see in the future:
     * - [ ] Fewer run-time overhead features:
     *   - [ ] Register methods before using them multiple times.
     *     Use cases:
     *     - [ ] Buffering the requests
     *     - [ ] Processing user requests (even parsing the request)
     * - [ ] Priority scheduling:
     *   - [ ] defer
     *   - [ ] dispatch
     *   - [ ] post
     * - [ ] Run in a specific thread
     *   - [ ] By thread id
     *   - [ ] By thread index
     *   - [ ] In the last thread which that function was processed on
     * - [ ] Stop, Pause, Continue, Start methods
     * - [ ] Join threads
     * - [ ] Underlying thread class:
     *   - [ ] std::thread
     *   - [ ] std::jthread
     *   - [ ] boost::thread
     *   - [ ] POSIX
     * - [ ] Constexpr way to hash a function object into a known number in the thread pool
     */
    template <typename T>
    concept ThreadPool = requires(T tp, stl::true_type lambda) {
        tp.post(lambda);
        tp.defer(lambda); // todo: fix these 3; I don't think they have the correct args
        tp.dispatch(lambda);
    };


    /**
     * A Pack is a series of types that potentially do the same thing and have the same APIs but they might
     * perform better in specific situations. For example you can have an "ASCII String" type which is good
     * for places where only ASCII characters are going to be handled and also have a "Unicode String" type
     * that is required for working with Unicode strings. Even though the "Unicode String" type can handle
     * the "ASCII String"'s job, it's probably going to have performance issues.
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
     */
    template <typename T>
    concept Pack = requires {

    };


    /**
     * TraitsPacks are types that the final user is going to pass to the entry-point templated structures.
     * They are designed to contain a `Pack` of types with different options to choose from.
     * These packs of types will help this project to "choose" between those types and create a new `traits`
     * type that contains only one of those types.
     *
     * This process of choosing is going to be based on the `features` that each of those types in the pack
     * have.
     *
     * For example: Allocators
     *   Allocators are a good example of having a pack of them instead of just one single type that does
     *   it all.
     */
    template <typename T>
    concept TraitsPack = requires {
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
     * Included traits:
     *   - [ ] char_type: character type
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
     *   - [ ] Thread Pool
     *
     * Proposed types:
     *   - [ ] Encryption algorithms and their keys
     *   - [ ] Date and Time systems: stl::chrono types
     *   - [ ] JSON types
     *   - [ ] Error Types
     *   - [ ] String formatting types (fmt/printf/...)
     *   - [ ] Config file types
     *   - [ ] Cache types
     *   - [ ] Task Scheduling types
     *   - [ ] Image processing types
     *   - [ ] char_traits: character traits
     *         which should satisfy the CharTraits requirements specified in:
     *         https://en.cppreference.com/w/cpp/named_req/CharTraits
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
     */
    template <typename T>
    concept Traits = requires {
        requires TraitsPack<typename T::traits_pack_type>; // parent traits type
        requires istl::CharType<typename T::char_type>;    // character type
        requires Allocator<typename T::allocator_type>;    // selected allocator type
        requires Logger<typename T::logger_type>;          // selected logger type
        requires ThreadPool<typename T::thread_pool>;      // the selected thread pool

        typename T::template string<typename T::char_type,
                                    typename T::template allocator<typename T::char_type>>;
        typename T::template string_view<typename T::char_type>;
    };


    template <typename T>
    concept TraitsPackEnabled = requires (T t) {

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
        t.template local_alloc<char>();
        t.template general_alloc<char>();
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
        using allocator = typename TT::template allocator<T>;

        template <Traits TT>
        using string_view = typename TT::template string_view<char_type<TT>>;

        template <Traits TT>
        using string = typename TT::template string<char_type<TT>, allocator<TT, char_type<TT>>>;

        template <Traits TT>
        using logger = typename TT::logger_type;

        /// traits packs ///

        template <TraitsPack TT, typename T>
        using local_allocator = typename TT::alloc_pack::template local<T>;

        template <TraitsPack TT>
        using local_char_allocator = local_allocator<TT, char_type<TT>>;

        template <TraitsPack TT, typename T>
        using general_allocator = typename TT::alloc_pack::template general<T>;

        template <TraitsPack TT>
        using general_char_allocator = general_allocator<TT, char_type<TT>>;

        template <TraitsPack TT, typename T>
        using alloc_list = typename TT::alloc_pack::template list<T>;

        template <TraitsPack TT>
        using char_alloc_list = alloc_list<TT, char_type<TT>>;

        template <Traits TT>
        using general_string = typename TT::template string<char_type<TT>, general_char_allocator<TT>>;

        template <Traits TT>
        using local_string = typename TT::template string<char_type<TT>, local_char_allocator<TT>>;

    } // namespace traits

} // namespace webpp

#endif // WEBPP_TRAITS_H
