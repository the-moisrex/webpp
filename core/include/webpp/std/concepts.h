// Created by moisrex on 5/7/20.

#ifndef WEBPP_CONCEPTS_H
#define WEBPP_CONCEPTS_H

#include <type_traits>

#if __has_include(<concepts>)
#    include <concepts>
#else
namespace std {
    namespace detail {
        template <class T, class U>
        concept SameHelper = ::std::is_same_v<T, U>;
    }

    /* same_as */
    template <class T, class U>
    concept same_as = detail::SameHelper<T, U>&& detail::SameHelper<U, T>;

    /* derived_from */
    template <class Derived, class Base>
    concept derived_from =
      ::std::is_base_of_v<Base, Derived>&& ::std::is_convertible_v<
        const volatile Derived*, const volatile Base*>;

    /* convertible_to */
    template <class From, class To>
    concept convertible_to = ::std::is_convertible_v<From, To>&& requires(
      ::std::add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

} // namespace std
#endif

namespace stl {
    template <typename D>
    concept Destructible = requires(D u) {
        { u.~D() }
        noexcept;
    };

    template <typename T>
    concept MoveAssignable = ::std::is_move_assignable_v<T>;

    template <typename T>
    concept CopyAssignable = ::std::is_copy_assignable_v<T>;

    template <typename T>
    concept CopyConstructible = ::std::is_copy_constructible_v<T>;

    template <typename T>
    concept DefaultConstructible = ::std::is_default_constructible_v<T>;

    template <typename X>
    concept CharTraits = requires(X ct, typename X::char_type c,
                                  typename X::char_type const* p,
                                  typename X::char_type const* s,
                                  ::std::size_t n, typename X::int_type e,
                                  typename X::char_type const& ch) {
        typename X::char_type;
        typename X::int_type;
        typename X::off_type;
        typename X::pos_type;

        typename X::state_type;
        Destructible<typename X::state_type>;
        CopyAssignable<typename X::state_type>;
        CopyConstructible<typename X::state_type>;
        DefaultConstructible<typename X::state_type>;

        { X::eq(c, c) }
        ->::std::same_as<bool>;
        { X::lt(c, c) }
        ->::std::same_as<bool>;
        { X::compare(p, p, n) }
        ->::std::same_as<int>;
        { X::length(p) }
        ->::std::same_as<::std::size_t>;
        { X::find(p, n, ch) }
        ->::std::same_as<typename X::char_type const*>;
        { X::move(s, n, ch) }
        ->::std::same_as<typename X::char_type*>;
        { X::copy(s, p, n) }
        ->::std::same_as<typename X::char_type*>;
        { X::assing(s, n, c) }
        ->::std::same_as<typename X::char_type*>;
        { X::not_eof(e) }
        ->::std::same_as<typename X::int_type>;
        { X::to_char_type(e) }
        ->::std::same_as<typename X::char_type>;
        { X::to_int_type(c) }
        ->::std::same_as<typename X::int_type>;
        { X::eq_int_type(e, e) }
        ->::std::same_as<bool>;
        { X::eof() }
        ->::std::same_as<typename X::int_type>;
    };


} // namespace stl

#endif // WEBPP_CONCEPTS_H
