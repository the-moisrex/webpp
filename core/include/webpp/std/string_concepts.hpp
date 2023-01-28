// Created by moisrex on 8/9/20.

#ifndef WEBPP_STRING_CONCEPTS_HPP
#define WEBPP_STRING_CONCEPTS_HPP

#include "concepts.hpp"

#include <string>

namespace webpp::istl {


    // clang-format off
    template <typename X>
    concept CharTraits = requires {
        typename X::char_type;
        typename X::int_type;
        typename X::off_type;
        typename X::pos_type;
        typename X::state_type;
    } && requires(
            typename X::char_type c,
            typename X::char_type const* p,
            typename X::char_type* s,
            stl::size_t n,
            typename X::int_type e,
            typename X::char_type const& ch
    ) {
        requires CopyAssignable<typename X::state_type>;
        requires Destructible<typename X::state_type>;
        requires CopyConstructible<typename X::state_type>;
        requires DefaultConstructible<typename X::state_type>;

        { X::eq(c, c) }             -> stl::same_as<bool>;
        { X::lt(c, c) }             -> stl::same_as<bool>;
        { X::compare(p, p, n) }     -> stl::same_as<int>;
        { X::length(p) }            -> stl::same_as<stl::size_t>;
        { X::find(p, n, ch) }       -> stl::same_as<typename X::char_type const*>;
        { X::move(s, p, ch) }       -> stl::same_as<typename X::char_type*>;
        { X::copy(s, p, n) }        -> stl::same_as<typename X::char_type*>;
        { X::assign(s, n, c) }      -> stl::same_as<typename X::char_type*>;
        { X::not_eof(e) }           -> stl::same_as<typename X::int_type>;
        { X::to_char_type(e) }      -> stl::same_as<typename X::char_type>;
        { X::to_int_type(c) }       -> stl::same_as<typename X::int_type>;
        { X::eq_int_type(e, e) }    -> stl::same_as<bool>;
        { X::eof() }                -> stl::same_as<typename X::int_type>;
    };
    // clang-format on

    //    struct fake_char_traits_type {
    //
    //        ~fake_char_traits_type() {}
    //
    //        using char_type = char;
    //        using int_type  = int;
    //        using off_type  = void;
    //        using pos_type  = void;
    //        struct state_type {};
    //
    //        typedef char_type        c;
    //        typedef char_type const* p;
    //        typedef char_type*       s;
    //        typedef stl::size_t      n;
    //        typedef int_type         e;
    //        typedef char_type const& ch;
    //
    //        static constexpr bool eq(c, c) noexcept {
    //            return true;
    //        }
    //        static constexpr bool lt(c, c) noexcept {
    //            return true;
    //        }
    //        static constexpr int compare(p, p, n) noexcept {
    //            return 0;
    //        }
    //        static constexpr n length(p) noexcept {
    //            return 0;
    //        }
    //        static constexpr p find(p _p, n, ch) noexcept {
    //            return _p;
    //        }
    //        static constexpr s copy(s _s, p, ch) noexcept {
    //            return _s;
    //        }
    //        static constexpr s move(s _s, p, n) noexcept {
    //            return _s;
    //        }
    //        static constexpr s assign(s _s, n, c) noexcept {
    //            return _s;
    //        }
    //        static constexpr e not_eof(e) noexcept {
    //            return 0;
    //        }
    //        static constexpr c to_char_type(e _e) noexcept {
    //            return _e;
    //        }
    //        static constexpr e to_int_type(c _c) noexcept {
    //            return _c;
    //        }
    //        static constexpr bool eq_int_type(e, e) noexcept {
    //            return true;
    //        }
    //        static constexpr e eof() noexcept {
    //            return 0;
    //        }
    //    };

    template <typename T>
    concept CharType = stl::is_integral_v<stl::remove_cvref_t<T>>;


    namespace details {
        template <typename T>
        struct char_extractor {
            using type = typename T::value_type;
        };

        template <typename T>
        struct traits_extractor {
            using type = typename T::traits_type;
        };

        // separated this because of Clang error; clang gives errors if I use the requires clause directly
        template <typename T>
        static constexpr bool has_value_type =
          requires {
              typename stl::remove_cvref_t<T>::value_type;
              requires stl::integral<typename stl::remove_cvref_t<T>::value_type>;
          };


        template <typename T>
        struct allocator_type_extractor {
            using type = typename T::allocator_type;
        };

        template <typename T>
        static constexpr bool has_allocator_type =
          requires { typename stl::remove_cvref_t<T>::allocator_type; };
    } // namespace details

    /**
     * Get the underlying character type in a string/string view/c style string
     */
    template <typename T,
              typename BestGuess =
                stl::remove_cvref_t<stl::remove_all_extents_t<stl::remove_pointer_t<stl::decay_t<T>>>>>
    using char_type_of =
      lazy_conditional_t<details::has_value_type<T>,
                         templated_lazy_type<details::char_extractor, BestGuess>,
                         lazy_type<stl::conditional_t<CharType<BestGuess>, BestGuess, void>>>;

    /**
     * Get the underlying allocator_type
     */
    template <typename T, typename DefaultAllocator = stl::allocator<char_type_of<T>>>
    using allocator_type_of = lazy_conditional_t<
      details::has_allocator_type<T>,
      templated_lazy_type<details::allocator_type_extractor, stl::decay_t<stl::remove_cvref_t<T>>>,
      lazy_type<DefaultAllocator>>;


    namespace details {
        template <typename T>
        concept has_traits_type = requires { typename stl::remove_cvref_t<T>::traits_type; };
    } // namespace details

    template <typename T, typename Default = stl::char_traits<char_type_of<T>>>
    using char_traits_type_of =
      lazy_conditional_t<details::has_traits_type<T>,
                         templated_lazy_type<details::traits_extractor, stl::remove_cvref_t<T>>,
                         lazy_type<Default>>;



    template <typename T>
    concept UTF8 = sizeof(char_type_of<T>) == sizeof(char8_t);

    template <typename T>
    concept UTF16 = sizeof(char_type_of<T>) == sizeof(char16_t);

    template <typename T>
    concept UTF32 = sizeof(char_type_of<T>) == sizeof(char32_t);

    template <typename T>
    concept StringLiteral =
      istl::one_of<stl::remove_const_t<T>, char*, char16_t*, char8_t*, char32_t*, wchar_t*>;

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
