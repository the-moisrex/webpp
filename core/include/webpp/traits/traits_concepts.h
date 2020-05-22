// Created by moisrex on 5/9/20.

#ifndef WEBPP_TRAITS_CONCEPTS_H
#define WEBPP_TRAITS_CONCEPTS_H

#include "../std/concepts.h"

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
     *    - [ ] stringstream_type: string stream type same thing as:
     *         https://en.cppreference.com/w/cpp/io/basic_stringstream
     *
     * Traits that might be added in the future:
     *   - [ ] Encryption algorithms and their keys
     *   - [ ] Interfaces
     *   - [ ] Date and Time systems: ::std::chrono types
     *
     * Any other custom traits must include the traits above so it can be used
     * through out the templated classes inside the webpp project.
     *
     * The std_traits is just the default standard traits defined in the std
     * library.
     */

    template <typename T>
    concept Traits =
      // char_type:
      ::std::is_integral_v<typename T::char_type>&&

        // char_traits:
        stl::CharTraits<typename T::char_traits>&&

      requires {
        typename T::template allocator<typename T::char_traits>;
        typename T::string_type;
        typename T::string_view_type;
        // todo: add Allocator<typename T::allocator>;
        // todo: add BasicString<typename T::string_type>;
        // todo: add BasicStringView<typename T::string_view_type>;
    };
} // namespace webpp

#endif // WEBPP_TRAITS_CONCEPTS_H
