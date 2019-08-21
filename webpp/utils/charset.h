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

#include <algorithm>
#include <array>
#include <initializer_list>

namespace webpp {

    /**
     * This represents a set of characters which can be queried
     * to find out if a character is in the set or not.
     */
    template <std::size_t N = 1>
    class charset {

        static_assert(
            N > 0,
            "A charset with zero elements doesn't makes sense to construct.");

      private:
        std::array<char, N> chars;

        // Lifecycle management
      public:
        ~charset() noexcept = default;
        charset(const charset&) = default;
        charset(charset&&) noexcept = default;
        charset& operator=(const charset&) = default;
        charset& operator=(charset&&) noexcept = default;

        // Methods
      public:
        /**
         * This is the default constructor.
         */
        constexpr charset() = default;

        /**
         * This constructs a character set that contains
         * just the given character.
         *
         * @param[in] c
         *     This is the only character to put in the set.
         */
        constexpr charset(char c) { chars[0] = c; }

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
        constexpr charset(char first, char last) noexcept {
            std::size_t i = 0;
            for (char c = first; c < last + 1; ++c) {
                if (!contains(c))
                    chars[i++] = c;
            }

            // filling the rest with null char
            for (; i < N; i++)
                chars[i] = '\0';
        }

        /**
         * This constructs a character set that contains all the
         * characters in all the other given character sets.
         *
         * @param[in] characterSets
         *     These are the character sets to include.
         */
        constexpr charset(
            const std::initializer_list<const charset> csets) noexcept {
            std::size_t i = 0;
            for (auto const& cset : csets) {
                for (auto const& c : cset) {
                    if (!contains(c))
                        chars[i++] = c;
                }
            }

            // filling the rest with null char
            for (; i < N; i++)
                chars[i] = '\0';
        }

        constexpr charset(decltype(chars) const& _chars) noexcept
            : chars(_chars) {}
        constexpr charset(decltype(chars)&& _chars) noexcept
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
            return std::find(chars.cbegin(), chars.cend(), c) != chars.cend();
        }
    };

} // namespace webpp

#endif // CHARSET_H
