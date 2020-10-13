#ifndef WEBPP_CHARSET_HPP
#define WEBPP_CHARSET_HPP

#include "../std/concepts.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
#include <tuple>
#include <utility>

namespace webpp {

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <istl::CharType CharT = char, stl::size_t N = 0>
    struct charset : public stl::array<CharT, N> {
        using value_type = stl::remove_cvref_t<CharT>;

      private:
        template <typename Tpl, typename Callable, stl::size_t... I>
        constexpr void do_this_for_that(stl::index_sequence<I...>, Tpl const& tpl,
                                        Callable callback) noexcept {
            (callback(stl::get<I>(tpl)), ...);
        }

        template <typename Tpl, typename Callable>
        constexpr void for_each_tuple(Tpl const& tpl, Callable callback) noexcept {
            constexpr auto tpl_size = stl::tuple_size<Tpl>::value;
            do_this_for_that(stl::make_index_sequence<tpl_size>(), tpl, callback);
        }

        using super = stl::array<value_type, N>;

        template <stl::size_t... I>
        constexpr auto to_array(stl::index_sequence<I...>, auto&& items) noexcept {
            return super{items[I]...};
        }


      public:
        constexpr charset(const value_type (&str)[N]) noexcept
          : super{to_array(stl::make_index_sequence<N>(), str)} {}

        template <typename... T>
        requires((stl::same_as<T, value_type> && ...) &&
                 sizeof...(T) <= N) constexpr charset(T... data) noexcept
          : super{data...} {}

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        constexpr explicit charset(charset<value_type, N1> const& set1, charset<value_type, N2> const& set2,
                                   charset<value_type, NN> const&... c_sets) noexcept {
            auto write = [this, i = 0u](auto const& set) mutable noexcept {
                for (auto c : set)
                    super::operator[](i++) = c;
            };
            write(set1);
            write(set2);
            (write(c_sets), ...);
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
        template <value_type First, value_type Last>
        [[nodiscard]] static constexpr auto range() noexcept {
            constexpr auto the_size = static_cast<stl::size_t>(Last) - static_cast<stl::size_t>(First) + 1;
            charset<value_type, the_size> data;
            for (auto it = First; it != Last + 1; ++it)
                data[it - First] = it;
            return data;
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
        [[nodiscard]] constexpr bool contains(value_type c) const noexcept {
            return stl::find(super::begin(), super::end(), c) != super::end();
        }

        /**
         * @brief checks if all the chars in the _cs is in the chars list or not
         * @param _cs
         * @return
         */
        [[nodiscard]] constexpr bool contains(stl::basic_string_view<value_type> const& _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains(c))
                    return false;
            return true;
        }

        template <istl::StringView StrViewType = stl::basic_string_view<value_type>>
        [[nodiscard]] constexpr auto string_view() const noexcept {
            return StrViewType(this->data(), this->size());
        }

        template <istl::String StrType = stl::basic_string<value_type>>
        [[nodiscard]] StrType string(
          typename StrType::allocator_type const& alloc = typename StrType::allocator_type{}) const noexcept {
            return StrType{super::data(), super::size(), alloc};
        }
    };


    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename Tp, typename... Up>
    requires((stl::same_as<Tp, Up> && ...)) charset(Tp, Up...)->charset<Tp, 1 + sizeof...(Up)>;

    template <typename Tp, stl::size_t N>
    charset(const Tp (&)[N])
      -> charset<stl::remove_cvref_t<Tp>, N - 1>; // minus 1 because we don't want to copy the null terminator character.

    template <istl::CharType CharT = char, stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charset(charset<CharT, N1>, charset<CharT, N2>, charset<CharT, N>...)
      -> charset<CharT, N1 + N2 + (N + ...)>;

    // TODO: add non-constexpr (or constexpr if you can) charset(first, last) as well


    template <istl::CharType CharT = char>
    constexpr auto LOWER_ALPHA = charset<CharT>::template range<'a', 'z'>();

    template <istl::CharType CharT = char>
    constexpr auto UPPER_ALPHA = charset<CharT>::template range<'A', 'Z'>();

    /**
     * This is the character set containing just the alphabetic characters
     * from the ASCII character set.
     */
    template <istl::CharType CharT = char>
    constexpr auto ALPHA = charset<CharT>(LOWER_ALPHA<CharT>, UPPER_ALPHA<CharT>);

    /**
     * This is the character set containing just numbers.
     */
    template <istl::CharType CharT = char>
    constexpr auto DIGIT = charset<CharT>::template range<'0', '9'>();

    /**
     * This is the character set containing just the characters allowed
     * in a hexadecimal digit.
     */
    template <istl::CharType CharT = char>
    constexpr auto HEXDIG = charset(DIGIT<CharT>, charset<CharT>::template range<'A', 'F'>(),
                                    charset<CharT>::template range<'a', 'f'>());


    template <istl::CharType CharT = char>
    constexpr auto ALPHA_DIGIT = charset<CharT>(ALPHA<CharT>, DIGIT<CharT>);



} // namespace webpp
#endif // WEBPP_CHARSET_HPP
