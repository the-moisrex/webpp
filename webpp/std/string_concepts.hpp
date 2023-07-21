// Created by moisrex on 8/9/20.

#ifndef WEBPP_STD_STRING_CONCEPTS_HPP
#define WEBPP_STD_STRING_CONCEPTS_HPP

#include "concepts.hpp"

#include <string>

namespace webpp::istl {


    template <typename X>
    concept CharTraits = requires {
                             typename X::char_type;
                             typename X::int_type;
                             typename X::off_type;
                             typename X::pos_type;
                             typename X::state_type;
                             requires requires(typename X::char_type        c,
                                               typename X::char_type const* p,
                                               typename X::char_type*       s,
                                               stl::size_t                  n,
                                               typename X::int_type         e,
                                               typename X::char_type const& ch) {
                                          requires CopyAssignable<typename X::state_type>;
                                          requires Destructible<typename X::state_type>;
                                          requires CopyConstructible<typename X::state_type>;
                                          requires DefaultConstructible<typename X::state_type>;

                                          { X::eq(c, c) } -> stl::same_as<bool>;
                                          { X::lt(c, c) } -> stl::same_as<bool>;
                                          { X::compare(p, p, n) } -> stl::same_as<int>;
                                          { X::length(p) } -> stl::same_as<stl::size_t>;
                                          { X::find(p, n, ch) } -> stl::same_as<typename X::char_type const*>;
                                          { X::move(s, p, ch) } -> stl::same_as<typename X::char_type*>;
                                          { X::copy(s, p, n) } -> stl::same_as<typename X::char_type*>;
                                          { X::assign(s, n, c) } -> stl::same_as<typename X::char_type*>;
                                          { X::not_eof(e) } -> stl::same_as<typename X::int_type>;
                                          { X::to_char_type(e) } -> stl::same_as<typename X::char_type>;
                                          { X::to_int_type(c) } -> stl::same_as<typename X::int_type>;
                                          { X::eq_int_type(e, e) } -> stl::same_as<bool>;
                                          { X::eof() } -> stl::same_as<typename X::int_type>;
                                      };
                         };


    template <typename T>
    concept SingleCharacter = part_of<T, unsigned char, char, wchar_t, char16_t, char8_t, char32_t>;

    template <typename T>
    concept CharType = istl::SingleCharacter<stl::remove_cvref_t<T>>;

    namespace details {
        template <typename T>
        struct char_type_of {
            using type = void;
        };

        template <typename T>
            requires requires {
                         typename T::value_type;
                         requires stl::is_trivial_v<typename T::value_type>;
                         requires stl::is_standard_layout_v<typename T::value_type>;
                     }
        struct char_type_of<T> {
            using type = typename T::value_type;
        };

        template <SingleCharacter T>
        struct char_type_of<T> {
            using type = T;
        };


        template <typename T>
        struct traits_extractor {
            using type = typename T::traits_type;
        };


        template <typename T>
        struct allocator_type_extractor {
            using type = typename T::allocator_type;
        };

        template <typename T>
        concept has_allocator_type = requires { typename stl::remove_cvref_t<T>::allocator_type; };
    } // namespace details

    /**
     * Get the underlying character type in a string/string view/c style string
     */
    template <typename T>
    struct char_type_of : details::char_type_of<stl::decay_t<stl::remove_pointer_t<stl::decay_t<T>>>> {};

    template <typename T>
    using char_type_of_t = typename char_type_of<T>::type;


    /**
     * Get the underlying allocator_type
     */
    template <typename T, typename DefaultAllocator = stl::allocator<char_type_of_t<T>>>
    using allocator_type_of = lazy_conditional_t<
      details::has_allocator_type<T>,
      templated_lazy_type<details::allocator_type_extractor, stl::decay_t<stl::remove_cvref_t<T>>>,
      lazy_type<DefaultAllocator>>;


    namespace details {
        template <typename T>
        concept has_traits_type = requires { typename stl::remove_cvref_t<T>::traits_type; };
    } // namespace details

    template <typename T, typename Default = stl::char_traits<char_type_of_t<T>>>
    using char_traits_type_of =
      lazy_conditional_t<details::has_traits_type<T>,
                         templated_lazy_type<details::traits_extractor, stl::remove_cvref_t<T>>,
                         lazy_type<Default>>;

    template <typename T>
    using char_type_of_t_string_literals =
      stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_all_extents_t<stl::remove_cvref_t<T>>>>;


    template <typename T>
    concept UTF8 = sizeof(char_type_of_t<T>) == sizeof(char8_t);

    template <typename T>
    concept UTF16 = sizeof(char_type_of_t<T>) == sizeof(char16_t);

    template <typename T>
    concept UTF32 = sizeof(char_type_of_t<T>) == sizeof(char32_t);

    template <typename T>
    concept StringLiteral = (!stl::same_as<char_type_of_t_string_literals<T>, stl::remove_cvref_t<T>>) &&
                            CharType<char_type_of_t_string_literals<T>>;

} // namespace webpp::istl

#endif // WEBPP_STD_STRING_CONCEPTS_HPP
