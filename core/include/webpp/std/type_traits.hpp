// Created by moisrex on 9/27/20.

#ifndef WEBPP_TYPE_TRAITS_HPP
#define WEBPP_TYPE_TRAITS_HPP

#include "./std.hpp"
#include <type_traits>

namespace webpp::istl {

    template <typename T>
    struct lazy_type {
        template <bool Condition>
        struct evaluate {
            using type = T;
        };
    };

    template <template <typename...> typename Extractor, typename ...Args>
    struct lazy_sub_type {
        template <bool Condition>
        struct evaluate {
            using type = void;
        };

        template <>
        struct evaluate<true> {
            using type = Extractor<Args...>;
        };
    };

    template <template <typename...> typename T, typename ...Args>
    struct templated_lazy_type {
        template <bool Condition>
        struct evaluate {
            using type = void;
        };

        template <>
        struct evaluate<true> {
            using type = T<Args...>;
        };

    };

    template <typename T>
    concept LazyType = requires {
      typename T::template evaluated<true>::type;
      typename T::template evaluated<false>::type;
    };

    /**
     * Lazy Conditional is a class type that is designed to have the same usage as std::conditional but
     * the values are not going to be evaluated if the condition doesn't meet for that value.
     */
    template <bool Condition, LazyType T1, LazyType T2>
    struct lazy_conditional {
        using type = typename T1::template evaluated<Condition>::type;
    };

    template <bool Condition, LazyType T1, LazyType T2>
    using lazy_conditional_t = typename lazy_conditional<Condition, T1, T2>::type;

}

#endif // WEBPP_TYPE_TRAITS_HPP
