// Created by moisrex on 5/7/20.

#ifndef WEBPP_STD_CONCEPTS_H
#define WEBPP_STD_CONCEPTS_H

#include "./std.h"

#include <type_traits>

#if __has_include(<concepts>)
#    include <concepts>
#else
namespace webppstd {
    namespace detail {
        template <class T, class U>
        concept SameHelper = std::is_same_v<T, U>;
    }

    /* same_as */
    template <class T, class U>
    concept same_as = detail::SameHelper<T, U>&& detail::SameHelper<U, T>;

    /* derived_from */
    template <class Derived, class Base>
    concept derived_from = std::is_base_of_v<Base, Derived>&&
      std::is_convertible_v<const volatile Derived*, const volatile Base*>;

    /* convertible_to */
    template <class From, class To>
    concept convertible_to = std::is_convertible_v<From, To>&& requires(
      std::add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

    template <typename T>
    concept integral = std::is_integral_v<T>;

    template <typename T>
    concept move_constructible =
      std::constructible_from<T, T>&& std::convertible_to<T, T>;

    template <class T>

    concept copy_constructible = std::move_constructible<
      T>&& std::constructible_from<T, T&>&& std::convertible_to<T&, T>&&
      std::constructible_from<T, const T&>&& std::convertible_to<const T&, T>&&
        std::constructible_from<T, const T>&& std::convertible_to<const T, T>;

} // namespace webppstd
#endif

namespace webpp::stl {
    template <typename D>
    concept Destructible = requires(D u) {
        { u.~D() }
        noexcept;
    };

    template <typename T>
    concept MoveAssignable = stl::is_move_assignable_v<T>;

    template <typename T>
    concept CopyAssignable = stl::is_copy_assignable_v<T>;

    template <typename T>
    concept CopyConstructible = stl::is_copy_constructible_v<T>;

    template <typename T>
    concept DefaultConstructible = stl::is_default_constructible_v<T>;

    template <typename X>
    concept CharTraits = Destructible<typename X::state_type>&&
          CopyAssignable<typename X::state_type>&&
          CopyConstructible<typename X::state_type>&&
          DefaultConstructible<typename X::state_type>&& requires(
            typename X::char_type c, typename X::char_type const* p,
            typename X::char_type* s, stl::size_t n, typename X::int_type e,
            typename X::char_type const& ch) {
        typename X::char_type;
        typename X::int_type;
        typename X::off_type;
        typename X::pos_type;
        typename X::state_type;


        { X::eq(c, c) }
        ->stl::same_as<bool>;
        { X::lt(c, c) }
        ->stl::same_as<bool>;
        { X::compare(p, p, n) }
        ->stl::same_as<int>;
        { X::length(p) }
        ->stl::same_as<stl::size_t>;
        { X::find(p, n, ch) }
        ->stl::same_as<typename X::char_type const*>;
        { X::move(s, p, ch) }
        ->stl::same_as<typename X::char_type*>;
        { X::copy(s, p, n) }
        ->stl::same_as<typename X::char_type*>;
        { X::assign(s, n, c) }
        ->stl::same_as<typename X::char_type*>;
        { X::not_eof(e) }
        ->stl::same_as<typename X::int_type>;
        { X::to_char_type(e) }
        ->stl::same_as<typename X::char_type>;
        { X::to_int_type(c) }
        ->stl::same_as<typename X::int_type>;
        { X::eq_int_type(e, e) }
        ->stl::same_as<bool>;
        { X::eof() }
        ->stl::same_as<typename X::int_type>;
    };

    struct fake_char_traits {
        using char_type  = char;
        using int_type   = int;
        using off_type   = void;
        using pos_type   = void;
        using state_type = void;

        typedef char_type        c;
        typedef char_type const* p;
        typedef char_type*       s;
        typedef stl::size_t      n;
        typedef int_type         e;
        typedef char_type const& ch;

        static constexpr bool eq(c, c) noexcept {
            return true;
        }
        static constexpr bool lt(c, c) noexcept {
            return true;
        }
        static constexpr int compare(p, p, n) noexcept {
            return 0;
        }
        static constexpr n length(p) noexcept {
            return 0;
        }
        static constexpr p find(p _p, n, ch) noexcept {
            return _p;
        }
        static constexpr s move(s _s, p, ch) noexcept {
            return _s;
        }
        static constexpr s move(s _s, p, n) noexcept {
            return _s;
        }
        static constexpr s assign(s _s, n, c) noexcept {
            return _s;
        }
        static constexpr e not_eof(e) noexcept {
            return 0;
        }
        static constexpr c to_char_type(e _e) noexcept {
            return _e;
        }
        static constexpr e to_int_type(c _c) noexcept {
            return _c;
        }
        static constexpr bool eq_int_type(e, e) noexcept {
            return true;
        }
        static constexpr e eof() noexcept {
            return 0;
        }
    };


} // namespace webpp::stl

#endif // WEBPP_STD_CONCEPTS_H
