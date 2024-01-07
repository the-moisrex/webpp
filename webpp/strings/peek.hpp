// Created by moisrex on 1/7/24.

#ifndef WEBPP_STRINGS_PEAK_HPP
#define WEBPP_STRINGS_PEAK_HPP

#include "../std/iterator.hpp"

namespace webpp::ascii {

    /// Look ahead and see if the next values are the ones that we expect
    template <typename Iter, typename EIter = Iter, typename... ValT>
        requires(stl::convertible_to<ValT, typename stl::iterator_traits<Iter>::value_type> && ...)
    [[nodiscard]] static constexpr bool peek(Iter pos, EIter end, ValT... val) noexcept {
        if constexpr (sizeof...(ValT) == 1) {
            return pos != end && val == *pos;
        } else {
            // NOLINTNEXTLINE(*-inc-dec-in-conditions)
            return pos + sizeof...(ValT) < end && ((val == *pos++) && ...);
        }
    }

    /// Look ahead and see if the expected values are there, or we're at the end
    template <typename Iter, typename EIter = Iter, typename... ValT>
        requires(stl::convertible_to<ValT, typename stl::iterator_traits<Iter>::value_type> && ...)
    [[nodiscard]] static constexpr bool peek_or_end(Iter pos, EIter end, ValT... val) noexcept {
        if constexpr (sizeof...(ValT) == 1) {
            return pos == end || val == *pos;
        } else {
            // NOLINTNEXTLINE(*-inc-dec-in-conditions)
            return pos + sizeof...(ValT) < end && ((val == *pos++) && ...) || pos == end;
        }
    }

    /// Look ahead and see if the specified values are there or not
    /// This one doesn't check the boundaries
    template <typename Iter, typename... ValT>
        requires(stl::convertible_to<ValT, typename stl::iterator_traits<Iter>::value_type> && ...)
    [[nodiscard]] static constexpr bool peek(Iter pos, ValT... val) noexcept {
        if constexpr (sizeof...(ValT) == 1) {
            return val == *pos;
        } else {
            // NOLINTNEXTLINE(*-inc-dec-in-conditions)
            return ((val == *pos++) && ...);
        }
    }

    /// Look ahead and see if the next values are the ones that we expect, if it is, move "pos" there,
    /// otherwise, do nothing.
    template <typename Iter, typename EIter = Iter, typename... ValT>
        requires(stl::convertible_to<ValT, typename stl::iterator_traits<Iter>::value_type> && ...)
    [[nodiscard]] static constexpr bool inc_if(Iter& pos, EIter end, ValT... val) noexcept {
        if constexpr (sizeof...(ValT) == 1) {
            if (pos != end && val == *pos) {
                ++pos;
                return true;
            }
            return false;
        } else {
            auto tmp_pos = pos;
            // NOLINTNEXTLINE(*-inc-dec-in-conditions)
            if (pos + sizeof...(ValT) < end && ((val == *tmp_pos++) && ...)) {
                pos = tmp_pos;
                return true;
            }
            return false;
        }
    }
} // namespace webpp::ascii

#endif // WEBPP_STRINGS_PEAK_HPP
