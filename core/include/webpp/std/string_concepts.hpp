// Created by moisrex on 8/9/20.

#ifndef WEBPP_STRING_CONCEPTS_HPP
#define WEBPP_STRING_CONCEPTS_HPP

#include "./concepts.hpp"

namespace webpp::istl {


    template <typename X>
    concept CharTraits = Destructible<typename X::state_type>&& CopyAssignable<typename X::state_type>&&
      CopyConstructible<typename X::state_type>&& DefaultConstructible<typename X::state_type>&& requires(
        typename X::char_type c, typename X::char_type const* p, typename X::char_type* s, stl::size_t n,
        typename X::int_type e, typename X::char_type const& ch) {
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

    struct fake_char_traits_type {

        ~fake_char_traits_type() {}

        using char_type = char;
        using int_type  = int;
        using off_type  = void;
        using pos_type  = void;
        struct state_type {};

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
        static constexpr s copy(s _s, p, ch) noexcept {
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

    template <typename T>
    concept CharType = stl::is_integral_v<stl::decay_t<T>>;





} // namespace webpp::istl

namespace webpp {

    /**
     * Automatically choose a string type based on mutability requested
     */
    template <typename TraitsType, bool Mutable>
    using auto_string_type =
      stl::conditional_t<Mutable, typename TraitsType::string_type, typename TraitsType::string_view_type>;

} // namespace webpp

#endif // WEBPP_STRING_CONCEPTS_HPP
