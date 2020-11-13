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
    template <istl::CharType CharT, stl::size_t N>
    struct charset : public stl::array<stl::remove_cvref_t<CharT>, N> {
        using value_type = stl::remove_cvref_t<CharT>;
        static constexpr stl::size_t array_size = N;

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

        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        constexpr auto merge(charset<value_type, N1> const& set1, charset<value_type, N2> const& set2,
                          charset<value_type, NN> const&... c_sets) noexcept {
            super data;
            auto write = [&, i = 0u](auto const& set) mutable noexcept {
              stl::copy(set.begin(), set.end(), data.begin() + i);
              i += set.size();
            };
            write(set1);
            write(set2);
            (write(c_sets), ...);
            return data;
        }


      public:
        // we use +1 so we don't copy the null terminator character as well
        constexpr charset(const value_type (&str)[N + 1]) noexcept
          : super{to_array(stl::make_index_sequence<N>(), str)} {}

        template <typename... T>
        requires((stl::same_as<T, value_type> && ...) && sizeof...(T) <= N)
          constexpr charset(T &&... data) noexcept
          : super{stl::forward<T>(data)...} {}

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        constexpr charset(charset<value_type, N1> const& set1, charset<value_type, N2> const& set2,
                                   charset<value_type, NN> const&... c_sets) noexcept :
            super{merge<N1, N2, NN...>(set1, set2, c_sets...)}
        {
            static_assert(N == (N1 + N2 + (0 + ... + NN)), "The charsets don't fit in this charset.");
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

    template <typename T>
    concept CharSet = requires (T cs) {
      typename stl::remove_cvref_t<T>::value_type;
      stl::remove_cvref_t<T>::array_size;
      stl::same_as<stl::remove_cvref_t<T>,
                   charset<typename stl::remove_cvref_t<T>::value_type, stl::remove_cvref_t<T>::array_size>>;
    };

    /**
     * This constructs a character set that contains all the characters between the given
     * "first" and "last" characters, inclusive.
     *
     * @param[in] first
     *     This is the first of the range of characters to put in the set.
     *
     * @param[in] last
     *     This is the last of the range of characters to put in the set.
     */
    template <istl::CharType CharT = char, CharT First, CharT Last>
    [[nodiscard]] static constexpr auto charset_range() noexcept {
        constexpr auto the_size = static_cast<stl::size_t>(Last) - static_cast<stl::size_t>(First) + 1;
        charset<CharT, the_size> data;
        for (auto it = First; it != Last; ++it)
            data[it - First] = it;
        data[static_cast<stl::size_t>(Last - First)] = Last;
        return data;
    }


    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename Tp, typename... Up>
    requires((stl::same_as<Tp, Up> && ...)) charset(Tp, Up...)->charset<Tp, 1 + sizeof...(Up)>;

    template <istl::CharType CharT = char, stl::size_t N>
    charset(const CharT (&)[N]) -> charset<stl::remove_cvref_t<CharT>, N - 1>;

    template <istl::CharType CharT = char, stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charset(charset<CharT, N1> const&, charset<CharT, N2> const&, charset<CharT, N> const&...)
      -> charset<CharT, N1 + N2 + (0 + ... + N)>;

    // TODO: add non-constexpr (or constexpr if you can) charset(first, last) as well


    template <istl::CharType CharT = char>
    static constexpr auto LOWER_ALPHA = charset_range<CharT, 'a', 'z'>();

    template <istl::CharType CharT = char>
    static constexpr auto UPPER_ALPHA = charset_range<CharT, 'A', 'Z'>();

    /**
     * This is the character set containing just the alphabetic characters
     * from the ASCII character set.
     */
    template <istl::CharType CharT = char>
    static constexpr auto ALPHA = charset(LOWER_ALPHA<CharT>, UPPER_ALPHA<CharT>);

    /**
     * This is the character set containing just numbers.
     */
    template <istl::CharType CharT = char>
    static constexpr auto DIGIT = charset_range<CharT, '0', '9'>();

    /**
     * This is the character set containing just the characters allowed
     * in a hexadecimal digit.
     */
    template <istl::CharType CharT = char>
    static constexpr auto HEXDIG = charset(DIGIT<CharT>, charset_range<CharT, 'A', 'F'>(),
                                    charset_range<CharT, 'a', 'f'>());


    template <istl::CharType CharT = char>
    static constexpr auto ALPHA_DIGIT = charset(ALPHA<CharT>, DIGIT<CharT>);



} // namespace webpp
#endif // WEBPP_CHARSET_HPP
