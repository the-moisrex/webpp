#ifndef CHARSET_H
#define CHARSET_H

#include <algorithm>
#include <array>
#include <cassert>
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
    class charset_t : public std::array<CharT, N> {
        static_assert(
          N > 0,
          "A charset with zero elements doesn't makes sense to construct.");

        template <typename Tpl, typename Callable, std::size_t... I>
        constexpr void do_this_for_that(std::index_sequence<I...>,
                                        Tpl const& tpl,
                                        Callable   callback) noexcept {
            (callback(std::get<I>(tpl)), ...);
        }

        template <typename Tpl, typename Callable>
        constexpr void for_each_tuple(Tpl const& tpl,
                                      Callable   callback) noexcept {
            constexpr auto tpl_size = std::tuple_size<Tpl>::value;
            do_this_for_that(std::make_index_sequence<tpl_size>(), tpl,
                             callback);
        }

        using super = std::array<CharT, N>;

      public:
        template <typename... T>
        constexpr charset_t(T&&... t) noexcept : super{std::forward<T>(t)...} {
        }

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        template <std::size_t N1, std::size_t N2, std::size_t... NN>
        constexpr explicit charset_t(
          charset_t<CharT, N1> const& set1, charset_t<CharT, N2> const& set2,
          charset_t<CharT, NN> const&... c_sets) noexcept {
            auto write = [this, i = 0u](auto const& set) mutable {
                for (auto c : set)
                    super::operator[](i++) = c;
            };
            write(set1);
            write(set2);
            (write(c_sets), ...);
        }


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
        [[nodiscard]] constexpr bool contains(CharT c) const noexcept {
            for (auto cc : *this) {
                if (cc == c)
                    return true;
            }
            return false;
        }

        /**
         * @brief checks if all the chars in the _cs is in the chars list or not
         * @param _cs
         * @return
         */
        [[nodiscard]] constexpr bool
        contains(std::basic_string_view<CharT> const& _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains(c))
                    return false;
            return true;
        }

        [[nodiscard]] constexpr auto string_view() const noexcept {
            return std::basic_string_view<CharT>(this->data(), this->size());
        }

        [[nodiscard]] std::string string() const noexcept {
            return std::basic_string<CharT>(this->data(), this->size());
        }
    };

    template <typename CharT = char, std::size_t N1, std::size_t N2,
              std::size_t... N>
    constexpr auto charset(charset_t<CharT, N1> const& set1,
                           charset_t<CharT, N2> const& set2,
                           charset_t<CharT, N> const&... csets) noexcept {
        return charset_t<CharT, N1 + N2 + (0 + ... + N)>{set1, set2, csets...};
    }

    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename _Tp, typename... _Up>
    charset_t(_Tp, _Up...)
      -> charset_t<std::enable_if_t<(std::is_same_v<_Tp, _Up> && ...), _Tp>,
                   1 + sizeof...(_Up)>;



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
    template <typename CharT = char, CharT First, CharT Last>
    constexpr auto charset() noexcept {
        constexpr auto the_size =
          static_cast<std::size_t>(Last) - static_cast<std::size_t>(First) + 1;
        charset_t<CharT, the_size> data;
        for (auto it = First; it != Last + 1; ++it)
            data[it - First] = it;
        return data;
    }

    // TODO: add non-constexpr (or constexpr if you can) charset(first, last) as well


    constexpr auto LOWER_ALPHA = charset<char, 'a', 'z'>();
    constexpr auto UPPER_ALPHA = charset<char, 'A', 'Z'>();

    /**
     * This is the character set containing just the alphabetic characters
     * from the ASCII character set.
     */
    constexpr auto ALPHA = charset<char>(LOWER_ALPHA, UPPER_ALPHA);

    /**
     * This is the character set containing just numbers.
     */
    constexpr auto DIGIT = charset<char, '0', '9'>();

    /**
     * This is the character set containing just the characters allowed
     * in a hexadecimal digit.
     */
    constexpr auto HEXDIG =
      charset(DIGIT, charset<char, 'A', 'F'>(), charset<char, 'a', 'f'>());

} // namespace webpp
#endif // CHARSET_H
