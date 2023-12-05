// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_TRAITS_HPP
#define WEBPP_TRAITS_TRAITS_HPP

#include "../logs/log_concepts.hpp"
#include "../memory/allocator_concepts.hpp"
#include "../std/concepts.hpp"

namespace webpp {

    // template <typename T>
    // concept ExecutionContextPack = requires {
    //     requires IOExecutionContext<typename T::io_execution_context>;
    //     requires EventLoopExecutionContext<typename T::event_loop_execution_context>;
    //     requires TaskChainExecutionContext<typename T::task_chain_execution_context>;
    //     requires TimerExecutionContext<typename T::timer_chain_execution_context>;
    // };


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
     *
     * Proposed types:
     *   - [ ] JSON types
     *   - [ ] Error Types
     *   - [ ] Unicode String Type
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
     */
    template <typename T>
    concept Traits = requires {
        requires AllocatorDescriptor<typename T::general_allocator_descriptor>;   // general allocator
        requires AllocatorDescriptor<typename T::monotonic_allocator_descriptor>; // monotonic allocator
        requires Logger<typename T::logger_type>;                                 // logger type

        typename T::string_view;
        typename T::template string<typename T::general_allocator_descriptor::template allocator_type<
          typename T::string_view::value_type>>;

        // todo: add String<typename T::string_type>; without adding a circular dependency
        // todo: add StringView<typename T::string_view_type>; without adding a circular dependency

        // typename T::execution_context;
        // requires io::ExecutionContext<typename T::execution_context>;
    };


    /**
     * Check if the specified type T holds an allocator
     * Almost the same thing as EnabledTraits to be honest
     */
    template <typename T>
    concept AllocatorHolder = requires(stl::remove_cvref_t<T> holder) {
        typename stl::remove_cvref_t<T>::template general_allocator_type<char>;
        typename stl::remove_cvref_t<T>::template monotonic_allocator_type<char>;
        holder.template general_allocator<char>();
        // holder.template monotonic_allocator<char>();
    };

    /**
     * A traits enabled type is a type that supports everything that a traits type has to offer.
     * This will probably have a run-time cost to instantiate.
     */
    template <typename T>
    concept EnabledTraits = AllocatorHolder<T> && requires(stl::remove_cvref_t<T> etraits) {
        requires Logger<typename stl::remove_cvref_t<T>::logger_type>;
        requires Traits<typename stl::remove_cvref_t<T>::traits_type>;
        etraits.logger;
        stl::remove_cvref_t<T>::is_resource_owner;
        etraits.get_traits();
        // { t.io } -> io::IOScheduler;
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

        namespace details {

            template <Traits TT, typename T>
            struct monotonic_allocator_extractor {
                using type = void;
            };

            template <Traits TT, typename T>
                requires(!stl::is_void_v<typename TT::monotonic_allocator_descriptor>)
            struct monotonic_allocator_extractor<TT, T> {
                using type = typename TT::monotonic_allocator_descriptor::template allocator_type<T>;
            };

        } // namespace details

        template <Traits TT, typename T = stl::byte>
        using local_allocator = typename details::monotonic_allocator_extractor<TT, T>::type;

        template <Traits TT, typename T = stl::byte>
        using general_allocator = typename TT::general_allocator_descriptor::template allocator_type<T>;

        template <Traits TT>
        using string_view = typename TT::string_view;

        template <Traits TT>
        using char_type = typename string_view<TT>::value_type; // we could use istl::char_type_of_t

        template <Traits TT>
        using local_string_allocator = local_allocator<TT, char_type<TT>>;

        template <Traits TT>
        using general_string_allocator = general_allocator<TT, char_type<TT>>;

        template <Traits TT, Allocator AllocType>
        using string = typename TT::template string<
          typename stl::allocator_traits<AllocType>::template rebind_alloc<char_type<TT>>>;

        template <Traits TT>
        using general_string = string<TT, general_string_allocator<TT>>;

        template <Traits TT>
        using local_string = string<TT, local_string_allocator<TT>>;

        template <Traits TT>
        using logger = typename TT::logger_type;

        template <typename TT, typename T>
        concept has_alloc_for =
          (Traits<TT> && requires {
              typename T::allocator_type;
              requires stl::convertible_to<general_allocator<TT>, typename T::allocator_type>;
          }) || (EnabledTraits<TT> && requires {
              typename T::allocator_type;
              requires stl::convertible_to<general_allocator<typename TT::traits_type>,
                                           typename T::allocator_type>;
          });

    } // namespace traits

} // namespace webpp

#endif // WEBPP_TRAITS_TRAITS_HPP
