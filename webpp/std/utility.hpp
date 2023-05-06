// Created by moisrex on 1/17/21.

#ifndef WEBPP_UTILITY_HPP
#define WEBPP_UTILITY_HPP

#include "std.hpp"

#include <utility>

namespace webpp::istl {


    /**
     *  @brief  Convert a rvalue or lvalue to a value
     *  This has the same effect of auto(...) in C++23
     *  This implementation is very similar to the std::move but does "almost" the opposite
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




    namespace details {
        template <stl::size_t Start, stl::size_t End, stl::size_t... I>
        struct make_index_range_maker : make_index_range_maker<Start + 1, End, I..., Start> {};

        // last element is ignored
        template <stl::size_t End, stl::size_t... I>
        struct make_index_range_maker<End, End, I...> {
            using type = stl::index_sequence<I...>;
        };

        // Start and End are the same, it's an empty range
        template <stl::size_t Start>
        struct make_index_range_maker<Start, Start> {
            using type = stl::index_sequence<>;
        };
    } // namespace details

    template <stl::size_t Start, stl::size_t End>
    using make_index_range = typename details::make_index_range_maker<Start, End>::type;


} // namespace webpp::istl

namespace webpp::stl {

    // polyfill for C++20 (to_underlying is a C++23 feature)
#ifndef __cpp_lib_to_underlying
    /// Convert an object of enumeration type to its underlying type.
    template <typename T>
    [[nodiscard]] constexpr underlying_type_t<T> to_underlying(T value) noexcept {
        return static_cast<underlying_type_t<T>>(value);
    }
#endif

} // namespace webpp::stl

#endif // WEBPP_UTILITY_HPP
