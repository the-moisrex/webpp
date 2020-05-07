// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

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
     *   - [ ] Date and Time systems: std::chrono types
     *
     * Any other custom traits must include the traits above so it can be used
     * through out the templated classes inside the webpp project.
     *
     * The std_traits is just the default standard traits defined in the std
     * library.
     */

    template <typename TraitsT>
    concept Traits = requires(TraitsT t) {
        // char_type:
        typename TraitsT::char_type;
        std::is_integral_v<TraitsT::char_type>;

        // char_traits:
        typename TraitsT::char_traits;
        stl::CharTraits<typename TraitsT::char_traits>;

        // allocator:
        typename TraitsT::allocator;
        // todo: add Allocator<typename TraitsT::allocator>;

        typename TraitsT::string_type;
        // todo: add BasicString<typename TraitsT::string_type>;

        typename TraitsT::string_view_type;
        // todo: add BasicStringView<typename TraitsT::string_view_type>;
    };




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
    constexpr bool is_traits_v = is_traits<Traits>::value;


    /**
     * Automatically choose a string type based on mutability requested
     */
    template <typename Traits, bool Mutable>
    using auto_string_type =
      ::std::conditional_t<Mutable, typename Traits::string_type,
                           typename Traits::string_view_type>;


    /**
     * todo: Use this, or remove this
     */
    template <typename FromTraits, typename ToTraits>
    struct traits_convert {
        using ft = FromTraits;
        using tt = ToTraits;

        using ft_char_type = typename ft::char_type;
        using tt_char_type = typename tt::char_type;

        using ft_string_view_type = typename ft::string_view_type;
        using tt_string_view_type = typename tt::string_view_type;

        using ft_string_type = typename ft::string_type;
        using tt_string_type = typename tt::string_type;

        /**
         * Converting the char_type
         */
        static constexpr auto to_char_type(ft_char_type ch) noexcept {
            return static_cast<tt_char_type>(ch);
        }

        //        static constexpr auto to_string_view(ft_string_view_type const
        //        &sv) noexcept {
        //            // todo: very bad thing:
        //            return tt_string_view_type{sv.data(), sv.size()};
        //        }
        //
        static constexpr auto to_string(ft_string_type const& str) noexcept {
            tt_string_type res;
            res.resize(str.size() + 1);
            auto it = res.begin();
            for (auto c : str)
                *it++ = to_char_type(c);
            *it = to_char_type('\0');
            return res;
        }
    };

} // namespace webpp

#endif // WEBPP_TRAITS_H
