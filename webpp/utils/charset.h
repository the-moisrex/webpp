#ifndef CHARSET_H
#define CHARSET_H
/**
 * @file cahrset.h
 *
 * This module declares a charset class
 *
 * © 2018 by Richard Walters (in his own project)
 * © 2019 by Mohammad Bahoosh (made it constexpr)
 */

#include "../std/string_view.h"
#include <algorithm>
#include <array>
#include <initializer_list>
#include <tuple>
#include <type_traits>
#include <utility>

namespace webpp {

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <std::size_t N = 1>
    class charset_t {
        static_assert(
            N > 0,
            "A charset with zero elements doesn't makes sense to construct.");

        /*
            struct charset_t_impl {

                template <std::size_t NN, std::size_t ...I>
                constexpr auto
           to_char_initializer_list(std::index_sequence<I...>, charset_t<NN>
           const &cset) noexcept { return { cset.chars[I]... }; // turning it
           into a sequence of chars
                }

                template <template <std::size_t N> class ...CharsetType,
           std::size_t ...I> constexpr auto impl_l1(std::index_sequence<I...>,
           CharsetType ...csets) noexcept {
                    //constexpr auto size_of_list = (0 + ... + N);
                    constexpr auto char_count = (0 + ... + csets[I].size());
                    constexpr auto l = (std::array<char, char_count>() += ... +=
           csets[I].array);
                    //constexpr auto list_of_list_of_chars = {
           to_char_initializer_list(std::make_index_sequence<csets[I].size()>{},
           csets[I])... }; return l;
                }

                template <std::size_t ...N>
                constexpr auto operator()(charset_t<N> const& ...csets) noexcept
           { return impl_l1(std::make_index_sequence<N>{}, csets...);
                }

            };
        */
        /*
        struct get_size {
            template <template <std::size_t> class ...CharsetType, std::size_t
        ...D, std::size_t ...I> constexpr auto impl(std::index_sequence<I...>,
        CharsetType<D> const &...csets) noexcept { return (0 + ... +
        csets[I].size());
            }

            template <std::size_t ...CharCount>
            constexpr auto operator()(charset_t<CharCount> const &...csets)
        noexcept { return impl(std::make_index_sequence<(0 + ... +
        CharCount)>(), csets...);
            }
        };
    */

        template <typename Tpl, typename Callable, std::size_t... I>
        constexpr void do_this_for_that(std::index_sequence<I...>,
                                        Tpl const& tpl,
                                        Callable callback) noexcept {
            (callback(std::get<I>(tpl)), ...);
        }

        template <typename Tpl, typename Callable>
        constexpr void for_each_tuple(Tpl const& tpl,
                                      Callable callback) noexcept {
            constexpr auto tpl_size = std::tuple_size<Tpl>::value;
            do_this_for_that(std::make_index_sequence<tpl_size>(), tpl,
                             callback);
        }

      public:
        std::array<char, N> chars;

        // Lifecycle management
      public:
        ~charset_t() noexcept = default;
        charset_t(const charset_t&) = default;
        charset_t(charset_t&&) noexcept = default;
        charset_t& operator=(const charset_t&) = default;
        charset_t& operator=(charset_t&&) noexcept = default;

        // Methods
      public:
        /**
         * This is the default constructor.
         */
        constexpr charset_t() noexcept : chars{} {}

        /**
         * This constructs a character set that contains
         * just the given character.
         *
         * @param[in] c
         *     This is the only character to put in the set.
         */
        constexpr explicit charset_t(char c) noexcept : chars{c} {}

        template <typename... T, typename = char>
        constexpr charset_t(T... t) noexcept : chars{t...} {}

        /*
        explicit charset_t(std::initializer_list<char> cset) noexcept {
            std::copy(cset.begin(), cset.end(), chars.begin());
        }
         */

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */

        template <std::size_t... NN>
        constexpr charset_t(const charset_t<NN>&... csets) noexcept : chars{} {
            // static_assert(((0 + ... + NN) > N), "Sum of csets is greater than
            // charset's container");
            auto csets_tupled = std::make_tuple(csets...);
            for_each_tuple(csets_tupled, [&, i = 0u](auto const& t) mutable {
                for (auto const& c : t.chars) {
                    if (!contains(c))
                        chars[i++] = c;
                }
            });
            // for (std::size_t i = 0; i < csets_count; i++) {
            //     auto cset = std::get<i>(csets_tupled);
            //   for (auto const &c : cset.chars) {
            //     if (!contains(c)) chars[i++] = c;
            //   }
            // }

            // filling the rest with null char
            // for (; i < N; i++) chars[i] = '\0';
        }

        constexpr explicit charset_t(decltype(chars) const& _chars) noexcept
            : chars(_chars) {}
        constexpr explicit charset_t(decltype(chars)&& _chars) noexcept
            : chars(std::move(_chars)) {}

        /**
         * This method checks to see if the given character
         * is in the character set.
         *
         * @param[in] c
         *     This is the character to check.
         *
         * @return
         *     An indication of whether or not the given character
         *     is in the character set is returned.
         */
        constexpr bool contains(char c) const noexcept {
            for (auto const& cc : chars)
                if (cc == c)
                    return true;
            return false;
        }

        /**
         * @brief checks if all the chars in the _cs is in the chars list or not
         * @param _cs
         * @return
         */
        constexpr bool contains(std::string_view _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains(c))
                    return false;
            return true;
        }

        constexpr auto size() const noexcept { return chars.size(); }

        constexpr std::string_view string_view() const noexcept {
            return std::string_view(chars.data(), N);
        }
    };
    /*
    template <typename T>
    constexpr auto charset(T const &cset) noexcept {
        return charset_t<cset.size()>(cset);
    }
    */

    //    auto charset(std::initializer_list<char> const& cset) noexcept {
    //        return charset_t<cset.size()>(cset);
    //    }

    template <typename... Char, typename = char>
    constexpr auto charset(Char... chars) noexcept {
        return charset_t<sizeof...(chars)>{chars...};
    }

    template <std::size_t... N>
    constexpr auto charset(charset_t<N>... csets) noexcept {
        return charset_t<(0 + ... + N)>{csets...};
    }

    template <std::size_t N, std::size_t... I>
    constexpr auto charset_impl(charset_t<N> const& cset,
                                std::index_sequence<I...>) noexcept {
        return charset<N>(
            {cset.chars[I]...}); // turning it into a sequence of chars
    }

    template <std::size_t N, typename Indeces = std::make_index_sequence<N>>
    constexpr auto charset(charset_t<N> const& cset) noexcept {
        return charset_impl(cset, Indeces{});
    }

    template <std::size_t N, char... I>
    constexpr auto charset(char first,
                           std::integer_sequence<char, I...>) noexcept {
        return charset_t<N>{(first + I)...};
    }

    /**
     * This constructs a character set that contains all the
     * characters between the given "first" and "last"
     * characters, inclusive.
     *
     * @param[in] first
     *     This is the first of the range of characters
     *     to put in the set.
     *
     * @param[in] last
     *     This is the last of the range of characters
     *     to put in the set.
     */
    template <char First, char Last>
    constexpr auto charset() noexcept {
        constexpr auto the_size = static_cast<std::size_t>(Last) -
                                  static_cast<std::size_t>(First) + 1;
        return charset<the_size>(First,
                                 std::make_integer_sequence<char, the_size>{});
    }

    // TODO: add non-constexpr (or constexpr if you can) charset(first, last) as
    // well

    /*
    struct charset_t_impl {

        template <std::size_t NN, std::size_t ...I>
        constexpr auto to_char_initializer_list(std::index_sequence<I...>,
    charset_t<NN> const &cset) noexcept { return { cset.chars[I]... }; //
    turning it into a sequence of chars
        }

        template <template <std::size_t N> class ...CharsetType, std::size_t
    ...I> constexpr auto impl_l1(std::index_sequence<I...>, CharsetType
    ...csets) noexcept {
            //constexpr auto size_of_list = (0 + ... + N);
            constexpr auto char_count = (0 + ... + csets[I].size());
            constexpr auto l = (std::array<char, char_count>() += ... +=
    csets[I].array);
            //constexpr auto list_of_list_of_chars = {
    to_char_initializer_list(std::make_index_sequence<csets[I].size()>{},
    csets[I])... }; return l;
        }

        template <std::size_t ...N>
        constexpr auto operator()(charset_t<N> const& ...csets) noexcept {
            return impl_l1(std::make_index_sequence<N>{}, csets...);
        }
    };
    */

    /*
    template <std::size_t ...N, std::size_t ...I>
    constexpr auto charset_(std::index_sequence<I...>, charset_t<N> const
    &...csets) noexcept { return charset<(0 + ... + N)>({ csets[I]... }); //
    turning it into a sequence of chars
    }

    template <std::size_t ...N>
    constexpr auto charset(charset_t<N> const &...cset) noexcept {
        return charset_<N>(std::make_index_sequence<N>{}, cset...);
    }
    */

    constexpr auto LOWER_ALPHA = charset<'a', 'z'>();
    constexpr auto UPPER_ALPHA = charset<'A', 'Z'>();

    /**
     * This is the character set containing just the alphabetic characters
     * from the ASCII character set.
     */
    constexpr auto ALPHA = charset(LOWER_ALPHA, UPPER_ALPHA);

    /**
     * This is the character set containing just numbers.
     */
    constexpr auto DIGIT = charset<'0', '9'>();

    /**
     * This is the character set containing just the characters allowed
     * in a hexadecimal digit.
     */
    constexpr auto HEXDIG =
        charset(DIGIT, charset<'A', 'F'>(), charset<'a', 'f'>());

} // namespace webpp
#endif // CHARSET_H
