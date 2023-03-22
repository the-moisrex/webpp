// Created by moisrex on 1/17/21.

#ifndef WEBPP_UTILITY_HPP
#define WEBPP_UTILITY_HPP

#include "std.hpp"

#include <utility>

namespace webpp::istl {


    /**
     *  @brief  Convert a rvalue or lvalue to a value
     *  This has the same effect of auto(...) in C++23
     *  This implementation is very similar to the std::move but does the opposite
     *  @param  t  A thing of arbitrary type.
     *  @return The parameter cast to a non-reference value.
     */
    template <typename T>
    [[nodiscard]] constexpr stl::remove_reference_t<T> deref(T&& t) noexcept {
        return static_cast<stl::remove_reference_t<T>>(t);
    }


    /**
     * The opposite of std::move; it's convert rvalue references into lvalue references.
     */
    template <typename T>
    [[nodiscard]] constexpr stl::remove_cvref_t<T>& unmove(T&& t) noexcept {
        return static_cast<stl::remove_cvref_t<T>&>(t);
    }

} // namespace webpp::istl

#endif // WEBPP_UTILITY_HPP
