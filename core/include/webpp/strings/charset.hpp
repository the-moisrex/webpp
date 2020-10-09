#ifndef WEBPP_CHARSET_HPP
#define WEBPP_CHARSET_HPP

#include "../std/concepts.hpp"
#include "../std/string_view.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <tuple>
#include <type_traits>
#include <utility>

namespace webpp {

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <typename CharT, stl::size_t N>
    class charset_t : public stl::array<CharT, N> {
        static_assert(
          N > 0,
          "A charset with zero elements doesn't makes sense to construct.");

        template <typename Tpl, typename Callable, stl::size_t... I>
        constexpr void do_this_for_that(stl::index_sequence<I...>,
                                        Tpl const& tpl,
                                        Callable   callback) noexcept {
            (callback(stl::get<I>(tpl)), ...);
        }

        template <typename Tpl, typename Callable>
        constexpr void for_each_tuple(Tpl const& tpl,
                                      Callable   callback) noexcept {
            constexpr auto tpl_size = stl::tuple_size<Tpl>::value;
            do_this_for_that(stl::make_index_sequence<tpl_size>(), tpl,
                             callback);
        }

        using super = stl::array<CharT, N>;

      public:
        template <typename... T>
        requires((stl::same_as<T, CharT> && ...) &&
                 sizeof...(T) <= N) constexpr charset_t(T... data) noexcept
          : super{data...} {
        }

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
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
        contains(stl::basic_string_view<CharT> const& _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains(c))
                    return false;
            return true;
        }

        [[nodiscard]] constexpr auto string_view() const noexcept {
            return stl::basic_string_view<CharT>(this->data(), this->size());
        }

        [[nodiscard]] stl::basic_string<CharT> string() const noexcept {
            return stl::basic_string<CharT>(this->data(), this->size());
        }
    };

    template <typename CharT = char, stl::size_t N1, stl::size_t N2,
              stl::size_t... N>
    constexpr auto charset(charset_t<CharT, N1> const& set1,
                           charset_t<CharT, N2> const& set2,
                           charset_t<CharT, N> const&... csets) noexcept {
        return charset_t<CharT, N1 + N2 + (0 + ... + N)>{set1, set2, csets...};
    }

    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename _Tp, typename... _Up>
    requires((stl::same_as<_Tp, _Up> && ...)) charset_t(_Tp, _Up...)
      ->charset_t<_Tp, 1 + sizeof...(_Up)>;



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
          static_cast<stl::size_t>(Last) - static_cast<stl::size_t>(First) + 1;
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
    template <typename CharT = char>
    constexpr auto ALPHA = charset<CharT>(LOWER_ALPHA, UPPER_ALPHA);

    /**
     * This is the character set containing just numbers.
     */
    template <typename CharT = char>
    constexpr auto DIGIT = charset<CharT, '0', '9'>();

    /**
     * This is the character set containing just the characters allowed
     * in a hexadecimal digit.
     */
    template <typename CharT = char>
    constexpr auto HEXDIG = charset(DIGIT<CharT>, charset<CharT, 'A', 'F'>(),
                                    charset<CharT, 'a', 'f'>());


    template <typename CharT = char>
    constexpr auto ALPHA_DIGIT = charset<CharT>(ALPHA<CharT>, DIGIT<CharT>);
} // namespace webpp
#endif // WEBPP_CHARSET_HPP
