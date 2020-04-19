#ifndef CHARSET_H
#define CHARSET_H
/**
 * @file charset.h
 *
 * This module declares a charset class
 */

#include <algorithm>
#include <array>
#include <initializer_list>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace webpp {

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <typename CharT = char, std::size_t N = 1>
    class charset_t {
        static_assert(
            N > 0,
            "A charset with zero elements doesn't makes sense to construct.");

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
        CharT content[N] = {};
        size_t _size{0};

      public:
        ~charset_t() noexcept = default;
        charset_t(const charset_t&) = default;
        charset_t(charset_t&&) noexcept = default;
        charset_t& operator=(const charset_t&) = default;
        charset_t& operator=(charset_t&&) noexcept = default;

      public:
        /**
         * This is the default constructor.
         */
        constexpr charset_t() noexcept {}

        template <typename CCharT = CharT>
        constexpr charset_t(const CCharT (&input)[N]) noexcept {
            if constexpr (std::is_same_v<CCharT, CharT>) {
                // copy the string over to content
                for (size_t i{0}; i < N; ++i) {
                    content[i] = input[i];
                    if ((i == (N - 1)) && (input[i] == 0))
                        break;
                    _size++;
                }
            } else {
            }
        }

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */

        template <std::size_t... NN>
        constexpr charset_t(const charset_t<CharT, NN>&... csets) noexcept {
            auto csets_tupled = std::make_tuple(csets...);
            for_each_tuple(csets_tupled, [&, i = 0u](auto const& t) mutable {
                for (auto const& c : t.chars) {
                    if (!contains(c))
                        chars[i++] = c;
                }
            });
        }

        template <typename CCharT = CharT, size_t NN>
        constexpr explicit charset_t(
            std::array<CCharT, NN> const& _chars) noexcept
            : charset_t<CCharT>{_chars.data()} {}

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
        template <typename CCharT = CharT>
        [[nodiscard]] constexpr bool contains(CCharT c) const noexcept {
            for (auto it = begin<CCharT>(); it != end<CCharT>(); ++it) {
                if (*it == c)
                    return true;
            }
            return false;
        }

        /**
         * @brief checks if all the chars in the _cs is in the chars list or not
         * @param _cs
         * @return
         */
        template <typename CCharT = CharT>
        [[nodiscard]] constexpr bool
        contains(std::basic_string_view<CCharT> const& _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains<CCharT>(c))
                    return false;
            return true;
        }

        template <typename CCharT = CharT>
        [[nodiscard]] constexpr auto data() const noexcept {
            // TODO: you have to implement your own iterator
            return content;
        }

        template <typename CCharT = CharT>
        [[nodiscard]] constexpr std::size_t size() const noexcept {
            return _size * sizeof(CharT) / sizeof(CCharT);
        }

        template <typename StrCharT = CharT>
        [[nodiscard]] constexpr auto string_view() const noexcept {
            return std::basic_string_view<StrCharT>(data(), size());
        }

        template <typename StrCharT = CharT>
        [[nodiscard]] std::string string() const noexcept {
            return std::basic_string<StrCharT>(data(), size<StrCharT>());
        }
    };
    /**
     * Constructing a charset with chars
     * God C++ really needs C++20's concepts; WTF
     * @return charset_t
     */
    template <typename... Char,
              typename = typename std::enable_if<
                  (true && ... && std::is_same_v<Char, char>), void>::type>
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
        return charset_t<N>{static_cast<char>(first + I)...};
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
