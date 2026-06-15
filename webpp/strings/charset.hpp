#ifndef WEBPP_CHARSET_HPP
#define WEBPP_CHARSET_HPP

#include "../std/concepts.hpp"
#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"

#include <concepts>

#ifdef __cpp_lib_constexpr_bitset
#    include <bitset>
#endif

#include <algorithm> // std::max
#include <array>
#include <climits>
#include <limits>
#include <utility>

namespace webpp {
    // NOLINTBEGIN(*-avoid-c-arrays)

    template <typename T>
    concept CharSet = requires(stl::remove_cvref_t<T> set) {
        typename stl::remove_cvref_t<T>::value_type;
        stl::remove_cvref_t<T>::array_size;

        requires requires(typename stl::remove_cvref_t<T>::value_type const* beg,
                          typename stl::remove_cvref_t<T>::value_type const* end,
                          typename stl::remove_cvref_t<T>::value_type        a_char) {
            { set.size() } noexcept -> stl::same_as<stl::size_t>;
            { set.contains(a_char) } noexcept -> stl::same_as<bool>;
            { set.contains(beg) } noexcept -> stl::same_as<bool>;
            {
                set.find_first_in(beg, end)
            } noexcept -> stl::same_as<typename stl::remove_cvref_t<T>::value_type const*>;
            {
                set.find_first_not_in(beg, end)
            } noexcept -> stl::same_as<typename stl::remove_cvref_t<T>::value_type const*>;
            set.set(1);
            { set.empty() } noexcept -> stl::same_as<bool>;
        };

        // Depends on CharSet itself:
        // { set.except(set) } noexcept;
    };

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <istl::CharType CharT, stl::size_t N>
        requires(N <= stl::numeric_limits<unsigned char>::max() + 1)
    struct charset : stl::array<CharT, N> {
        using value_type                        = CharT;
        static constexpr stl::size_t array_size = N;

      private:
        using super = stl::array<value_type, N>;

        template <stl::size_t... I>
        consteval auto to_array([[maybe_unused]] stl::index_sequence<I...> sequence, auto&& items) noexcept {
            return super{items[I]...};
        }

        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        consteval auto merge(charset<value_type, N1> const& set1,
                             charset<value_type, N2> const& set2,
                             charset<value_type, NN> const&... c_sets) noexcept {
            super       data{};
            stl::size_t index = 0;

            // Copy elements from set1
            for (stl::size_t i = 0; i < set1.size(); ++i) {
                data[index++] = set1[i];
            }

            // Copy elements from set2
            for (stl::size_t i = 0; i < set2.size(); ++i) {
                data[index++] = set2[i];
            }

            // Copy elements from additional sets
            (
              [&] {
                  for (stl::size_t i = 0; i < c_sets.size(); ++i) {
                      data[index++] = c_sets[i];
                  }
              }(),
              ...);

            return data;
        }


      public:
        // we use +1, so we don't copy the null terminator character as well
        explicit consteval charset(value_type const (&str)[N + 1]) noexcept
          : super{to_array(stl::make_index_sequence<N>(), str)} {}

        template <typename... T>
            requires((stl::convertible_to<T, value_type> && ...) && sizeof...(T) <= N)
        explicit consteval charset(T... chars) noexcept : super{static_cast<value_type>(chars)...} {}

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
        explicit consteval charset(charset<value_type, N1> const& set1,
                                   charset<value_type, N2> const& set2,
                                   charset<value_type, NN> const&... c_sets) noexcept
          : super{merge<N1, N2, NN...>(set1, set2, c_sets...)} {
            static_assert(N == (N1 + N2 + (0 + ... + NN)), "The charsets don't fit in this charset.");
        }

        /**
         * This method checks to see if the given character is in the character set.
         *
         * @param[in] character
         *     This is the character to check.
         *
         * @return
         *     An indication of whether the given character is in the character set is returned.
         */
        [[nodiscard]] constexpr bool contains(value_type character) const noexcept {
            // this is just an optimization to let the compiler optimize more
            if constexpr (N == 0) {
                return false;
            } else if constexpr (N == 1) {
                return character == super::operator[](0);
            } else if constexpr (N == 2) {
                return character == super::operator[](0) || character == super::operator[](1);
            } else if constexpr (N == 3) {
                return character == super::operator[](0) || character == super::operator[](1) ||
                       character == super::operator[](2);
            } else if constexpr (N == 4) {
                return character == super::operator[](0) || character == super::operator[](1) ||
                       character == super::operator[](2) || character == super::operator[](3);
            } else {
                // I don't want to include <algorithm>, it's like 9000 lines of code
                // return stl::find(super::begin(), super::end(), character) != super::end();
                for (auto const cur_ch : static_cast<super const&>(*this)) {
                    if (cur_ch == character) {
                        return true;
                    }
                }
                return false;
            }
        }

        template <stl::integral T>
            requires(!stl::same_as<T, value_type>)
        [[nodiscard]] constexpr bool contains(T const character) const noexcept {
            if constexpr (!stl::unsigned_integral<T>) {
                return character > 0 && contains(static_cast<value_type>(character));
            } else {
                return contains(static_cast<value_type>(character));
            }
        }

        [[nodiscard]] constexpr bool unsafe_contains(value_type character) const noexcept {
            return contains(character);
        }

        /**
         * @brief Checks if all the characters in the string view are present in the list of characters.
         *
         * @param inp_str The string view to check.
         * @return True if all characters are present in the list, false otherwise.
         */
        [[nodiscard]] constexpr bool contains(stl::basic_string_view<value_type> inp_str) const noexcept {
            for (auto const character : inp_str) {
                if (!contains(character)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Checks if a given range contains all elements in a container.
         *
         * This function iterates over the given range and checks if each element
         * exists in the container. It returns true if all elements exist in the
         * container, and false otherwise.
         *
         * @tparam Iter The iterator type of the range.
         * @param beg The iterator pointing to the beginning of the range.
         * @param end The iterator pointing to the end of the range.
         * @return True if all elements exist in the container, false otherwise.
         *
         * @note The elements are checked for containment using the contains() member function
         *       of the container class.
         *
         * @par Example:
         * \code
         * std::string_view str {"this is a string"};
         *
         * bool result = DIGIT<char>.contains(str.begin(), str.end());
         *
         * // result = true
         * \endcode
         *
         * @see contains
         */
        template <typename Iter>
        [[nodiscard]] constexpr bool contains(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (!contains(*beg)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Finds the first element in a range that is not contained in the container.
         *
         * This function searches for the first element in the range [beg, end) that is not contained
         * in the container. The function uses the `contains` method of the container to check for
         * containment.
         *
         * @tparam Iter The iterator type of the range.
         * @param beg Iterator to the beginning of the range.
         * @param end Iterator to the end of the range.
         * @return Iterator to the first element in the range that is not contained in the container.
         *         If all elements are contained, returns the `end` iterator.
         */
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_not_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (!contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        /// find the first character that's in the range
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        /**
         * Exclude these charsets from the original one; all of these sets MUST be in the original charset.
         */
        template <stl::size_t... NN>
        [[nodiscard]] consteval charset<value_type, array_size - (NN + ...)> except(
          charset<value_type, NN> const&... sets) const noexcept {
            charset<value_type, array_size - (NN + ...)> chars;
            stl::size_t                                  index = 0;
            for (auto const character : *this) {
                if ((sets.contains(character) || ...)) {
                    continue;
                }
                chars[index] = character;
                // *stl::next(chars.data(), index) = character;
                ++index;
            }
            return chars;
        }

        /**
         * @brief Creates a string view of the given charset
         *
         * The template parameter `StrViewType` is the type of the string view to be returned. It defaults to
         * `std::basic_string_view<value_type>`, which is a string view of characters of the underlying string
         * type.
         *
         * @returns A string view of the current string object.
         *
         * @tparam StrViewType The type of the string view to be returned.
         *
         */
        template <istl::StringView StrViewType = stl::basic_string_view<value_type>>
        [[nodiscard]] consteval auto string_view() const noexcept {
            return StrViewType(this->data(), this->size());
        }

        /**
         * @brief Create a new string from the charsets.
         *
         * This method creates a new string from the existing string by copying the data and using the
         * specified allocator.
         *
         * @tparam StrType The type of string to be created. It should be a specialization of
         * `std::basic_string` or `istl::String`.
         * @param inp_alloc The allocator
         * @return The new string created from the existing string, with the specified allocator.
         *
         * @see istl::String
         */
        template <istl::String StrType = stl::basic_string<value_type>>
        [[nodiscard]] constexpr StrType string(
          typename StrType::allocator_type const& inp_alloc = typename StrType::allocator_type{}) const noexcept {
            return StrType{this->data(), this->size(), inp_alloc};
        }

        /// default value is for compatibility
        constexpr charset& set(stl::size_t pos, value_type val = '\0') noexcept {
            this->operator[](pos) = val;
            return *this;
        }
    };

    /**
     * This constructs a character set that contains all the characters between the given
     * "first" and "last" characters, inclusive.
     *
     * @tparam[in] CharT
     *     Character Type
     *
     * @tparam[in] First
     *     This is the first of the range of characters to put in the set.
     *
     * @tparam[in] Last
     *     This is the last of the range of characters to put in the set.
     */
    template <istl::CharType CharT = char, CharT First, CharT Last>
    [[nodiscard]] static consteval auto charset_range() noexcept {
        constexpr stl::size_t    the_size = static_cast<stl::size_t>(Last - First) + 1UL;
        charset<CharT, the_size> data;
        for (CharT it = First; it != Last; ++it) {
            data[static_cast<stl::size_t>(it - First)] = it;
        }
        data[static_cast<stl::size_t>(Last - First)] = Last;
        return data;
    }

    /**
     * Type deduction. I stole this from a type deduction from std::array
     */
    template <typename Tp, typename... Up>
        requires((stl::same_as<Tp, Up> && ...)) // T and Us should be same
    charset(Tp, Up...) -> charset<Tp, (1 + sizeof...(Up))>;

    template <istl::CharType CharT = char, stl::size_t N>
    charset(CharT const (&)[N]) -> charset<stl::remove_cvref_t<CharT>, N - 1>;

    template <istl::CharType CharT = char, stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charset(charset<CharT, N1> const&, charset<CharT, N2> const&, charset<CharT, N> const&...)
      -> charset<CharT, N1 + N2 + (0 + ... + N)>;

    // todo: add non-constexpr (or constexpr if you can) charset(first, last) as well

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

    template <istl::CharType CharT = char>
    static constexpr auto ALL_ASCII = charset_range<CharT, 0U, 0x7FU>();

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

    template <istl::CharType CharT = char>
    static constexpr auto LOWER_ALPHA_DIGIT = charset(LOWER_ALPHA<CharT>, DIGIT<CharT>);

    ////////////////////////////// CHAR MAP //////////////////////////////


    /**
     * This is a Boolean Map of the specified characters.
     * This is faster to query than charset but might take more memory
     *
     * Attention: if your table is more than 256 cells, you're doing it wrong
     */
    template <stl::size_t N>
        requires(N <= stl::numeric_limits<unsigned char>::max() + 1)
    struct charmap : stl::array<bool, N> {
        static constexpr stl::size_t array_size = N;
        using value_type                        = bool;

      private:
        using super = stl::array<bool, N>;

        // NOLINTBEGIN(*-avoid-do-while, *-macro-usage)
#define webpp_set_at(set, out)                                        \
    do {                                                              \
        for (stl::size_t index = 0; index != (set).size(); ++index) { \
            (out)[static_cast<stl::size_t>((set)[index])] = true;     \
        }                                                             \
    } while (false)


#define webpp_xor_all(set, out)                                       \
    do {                                                              \
        for (stl::size_t index = 0; index != (set).size(); ++index) { \
            (out)[index] |= (set)[index];                             \
        }                                                             \
    } while (false)
        // NOLINTEND(*-avoid-do-while, *-macro-usage)


      public:
        explicit consteval charmap(bool const (&bools)[N]) noexcept : super{bools} {}

        template <typename CharT, stl::size_t... I>
        explicit consteval charmap(CharT const (&... strs)[I]) noexcept
          : super{} // init with false
        {
            (
              [this](CharT const(&str)[I]) {
                  webpp_set_at(str, *this);
              }(strs),
              ...); // make them true
        }

        template <istl::CharType... T>
            requires(sizeof...(T) <= N)
        explicit consteval charmap(T... data) noexcept : super{} {
            stl::array<char, sizeof...(T)> const list{data...};
            webpp_set_at(list, *this);
        }

        /**
         * This constructs a character map that contains all the
         * characters in all the other given character maps.
         */
        template <stl::size_t N1, stl::size_t N2, stl::size_t... NN>
            requires(N1 <= N && N2 <= N && (... && (NN <= N)))
        explicit consteval charmap(charmap<N1> const& set1,
                                   charmap<N2> const& set2,
                                   charmap<NN> const&... c_sets) noexcept
          : super{} // init with false values
        {
            webpp_xor_all(set1, *this);
            webpp_xor_all(set2, *this);
            (
              [this](charmap<NN> const& set) {
                  webpp_xor_all(set, *this);
              }(c_sets),
              ...);
        }

        template <typename CharT, stl::size_t... NN>
            requires((... && (NN <= N)))
        explicit consteval charmap(charset<CharT, NN> const&... c_sets) noexcept
          : super{} // init with false values
        {
            (
              [this](charset<CharT, NN> const& set) {
                  webpp_set_at(set, *this);
              }(c_sets),
              ...);
        }

        template <stl::size_t N1, typename... CharT>
        explicit consteval charmap(charmap<N1> const& set1, CharT... c_set) noexcept
          : charmap{set1, charmap{c_set...}} {}

        /**
         * Exclude these charsets from the original one
         */
        [[nodiscard]] consteval charmap<array_size> except(CharSet auto const&... sets) const noexcept {
            charmap<array_size> chars{};
            for (stl::size_t character = 0; character != array_size; ++character) {
                if ((sets.contains(character) || ...)) {
                    continue;
                }
                chars.set(character, this->contains(character));
            }
            return chars;
        }

        /**
         * This method checks to see if the given character is in the character map.
         *
         * @param[in] character
         *     This is the character to check.
         *
         * @return
         *     An indication of whether the given character is in the character map is returned.
         */
        template <typename CharT = stl::size_t>
        [[nodiscard]] constexpr bool unsafe_contains(CharT character) const noexcept {
            return this->operator[](static_cast<stl::size_t>(character));
        }

        template <typename CharT = stl::size_t>
        [[nodiscard]] constexpr bool contains(CharT character) const noexcept {
            using unsigned_char_type = stl::make_unsigned_t<CharT>;
            auto const uc            = static_cast<unsigned_char_type>(character);
            return uc < N && this->operator[](uc);
        }

        template <typename Iter>
        [[nodiscard]] constexpr bool contains(Iter spos, Iter const& send) const noexcept {
            for (; spos != send; ++spos) {
                if (!contains(*spos)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief checks if all the chars in the inp_set is in the chars list or not
         * @param inp_set
         */
        [[nodiscard]] constexpr bool contains(stl::basic_string_view<bool> const inp_set) const noexcept {
            for (auto const character : inp_set) {
                if (!contains(character)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Finds the first element in a range that is not contained in the container.
         *
         * This function searches for the first element in the range [beg, end) that is not contained
         * in the container. The function uses the `contains` method of the container to check for
         * containment.
         *
         * @tparam Iter The iterator type of the range.
         * @param beg Iterator to the beginning of the range.
         * @param end Iterator to the end of the range.
         * @return Iterator to the first element in the range that is not contained in the container.
         *         If all elements are contained, returns the `end` iterator.
         */
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_not_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (!contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        /// find the first character that's in the range
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        template <istl::StringView StrViewType = stl::basic_string_view<bool>>
        [[nodiscard]] constexpr auto string_view() const noexcept {
            return StrViewType(this->data(), this->size());
        }

        template <istl::String StrType = stl::basic_string<bool>>
        [[nodiscard]] StrType string(
          typename StrType::allocator_type const& inp_alloc = typename StrType::allocator_type{}) const noexcept {
            return StrType{super::data(), super::size(), inp_alloc};
        }

        constexpr charmap& set(stl::size_t pos, bool val = true) noexcept {
            this->operator[](pos) = val;
            return *this;
        }

#undef webpp_set_at
#undef webpp_xor_all
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
        for (auto it = First; it != Last + 1; ++it) {
            data[static_cast<stl::size_t>(it)] = true;
        }
        return data;
    }

    template <istl::CharType CharT = char, stl::size_t... N>
    charmap(CharT const (&... str)[N]) -> charmap<stl::max({N...}) - 1>;

    template <stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charmap(charmap<N1> const&, charmap<N2> const&, charmap<N> const&...) -> charmap<stl::max({N1, N2, N...})>;

    template <typename CharT, stl::size_t N1, stl::size_t N2, stl::size_t... N>
    charmap(charset<CharT, N1> const&, charset<CharT, N2> const&, charset<CharT, N> const&...)
      -> charmap<stl::max({N1, N2, N...})>;


    // Half Table (excluding negative chars)
    using charmap_half = charmap<stl::numeric_limits<char>::max() + 1>;

    // Full Table
    using charmap_full = charmap<stl::numeric_limits<unsigned char>::max() + 1>;




#ifndef __cpp_lib_constexpr_bitset

    template <stl::size_t N>
    using bitmap = charmap<N>;

#else
    template <stl::size_t N>
    struct bitmap : stl::bitset<N> {
        using value_type  = bool;
        using bitset_type = stl::bitset<N>;

        static constexpr stl::size_t array_size = N;

        using stl::bitset<N>::bitset;

        template <stl::size_t N1, stl::size_t... NN>
            requires((N1 <= N) && ((NN <= N) && ...)) // todo
        explicit consteval bitmap(bitmap<N1> const& set1, bitmap<NN> const&... sets) noexcept : bitset_type{set1} {
            ((*this |= sets), ...);
        }

        template <istl::CharType... CharT>
        explicit consteval bitmap(CharT... chars) noexcept {
            (this->set(static_cast<stl::size_t>(chars)), ...);
        }

        template <istl::CharType... CharT>
        explicit consteval bitmap(bitmap<N> const& inp_bitmap, CharT... chars) noexcept : bitset_type{inp_bitmap} {
            (this->set(static_cast<stl::size_t>(chars)), ...);
        }

        template <typename... T>
            requires((requires(T set) { set.string_view(); }) && ...)
        explicit consteval bitmap(T const&... sets) noexcept {
            (([this](T const& set) constexpr noexcept {
                 for (auto const character : set) {
                     this->set(static_cast<stl::size_t>(character));
                 }
             })(sets),
             ...);
        }

        /**
         * Exclude these charsets from the original one
         */
        [[nodiscard]] consteval bitmap<array_size> except(CharSet auto const&... sets) const noexcept {
            bitmap<array_size> chars{};
            for (stl::size_t character = 0ULL; character != array_size; ++character) {
                if ((sets.contains(character) || ...)) {
                    continue;
                }
                chars.set(character, this->contains(character));
            }
            return chars;
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool unsafe_contains(CharT character) const noexcept {
            webpp_assume(character >= 0 && static_cast<stl::size_t>(character) <= N);
            return this->operator[](static_cast<stl::size_t>(character));
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool contains(CharT character) const noexcept {
            using unsigned_char_type = stl::make_unsigned_t<CharT>;
            auto const index         = static_cast<unsigned_char_type>(character);
            return index < N && this->operator[](index);
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool contains(stl::basic_string_view<CharT> set) const noexcept {
            for (auto const character : set) {
                if (!this->contains(character)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Finds the first element in a range that is not contained in the container.
         *
         * This function searches for the first element in the range [beg, end) that is not contained
         * in the container. The function uses the `contains` method of the container to check for
         * containment.
         *
         * @tparam Iter The iterator type of the range.
         * @param beg Iterator to the beginning of the range.
         * @param end Iterator to the end of the range.
         * @return Iterator to the first element in the range that is not contained in the container.
         *         If all elements are contained, returns the `end` iterator.
         */
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_not_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (!contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        /// find the first character that is in the range
        template <typename Iter>
        [[nodiscard]] constexpr Iter find_first_in(Iter beg, Iter end) const noexcept {
            for (; beg != end; ++beg) {
                if (contains(*beg)) {
                    return beg;
                }
            }
            return end;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return this->size() == 0;
        }
    };

    template <istl::CharType CharT = char, stl::size_t... N>
    bitmap(CharT const (&... str)[N]) -> bitmap<stl::max({N...}) - 1>;

    template <typename... SetN>
        requires(requires { SetN::array_size; } && ...)
    bitmap(SetN&&...) -> bitmap<stl::max({SetN::array_size...})>;
#endif


    // Half Table (excluding negative chars)
    using bitmap_half = bitmap<stl::numeric_limits<char>::max() + 1>;

    // Full Table
    using bitmap_full = bitmap<stl::numeric_limits<unsigned char>::max() + 1>;

    template <auto First, auto Last, stl::size_t Size = static_cast<stl::size_t>(Last) + 1>
    [[nodiscard]] static consteval auto bitmap_range() noexcept {
        bitmap<Size> data{}; // all false
        for (auto it = static_cast<stl::size_t>(First); it != static_cast<stl::size_t>(Last) + 1; ++it) {
            data.set(it);
        }
        return data;
    }

    template <typename>
    struct char_size {};

    // for std::bitset and bitmap
    template <stl::size_t N>
    struct char_size<bitmap<N>> {
        static constexpr auto value = N;
    };

    // for std::array and charmap
    template <template <typename, stl::size_t> typename TT, typename CharT, stl::size_t N>
    struct char_size<TT<CharT, N>> {
        static constexpr auto value = stl::numeric_limits<CharT>::max();
    };

    /**
     * Specified a category that then can be passed to categorize
     * @tparam CharSetT Character Set
     * @tparam T Value Type
     */
    template <typename CharSetT, typename T>
    struct cat {
        CharSetT set;
        T        value;
    };

    /**
     * Map the specified sets of characters into the specified values.
     * Usage:
     *    constexpr auto mappings = categorize(
     *      cat{DIGITS<char>, 1},
     *      cat{"abc", 2}
     *    );
     *
     * @tparam T Value Type
     * @tparam N Length of the array
     * @tparam CharSetsT Character Sets
     * @return an array
     */
    template <typename T, stl::size_t N, typename... CharSetsT>
    [[nodiscard]] static consteval auto categorize(cat<CharSetsT, T> const&... sets) noexcept {
        static_assert(N <= 256, "We cast to uint8_t, which means you can't do more than 255");
        using flag_type =
          typename stl::conditional_t<stl::is_enum_v<T>, stl::underlying_type<T>, stl::type_identity<T>>::type;
        stl::array<flag_type, N> data{};
        auto const               value_of = [](auto value) {
            if constexpr (stl::is_enum_v<T>) {
                return static_cast<flag_type>(value);
            } else {
                return static_cast<T>(value);
            }
        };
        (([&]<typename CharSetT>(CharSetT const& set, auto value) {
             using value_type = istl::char_type_of_t<CharSetT>;
             if constexpr (stl::same_as<value_type, bool>) {
                 // things like std::bitset
                 auto const len = set.size();
                 for (stl::size_t i = 0; i < len; ++i) {
                     if (set[i]) {
                         data[i] |= value_of(value);
                     }
                 }
             } else if constexpr (istl::Iterable<CharSetT>) {
                 for (auto const character : set) {
                     data[static_cast<stl::uint8_t>(character)] |= value_of(value);
                 }
             } else { // for strings (char const*)
                 for (auto cur = set; *cur != '\0'; ++cur) {
                     data[static_cast<stl::uint8_t>(*cur)] |= value_of(value);
                 }
             }
         })(sets.set, sets.value),
         ...);
        return data;
    }

    template <stl::size_t N, typename T, typename... CharSetsT>
    [[nodiscard]] static consteval auto categorize(cat<CharSetsT, T> const&... sets) noexcept {
        return categorize<T, N>(sets...);
    }

    template <typename T, typename... CharSetsT>
    [[nodiscard]] static consteval auto categorize(cat<CharSetsT, T> const&... sets) noexcept {
        webpp_static_constexpr auto len = stl::max({char_size<CharSetsT>::value...});
        return categorize<T, len>(sets...);
    }

    template <stl::integral T = stl::uint32_t, stl::size_t N, typename CharT = char32_t>
    [[nodiscard]] static constexpr T or_one(stl::array<T, N> const& arr, CharT const code_point) noexcept {
        static_assert(N <= 256, "We cast to uint8_t, which means you can't do more than 255");
        using char_type        = stl::make_unsigned_t<CharT>;
        constexpr auto last_el = static_cast<char_type>(N - 1U);
        return arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(code_point), last_el))];
    }

    /**
     * Usage:
     *   auto mapping = categorize(...);
     *   switch (or_all(mapping, start, end)) {
     *     case ...: ...;
     *     case ...: ...;
     *   }
     */
    template <stl::integral T = stl::uint32_t, stl::size_t N, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr T or_all(stl::array<T, N> const& arr, Iter pos, Iter const end) noexcept {
        static_assert(N <= 256, "We cast to uint8_t, which means you can't do more than 255");
        using char_type        = stl::make_unsigned_t<stl::iter_value_t<Iter>>;
        constexpr auto last_el = static_cast<char_type>(N - 1U);
        T              res{};
        while (stl::next(pos, 4) <= end) {
            res |= arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos++), last_el))];
            res |= arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos++), last_el))];
            res |= arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos++), last_el))];
            res |= arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos++), last_el))];
        }
        for (; pos != end; ++pos) {
            res |= arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos), last_el))];
        }
        return res;
    }

    template <stl::integral T = stl::uint32_t, stl::size_t N, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr T
    or_all(stl::array<T, N> const& arr, T const stop_token, Iter& pos, Iter const end) noexcept {
        static_assert(N <= 256, "We cast to uint8_t, which means you can't do more than 255");
        using char_type        = stl::make_unsigned_t<stl::iter_value_t<Iter>>;
        constexpr auto last_el = static_cast<char_type>(N - 1U);
        T              res{};
        // todo: this can be optimized using SIMD
        for (; pos != end; ++pos) {
            auto const code =
              arr[static_cast<stl::uint8_t>(stl::min<char_type>(static_cast<char_type>(*pos), last_el))];
            res |= code;
            if ((code & stop_token) == stop_token) [[unlikely]] {
                break;
            }
        }
        return res;
    }

    template <stl::integral T = stl::uint32_t, stl::size_t N, stl::random_access_iterator Iter>
    [[nodiscard]] static constexpr T
    or_all_if(stl::array<T, N> const& arr, Iter& pos, Iter const end, auto&& func) noexcept {
        static_assert(N <= 256, "We cast to uint8_t, which means you can't do more than 255");
        using char_type = stl::make_unsigned_t<stl::iter_value_t<Iter>>;
        T res{};
        // todo: this can be optimized
        for (; pos != end && !func(res); ++pos) {
            // unsigned char must be used to make sure the Unicode Code Units don't show up as negative
            if constexpr (sizeof(char8_t) == sizeof(char_type)) {
                auto const index  = static_cast<stl::uint8_t>(*pos);
                res              |= static_cast<T>(arr[index]);
            } else {
                auto const index  = static_cast<stl::uint8_t>(stl::min<char_type>(*pos, N - 1U));
                res              |= static_cast<T>(arr[index]);
            }
        }
        return res;
    }

    template <stl::size_t NewLen, istl::CharType CharT, stl::size_t N>
    [[nodiscard]] static consteval charmap<NewLen> inverse(charset<CharT, N> const& set) noexcept {
        charmap<NewLen> res{};
        static_assert(NewLen >= N, "This is not a cutting tool");
        for (CharT cur = 0; cur < static_cast<CharT>(N); ++cur) {
            if (set.contains(cur)) {
                continue;
            }
            res.set(static_cast<stl::size_t>(cur));
        }
        return res;
    }

    template <istl::CharType CharT, stl::size_t N>
    [[nodiscard]] static consteval auto inverse(charset<CharT, N> const& set) noexcept {
        static_assert(sizeof(CharT) <= sizeof(stl::uint8_t),
                      "Too big of a size, you're probably didn't want this; try specifying the length manually.");
        return inverse<stl::numeric_limits<CharT>::max()>(set);
    }

    // NOLINTEND(*-avoid-c-arrays)
} // namespace webpp


#endif // WEBPP_CHARSET_HPP
