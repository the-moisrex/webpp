// Created by moisrex on 5/7/20.

#ifndef WEBPP_STD_CONCEPTS_H
#define WEBPP_STD_CONCEPTS_H

#include "std.hpp"
#include "type_traits.hpp"

#if __has_include(<concepts>) && __cpp_lib_concepts
#    include <concepts>
#else
#    include "./concepts_impl.hpp"
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



    namespace details {
        template <typename T>
        struct returnable {
            T operator()() {}
        };

        template <auto Constraint>
        struct requires_arg_op {
            template <typename T>
                requires(Constraint.template operator()<T>())
            operator T() {}
        };
    } // namespace details

    /**
     * Require an input argument inside concepts
     *
     * Possible usage:
     *
     *   template <typename T>
     *   concept fooable =
     *       requires(T t) {
     *           t.foo(
     *               requires_arg(std::integral),
     *               requires_arg(std::same_as<short>)
     *           );
     *       };
     */
#define requires_arg(...)                                                       \
    webpp::istl::details::requires_arg_op<[]<typename RequiresT> {              \
        return (requires {                                                      \
            { webpp::istl::details::returnable<RequiresT>()() } -> __VA_ARGS__; \
        });                                                                     \
    }> {}

    /**
     * This is essentially the same thing; but the difference is you can add more custom constraints:
     *
     * Possible usage:
     *
     *   template <typename T>
     *   concept fooable =
     *       requires(T t) {
     *           t.foo(
     *               satisfy_arg(std::integral<_> && std::is_lvalue_reference_v<_>),
     *               satisfy_arg(std::same_as<short, _>)
     *           );
     *       };
     */
#define satisfy_arg(...)                                   \
    webpp::istl::details::requires_arg_op<[]<typename _> { \
        return __VA_ARGS__;                                \
    }> {}

} // namespace webpp::istl

#endif // WEBPP_STD_CONCEPTS_H
