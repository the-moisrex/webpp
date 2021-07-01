// Created by moisrex on 5/7/20.

#ifndef WEBPP_STD_CONCEPTS_H
#define WEBPP_STD_CONCEPTS_H

#include "./std.hpp"
#include "./type_traits.hpp"

#include <memory> // for Allocator

#if __has_include(<concepts>) && __has_cpp_attribute(__cpp_concepts)
#    include <concepts>
#else
namespace webpp::stl {
    namespace detail {
        template <class T, class U>
        concept SameHelper = stl::is_same_v<T, U>;
    }

    /* same_as */
    template <class T, class U>
    concept same_as = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

    /* derived_from */
    template <class Derived, class Base>
    concept derived_from = stl::is_base_of_v<Base, Derived> &&
      stl::is_convertible_v<const volatile Derived*, const volatile Base*>;

    /* convertible_to */
    template <class From, class To>
    concept convertible_to = stl::is_convertible_v<From, To> &&
      requires(stl::add_rvalue_reference_t<From> (&f)()) {
        static_cast<To>(f());
    };

    template <typename T>
    concept integral = stl::is_integral_v<T>;


    template <typename T>
    concept destructible = stl::is_nothrow_destructible_v<T>;

    template <typename T, typename... Args>
    concept constructible_from = stl::destructible<T> && stl::is_constructible_v<T, Args...>;

    template <typename T>
    concept move_constructible = stl::constructible_from<T, T> && stl::convertible_to<T, T>;

    template <class T>
    concept copy_constructible =
      stl::move_constructible<T> && stl::constructible_from<T, T&> && stl::convertible_to<T&, T> &&
      stl::constructible_from<T, const T&> && stl::convertible_to<const T&, T> &&
      stl::constructible_from<T, const T> && stl::convertible_to<const T, T>;

    template <class T>
    concept default_initializable = stl::constructible_from<T> && requires {
        T{};
    } && requires {
        ::new (static_cast<void*>(nullptr)) T;
    };


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
