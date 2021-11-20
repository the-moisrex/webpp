// Created by moisrex on 5/7/20.

#ifndef WEBPP_STD_CONCEPTS_H
#define WEBPP_STD_CONCEPTS_H

#include "./std.hpp"
#include "./type_traits.hpp"

#include <memory> // for Allocator

#if __has_include(<concepts>) && __cpp_lib_concepts
#    include <concepts>
#else
#    include <concepts>
namespace webpp::stl {
    namespace detail {
        template <class T, class U>
        concept SameHelper = is_same_v<T, U>;
    }

    /* integral */
    template <class T>
    concept integral = is_integral_v<T>;

    /* signed_integral */
    template <class T>
    concept signed_integral = integral<T> && is_signed_v<T>;

    /* same_as */
    template <class T, class U>
    concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

    /* derived_from */
    template <class Derived, class Base>
    concept derived_from =
      is_base_of_v<Base, Derived> && is_convertible_v<const volatile Derived*, const volatile Base*>;

    /* convertible_to */
    template <class From, class To>
    concept convertible_to = is_convertible_v<From, To> && requires {
        static_cast<To>(declval<From>());
    };


    template <typename T>
    concept destructible = is_nothrow_destructible_v<T>;

    template <typename T, typename... Args>
    concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

    template <typename T>
    concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

    template <class T>
    concept copy_constructible = move_constructible<T> && constructible_from<T, T&> &&
      convertible_to<T&, T> && constructible_from<T, const T&> && convertible_to<const T&, T> &&
      constructible_from<T, const T> && convertible_to<const T, T>;

    template <class T>
    concept default_initializable = constructible_from<T> && requires {
        T{};
    } && requires {
        ::new (static_cast<void*>(nullptr)) T;
    };


    template <class T, class U>

    concept common_reference_with = same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
      convertible_to<T, common_reference_t<T, U>> && convertible_to<U, common_reference_t<T, U>>;

    namespace details {
        template <class B>
        concept boolean_testable_impl = convertible_to<B, bool>;

        template <class B>
        concept boolean_testable = boolean_testable_impl<B> && requires(B && b) {
            { !forward<B>(b) } -> boolean_testable_impl;
        };


        template <class T, class U>
        concept WeaklyEqualityComparableWith =
          requires(const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
            { t == u } -> boolean_testable;
            { t != u } -> boolean_testable;
            { u == t } -> boolean_testable;
            { u != t } -> boolean_testable;
        };


    } // namespace details

    template <class T>
    concept equality_comparable = details::WeaklyEqualityComparableWith<T, T>;

    template <class T, class U>
    concept equality_comparable_with =
      equality_comparable<T> && equality_comparable<U> && common_reference_with < const remove_reference_t<T>
    &, const remove_reference_t<U>& >
         &&equality_comparable<
           common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>>&&
           details::WeaklyEqualityComparableWith<T, U>;

    template <class T>
    concept semiregular = copyable<T> && default_initializable<T>;

    template <class T>
    concept regular = semiregular<T> && equality_comparable<T>;





} // namespace webpp::stl
#endif

namespace webpp::istl {
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


    template <typename A>
    concept Allocator =
      !stl::integral<A> && stl::default_initializable<A> && stl::copy_constructible<A> && requires(A alloc) {
        {alloc.allocate(1)};
        {alloc.deallocate(nullptr, 1)};
        {alloc == alloc};
        {alloc != alloc};
        typename A::value_type;
    };

    /**
     * This concept always returns true. Use it for:
     *   - for explicitly saying that it does capture all
     *   - instead of commenting the whole in-development concepts, fill them with this temporarily.
     */
    template <typename T>
    concept All = requires {
        typename T::yes;
    }
    || !requires {
        typename T::yes;
    };


    /**
     * This concept is always false
     */
    template <typename T>
    concept None = !All<T>;

} // namespace webpp::istl

#endif // WEBPP_STD_CONCEPTS_H
