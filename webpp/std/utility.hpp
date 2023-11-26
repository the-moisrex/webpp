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
        template <typename IntegerT, IntegerT Start, IntegerT End, IntegerT... I>
        struct integer_range_inclusive_maker
          : integer_range_inclusive_maker<IntegerT, End >= Start ? Start + 1 : Start - 1, End, I..., Start> {
        };

        // last element is not ignored
        template <typename IntegerT, IntegerT End, IntegerT... I>
        struct integer_range_inclusive_maker<IntegerT, End, End, I...> {
            using type = stl::index_sequence<I..., End>;
        };

        // Start and End are the same, it'll contain the Start itself
        template <typename IntegerT, IntegerT Start>
        struct integer_range_inclusive_maker<IntegerT, Start, Start> {
            using type = stl::index_sequence<Start>;
        };

        /////////////////// Non inclusive version:


        template <typename IntegerT, IntegerT Start, IntegerT End, IntegerT... I>
        struct integer_range_maker
          : integer_range_maker<IntegerT, End >= Start ? Start + 1 : Start - 1, End, I..., Start> {};

        // last element is not ignored
        template <typename IntegerT, IntegerT End, IntegerT... I>
        struct integer_range_maker<IntegerT, End, End, I...> {
            using type = stl::index_sequence<I...>;
        };

        // Start and End are the same, it'll be empty
        template <typename IntegerT, IntegerT Start>
        struct integer_range_maker<IntegerT, Start, Start> {
            using type = stl::index_sequence<>;
        };


    } // namespace details

    // Make integer range inclusively [Start, End].
    template <typename IntegerT, IntegerT Start, IntegerT End>
    using make_integer_inclusive_range =
      typename details::integer_range_inclusive_maker<IntegerT, Start, End>::type;

    // Make index range inclusively [Start, End].
    template <stl::size_t Start, stl::size_t End>
    using make_index_inclusive_range =
      typename details::integer_range_inclusive_maker<stl::size_t, Start, End>::type;

    // Make integer range non-inclusively [Start, End).
    template <typename IntegerT, IntegerT Start, IntegerT End>
    using make_integer_range = typename details::integer_range_maker<IntegerT, Start, End>::type;

    // Make index range non-inclusively [Start, End).
    template <stl::size_t Start, stl::size_t End>
    using make_index_range = typename details::integer_range_maker<stl::size_t, Start, End>::type;


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

    // polyfill for C++20 (forward_like is a C++23 feature)
#ifndef __cpp_lib_forward_like
    template <class T, class U>
    [[nodiscard]] constexpr auto&& forward_like(U&& x) noexcept {
        constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
        if constexpr (std::is_lvalue_reference_v<T&&>) {
            if constexpr (is_adding_const) {
                return std::as_const(x);
            } else {
                return static_cast<U&>(x);
            }
        } else {
            if constexpr (is_adding_const) {
                return std::move(std::as_const(x));
            } else {
                return std::move(x);
            }
        }
    }
#endif

    // polyfill for C++23 feature named unreachable
#ifndef __cpp_lib_unreachable
    [[noreturn]] inline void unreachable() {
#    ifdef __GNUC__         // GCC, Clang, ICC
        __builtin_unreachable();
#    elif defined(_MSC_VER) // MSVC
        __assume(false);
#    endif
    }
#endif                      // __cpp_lib_unreachable

} // namespace webpp::stl

#endif // WEBPP_UTILITY_HPP
