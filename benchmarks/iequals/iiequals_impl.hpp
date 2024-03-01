//
// Created by moisrex on 2/29/24.
//

#ifndef IIEQUALS_IMPL_HPP
#define IIEQUALS_IMPL_HPP

#include <iterator>
#include <string_view>

namespace test {

    template <typename CharT>
    [[nodiscard]] constexpr CharT to_lower_copy(CharT inp_char) noexcept {
        constexpr auto diff = static_cast<CharT>('a' - 'A');
        return inp_char >= static_cast<CharT>('A') && inp_char <= static_cast<CharT>('Z')
                 ? inp_char + diff
                 : inp_char;
    }

    template <bool IgnoreSpecialCharacters = true, typename LT, typename RT>
    [[nodiscard]] bool iiequals(LT&& lhs, RT&& rhs) noexcept {
        auto isSpecialChar = [](auto const& ch) noexcept {
            return (ch == '\t' || ch == '\r' || ch == '\n');
        };

        auto isEqualChars = [](auto const& ch1, auto const& ch2) noexcept {
            return to_lower_copy(ch1) == to_lower_copy(ch2);
        };

        auto const chunkSize = 64; // Adjust the chunk size as needed

        auto lhsIt = std::begin(lhs);
        auto rhsIt = std::begin(rhs);

        while (lhsIt != std::end(lhs) && rhsIt != std::end(rhs)) {
            // Process the strings in chunks
            for (int i = 0; i < chunkSize; ++i) {
                if (lhsIt == std::end(lhs) || rhsIt == std::end(rhs)) {
                    break;
                }

                if (isSpecialChar(*lhsIt)) {
                    ++lhsIt;
                    continue;
                }

                if (isSpecialChar(*rhsIt)) {
                    ++rhsIt;
                    continue;
                }

                if (!isEqualChars(*lhsIt, *rhsIt)) {
                    return false;
                }

                ++lhsIt;
                ++rhsIt;
            }

            // Skip over special characters in the remaining chunk
            while (lhsIt != std::end(lhs) && isSpecialChar(*lhsIt)) {
                ++lhsIt;
            }

            while (rhsIt != std::end(rhs) && isSpecialChar(*rhsIt)) {
                ++rhsIt;
            }
        }

        // If both strings have been traversed completely, they are equal.
        // Otherwise, if there are remaining characters in either string,
        // they are not equal.
        return (lhsIt == std::end(lhs) && rhsIt == std::end(rhs));
    }

} // namespace test

namespace test2 {

    template <bool IgnoreSpecialCharacters = true, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals(LT&& lhs, RT&& rhs) noexcept {
        auto isSpecialChar = [](auto const& ch) noexcept {
            return (ch == '\t' || ch == '\r' || ch == '\n');
        };

        auto isEqualChars = [](auto const& ch1, auto const& ch2) noexcept {
            using test::to_lower_copy;
            return to_lower_copy(ch1) == to_lower_copy(ch2);
        };

        auto lhsIt = std::begin(lhs);
        auto rhsIt = std::begin(rhs);

        while (lhsIt != std::end(lhs) && rhsIt != std::end(rhs)) {
            if (isSpecialChar(*lhsIt)) {
                ++lhsIt;
                continue;
            }

            if (isSpecialChar(*rhsIt)) {
                ++rhsIt;
                continue;
            }

            if (!isEqualChars(*lhsIt, *rhsIt)) {
                return false;
            }

            ++lhsIt;
            ++rhsIt;
        }

        // If both strings have been traversed completely, they are equal.
        // Otherwise, if there are remaining characters in either string,
        // they are not equal.
        return (lhsIt == std::end(lhs) && rhsIt == std::end(rhs));
    }
} // namespace test2

namespace test3 {

    template <bool IgnoreSpecialCharacters = true, typename LT, typename RT>
    [[nodiscard]] static constexpr bool iiequals(LT&& lhs, RT&& rhs) noexcept {
        auto lhsIt = std::begin(lhs);
        auto rhsIt = std::begin(rhs);

        constexpr auto isEqualChars = [](auto const& ch1, auto const& ch2) noexcept {
            using test::to_lower_copy;
            return to_lower_copy(ch1) == to_lower_copy(ch2);
        };

        while (lhsIt != std::end(lhs) && rhsIt != std::end(rhs)) {
            if (isEqualChars(*lhsIt, *rhsIt)) {
                ++lhsIt;
                ++rhsIt;
                continue;
            }

            if (*lhsIt == '\t' || *lhsIt == '\r' || *lhsIt == '\n') {
                ++lhsIt;
                continue;
            }

            if (*rhsIt == '\t' || *rhsIt == '\r' || *rhsIt == '\n') {
                ++rhsIt;
                continue;
            }
            return false;
        }

        // If both strings have been traversed completely, they are equal.
        // Otherwise, if there are remaining characters in either string,
        // they are not equal.
        return lhsIt == std::end(lhs) && rhsIt == std::end(rhs);
    }
} // namespace test3

#endif // IIEQUALS_IMPL_HPP
