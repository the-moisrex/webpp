// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

#include <string>
#include <string_view>

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
     *   - [ ] Date and Time systems: std::chrono types
     *
     * Any other custom traits must include the traits above so it can be used
     * through out the templated classes inside the webpp project.
     *
     * The std_traits is just the default standard traits defined in the std
     * library.
     */
    template <typename CharT>
    struct basic_std_traits {
        using char_type   = CharT;
        using char_traits = std::char_traits<char_type>;

        template <typename Type>
        using allocator = std::allocator<Type>;

        using string_view_type = std::basic_string_view<char_type, char_traits>;
        using string_type =
          std::basic_string<char_type, char_traits, allocator<char_type>>;

        using stringstream_type =
          std::basic_stringstream<char_type, char_traits, allocator<char_type>>;
    };

    using std_traits = basic_std_traits<char>;


    /**
     * A series of simple tests to check if the specified template parameter
     * Traits is a valid traits or not.
     *
     * This set of tests are not yet fool proof, but they hold the fort until
     * later.
     *
     * todo: add more test and make them more clever than this
     */
    template <typename Traits>
    using is_traits = std::conjunction<
      std::is_integral<typename Traits::char_type>,
      std::is_integral<typename Traits::char_traits::char_type>>;

    template <typename Traits>
    constexpr auto is_traits_v = is_traits<Traits>::value;


    /**
     * Automatically choose a string type based on mutability requested
     */
    template <typename Traits, bool Mutable>
    using auto_string_type =
      ::std::conditional_t<Mutable, typename Traits::string_type,
                           typename Traits::string_view_type>;


} // namespace webpp

#endif // WEBPP_TRAITS_H
