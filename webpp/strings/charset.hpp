#ifndef WEBPP_CHARSET_HPP
#define WEBPP_CHARSET_HPP

#include "../std/concepts.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <initializer_list>
#include <limits>
#include <utility>

namespace webpp {
    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <istl::CharType CharT, stl::size_t N>
        requires(N <= stl::numeric_limits<unsigned char>::max() + 1)
    struct charset : public stl::array<CharT, N> {
        using value_type                        = CharT;
        static constexpr stl::size_t array_size = N;

      private:
        using super = stl::array<value_type, N>;

        template <stl::size_t... I>
        consteval auto to_array(stl::index_sequence<I...>, auto&& items) noexcept {
            return super{items[I]...};
        }

        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        consteval auto merge(charset<value_type, N1> const& set1,
                             charset<value_type, N2> const& set2,
                             charset<value_type, NN> const&... c_sets) noexcept {
            super data;
            auto  write = [&, i = 0ul](auto const& set) constexpr mutable noexcept {
                stl::copy(set.begin(), set.end(), data.begin() + i);
                i += set.size();
            };
            write(set1);
            write(set2);
            (write(c_sets), ...);
            return data;
        }


      public:
        // we use +1, so we don't copy the null terminator character as well
        consteval charset(const value_type (&str)[N + 1]) noexcept
          : super{to_array(stl::make_index_sequence<N>(), str)} {}


        template <typename... T>
            requires((stl::convertible_to<T, value_type> && ...) && sizeof...(T) <= N)
        consteval charset(T... chars) noexcept : super{static_cast<value_type>(chars)...} {}

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        consteval charset(charset<value_type, N1> const& set1,
                          charset<value_type, N2> const& set2,
                          charset<value_type, NN> const&... c_sets) noexcept
          : super{merge<N1, N2, NN...>(set1, set2, c_sets...)} {
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
            // this is just an optimization to let the compiler optimize more
            if constexpr (N == 0) {
                return false;
            } else if constexpr (N == 1) {
                return c == super::operator[](0);
            } else if constexpr (N == 2) {
                return c == super::operator[](0) || c == super::operator[](1);
            } else if constexpr (N == 3) {
                return c == super::operator[](0) || c == super::operator[](1) || c == super::operator[](2);
            } else if constexpr (N == 4) {
                return c == super::operator[](0) || c == super::operator[](1) || c == super::operator[](2) ||
                       c == super::                                                          operator[](3);
            } else {
                return stl::find(super::begin(), super::end(), c) != super::end();
            }
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

        template <typename Iter>
        [[nodiscard]] constexpr bool contains(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg)
                if (!contains(*beg))
                    return false;
            return true;
        }

        template <typename Iter>
        [[nodiscard]] constexpr Iter contains_until(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg)
                if (!contains(*beg))
                    return beg;
            return end;
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
    concept CharSet =
      requires(T cs) {
          typename stl::remove_cvref_t<T>::value_type;
          stl::remove_cvref_t<T>::array_size;
          requires stl::same_as<
            stl::remove_cvref_t<T>,
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
    [[nodiscard]] static consteval auto charset_range() noexcept {
        constexpr stl::size_t    the_size = static_cast<stl::size_t>(Last - First) + 1ul;
        charset<CharT, the_size> data;
        for (CharT it = First; it != Last; ++it)
            data[static_cast<stl::size_t>(it - First)] = it;
        data[static_cast<stl::size_t>(Last - First)] = Last;
        return data;
    }


    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename Tp, typename... Up>
        requires((stl::same_as<Tp, Up> && ...)) // T and Us should be same
    charset(Tp, Up...)->charset<Tp, (1 + sizeof...(Up))>;

    template <istl::CharType CharT = char, stl::size_t N>
    charset(const CharT (&)[N]) -> charset<stl::remove_cvref_t<CharT>, N - 1>;

    template <istl::CharType CharT = char, stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charset(charset<CharT, N1> const&, charset<CharT, N2> const&, charset<CharT, N> const&...)
      -> charset<CharT, N1 + N2 + (0 + ... + N)>;

    // TODO: add non-constexpr (or constexpr if you can) charset(first, last) as well

    template <istl::CharType CharT = char>
    static constexpr auto NULL_CHAR = charset<CharT, 1>{'\0'};

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
    static constexpr auto HEXDIG =
      charset(DIGIT<CharT>, charset_range<CharT, 'A', 'F'>(), charset_range<CharT, 'a', 'f'>());


    template <istl::CharType CharT = char>
    static constexpr auto ALPHA_DIGIT = charset(ALPHA<CharT>, DIGIT<CharT>);




    ////////////////////////////// CHAR MAP //////////////////////////////


    /**
     * This is a Bolean Map of the specified characters.
     * This is faster to query than charset but might take more memory
     *
     * Attention: if your table is more than 256 cells, you're doing it wrong
     **/
    template <stl::size_t N>
        requires(N <= stl::numeric_limits<unsigned char>::max() + 1)
    struct charmap : public stl::array<bool, N> {
        static constexpr stl::size_t array_size = N;

      private:
        using super = stl::array<bool, N>;

#define webpp_or_set(set, out)                                      \
    do {                                                            \
        for (stl::size_t index = 0; index != (set).size(); ++index) \
            (out)[static_cast<stl::size_t>((set)[index])] |= true;  \
    } while (false)


      public:
        consteval charmap(const bool (&bools)[N]) noexcept : super{bools} {}

        template <typename CharT, stl::size_t... I>
        consteval charmap(const CharT (&... str)[I]) noexcept
          : super{} // init with false
        {
            (
              [this, &str]() {
                  webpp_or_set(str, *this);
              }(),
              ...); // make them true
        }

        template <istl::CharType... T>
            requires(sizeof...(T) <= N)
        consteval charmap(T... data) noexcept : super{} {
            stl::array<char, sizeof...(T)> const list{data...};
            webpp_or_set(list, *this);
        }

        /**
         * This constructs a character map that contains all the
         * characters in all the other given character maps.
         *
         * @param[in] characterMaps
         *     These are the character maps to include.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
            requires(N1 <= N && N2 <= N && (... && (NN <= N)))
        consteval charmap(charmap<N1> const& set1,
                          charmap<N2> const& set2,
                          charmap<NN> const&... c_sets) noexcept
          : super{} // init with false values
        {
            webpp_or_set(set1, *this);
            webpp_or_set(set2, *this);
            (
              [this, &c_sets]() {
                  webpp_or_set(c_sets, *this);
              }(),
              ...);
        }

        template <typename CharT, stl::size_t... NN>
            requires((... && (NN <= N)))
        consteval charmap(charset<CharT, NN> const&... c_sets) noexcept
          : super{} // init with false values
        {
            (
              [this, &c_sets]() {
                  webpp_or_set(c_sets, *this);
              }(),
              ...);
        }

        template <stl::size_t N1, typename... CharT>
        consteval charmap(charmap<N1> const& set1, CharT... c_set) noexcept
          : charmap{set1, charmap{c_set...}} {}


        /**
         * This method checks to see if the given character
         * is in the character map.
         *
         * @param[in] c
         *     This is the character to check.
         *
         * @return
         *     An indication of whether or not the given character
         *     is in the character map is returned.
         */
        template <typename CharT>
        [[nodiscard]] constexpr bool contains(CharT c) const noexcept {
            return this->operator[](static_cast<stl::size_t>(c));
        }

        /**
         * @brief checks if all the chars in the _cs is in the chars list or not
         * @param _cs
         * @return
         */
        [[nodiscard]] constexpr bool contains(stl::basic_string_view<bool> const& _cs) const noexcept {
            for (auto const& c : _cs)
                if (!contains(c))
                    return false;
            return true;
        }

        template <istl::StringView StrViewType = stl::basic_string_view<bool>>
        [[nodiscard]] constexpr auto string_view() const noexcept {
            return StrViewType(this->data(), this->size());
        }

        template <istl::String StrType = stl::basic_string<bool>>
        [[nodiscard]] StrType string(
          typename StrType::allocator_type const& alloc = typename StrType::allocator_type{}) const noexcept {
            return StrType{super::data(), super::size(), alloc};
        }

#undef webpp_or_set
    };

    template <typename T>
    concept CharMap = requires(T cs) {
                          stl::remove_cvref_t<T>::array_size;
                          requires istl::cvref_as<T, charmap<stl::remove_cvref_t<T>::array_size>>;
                      };

    /**
     * This constructs a character map that contains all the characters between the given
     * "first" and "last" characters, inclusive.
     *
     * first
     *     This is the first of the range of characters to put in the set.
     *
     * last
     *     This is the last of the range of characters to put in the set.
     */
    template <auto First, auto Last>
    [[nodiscard]] static consteval auto charmap_range() noexcept {
        constexpr auto    the_size = static_cast<stl::size_t>(Last) + 1;
        charmap<the_size> data{}; // all false
        for (auto it = First; it != Last + 1; ++it)
            data[it] = true;
        return data;
    }


    template <istl::CharType CharT = char, stl::size_t... N>
    charmap(const CharT (&... str)[N]) -> charmap<stl::max(N...) - 1>;

    template <stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charmap(charmap<N1> const&, charmap<N2> const&, charmap<N> const&...)
      -> charmap<stl::max({N1, N2, N...})>;


    using charmap_half =
      charmap<stl::numeric_limits<char>::max() + 1>; // Half Table (excluding negative chars)
    using charmap_full = charmap<stl::numeric_limits<unsigned char>::max() + 1>; // Full Table


    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

} // namespace webpp
#endif // WEBPP_CHARSET_HPP
