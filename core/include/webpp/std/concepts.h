// Created by moisrex on 5/7/20.

#ifndef WEBPP_CONCEPTS_H
#define WEBPP_CONCEPTS_H

#include <type_traits>

template <typename D>
concept Destructible = requires(D u) {
    { u.~D() }
    noexcept;
};

template <typename T>
concept MoveAssignable = std::is_move_assignable_v<T>;

template <typename T>
concept CopyAssignable = std::is_copy_assignable_v<T>;

template <typename T>
concept CopyConstructible = std::is_copy_constructible_v<T>;

template <typename T>
concept DefaultConstructible = std::is_default_constructible_v<T>;

template <typename X>
concept CharTraits = requires(X ct, typename X::char_type c,
                              typename X::char_type const* p,
                              typename X::char_type const* s, std::size_t n,
                              typename X::int_type         e,
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
    ->std::is_same<bool>;
    { X::lt(c, c) }
    ->std::is_same<bool>;
    { X::compare(p, p, n) }
    ->std::is_same<int>;
    { X::length(p) }
    ->std::is_same<std::size_t>;
    { X::find(p, n, ch) }
    ->std::is_same<typename X::char_type const*>;
    { X::move(s, n, ch) }
    ->std::is_same<typename X::char_type*>;
    { X::copy(s, p, n) }
    ->std::is_same<typename X::char_type*>;
    { X::assing(s, n, c) }
    ->std::is_same<typename X::char_type*>;
    { X::not_eof(e) }
    ->std::is_same<typename X::int_type>;
    { X::to_char_type(e) }
    ->std::is_same<typename X::char_type>;
    { X::to_int_type(c) }
    ->std::is_same<typename X::int_type>;
    { X::eq_int_type(e, e) }
    ->std::is_same<bool>;
    { X::eof() }
    ->std::is_same<typename X::int_type>;
};


#endif // WEBPP_CONCEPTS_H
