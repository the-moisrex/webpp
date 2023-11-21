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

// NOLINTBEGIN(*-macro-usage)

namespace webpp::istl {
    template <typename D>
    concept Destructible = requires(D obj) {
        { obj.~D() } noexcept;
    };

    template <typename T>
    concept MoveAssignable = stl::is_move_assignable_v<T>;

    template <typename T>
    concept CopyAssignable = stl::is_copy_assignable_v<T>;

    template <typename T>
    concept CopyConstructible = stl::is_copy_constructible_v<T>;

    template <typename T>
    concept DefaultConstructible = stl::is_default_constructible_v<T>;


    /**
     * This concept always returns true. Use it for:
     *   - for explicitly saying that it does capture all
     *   - instead of commenting the whole in-development concepts, fill them with this temporarily.
     */
    template <typename T>
    concept All = true;


    /**
     * This concept is always false
     */
    template <typename T>
    concept None = false;



    namespace details {
        template <typename T>
        struct returnable {
            T operator()() {}
        };


        template <auto Constraint>
        struct requires_arg_op {
            template <typename T>
                requires(Constraint.template operator()<T>())
            explicit operator T() {}
        };


        template <auto Constraint>
        struct requires_arg_op_cvref {
            template <typename T>
                requires(Constraint.template operator()<T>())
            explicit operator T() {}

            template <typename T>
                requires(Constraint.template operator()<T&>())
            explicit operator T&() {}
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
     * Use std::remove_cvref_t to clean up the type first and then run it through the "requires_arg"
     */
#define requires_arg_cvref(...)                                                                        \
    webpp::istl::details::requires_arg_op_cvref<[]<typename RequiresT> {                               \
        return (                                                                                       \
          requires {                                                                                   \
              { webpp::istl::details::returnable<RequiresT>()() } -> __VA_ARGS__;                      \
          } ||                                                                                         \
          requires {                                                                                   \
              { webpp::istl::details::returnable<stl::remove_cvref_t<RequiresT>>()() } -> __VA_ARGS__; \
          });                                                                                          \
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



    namespace details {
        template <auto Concept>
        struct typify_type {
            template <typename T>
            struct type {
                static constexpr bool value = Concept.template operator()<T>();
            };
        };
    } // namespace details

    /**
     * This lets you convert a concept to a type with a "value" of bool type.
     * It converts this:
     *
     *   template <typename T>
     *   concept test_concept = ...;
     *
     * To this:
     *
     *   template <typename T>
     *   struct test {
     *     static constexpr bool value = test_concept<T>;
     *   };
     *
     * Usage:
     *
     *   template <typename T>
     *   concept tuple_of_tests = istl::TupleOf<typify(test_concept), T>;
     */
#define typify(...)                                            \
    webpp::istl::details::typify_type<[]<typename RequiresT> { \
        return __VA_ARGS__<RequiresT>;                         \
    }>::template type



    template <typename T>
    concept arithmetic = stl::is_arithmetic_v<T>;


    namespace details {

        template <typename First, typename... T>
        struct is_one_of {
            static constexpr bool value = (stl::same_as<First, T> || ...);
        };

    }; // namespace details

    /**
     * Same as "same_as" but check against multiple types.
     * The last type that you specify will be the one that gets checked against the other type not the first
     * type and the reason is that this way you can use this concept in the requires clauses.
     *
     * @code
     *   requires (T val) {
     *       { val.get() } -> one_of<int, double, float>;
     *   }
     * @endcode
     */
    template <typename... T>
    concept one_of = (sizeof...(T) > 1) && (last_type<T...>::template rotate<details::is_one_of>::value);


    // The simpler version of the above code, with the difference that can't be used as concept for return
    // types
    template <typename T, typename... E>
    concept part_of = (stl::same_as<T, E> || ...);




    namespace details {
        // failure condition
        template <typename... T>
        struct is_same_all : stl::false_type {};

        // success termination condition
        template <typename T1>
        struct is_same_all<T1, T1> : stl::true_type {};

        // append from right tuple to left tuple
        template <typename... TX, typename T2, typename... T>
            requires(sizeof...(TX) < sizeof...(T) + 1)
        struct is_same_all<is_same_all<TX...>, is_same_all<T2, T...>>
          : is_same_all<is_same_all<TX..., T2>, is_same_all<T...>> {};
    } // namespace details

    /**
     * Check if the first half of types are the same as the second half of types.
     * This is the same thing as "std::same_as" but accepts more templates
     * Possible Usage:
     * @code
     *   template <typename ...T>
     *   struct A {
     *
     *      template <typename ...U>
     *          requires(same_as_all<T..., U...>)
     *      void call(U&&...); // You can do perfect forwarding with U
     *   };
     * @endcode
     */
    template <typename... T>
    concept same_as_all = details::is_same_all<details::is_same_all<>, details::is_same_all<T...>>::value;

    /**
     * Same as "same_as_all" except it removes the cvref qualifications first.
     */
    template <typename... T>
    concept cvref_as = same_as_all<stl::remove_cvref_t<T>...>;

} // namespace webpp::istl
// NOLINTEND(*-macro-usage)

#endif // WEBPP_STD_CONCEPTS_H
