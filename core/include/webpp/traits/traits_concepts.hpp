// Created by moisrex on 5/9/20.

#ifndef WEBPP_TRAITS_CONCEPTS_H
#define WEBPP_TRAITS_CONCEPTS_H

#include "../logs/log_concepts.hpp"
#include "../memory/allocator_concepts.hpp"
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
        requires istl::CharTraits<typename T::char_traits>;

        // allocator pack
        requires AllocatorPack<typename T::allocator_pack>;

        // typename T::template allocator<typename T::char_traits>;
        typename T::string_type;
        typename T::string_view_type; // todo: do we need this here? should we remove this?
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
        { t.logger } -> Logger;
        t.get_allocator();
    };

} // namespace webpp

#endif // WEBPP_TRAITS_CONCEPTS_H
