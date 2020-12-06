// Created by moisrex on 5/11/20.

#ifndef WEBPP_TUPLE_H
#define WEBPP_TUPLE_H

#include "./std.hpp"

#include <tuple>
#include <type_traits>

namespace webpp::istl {

    template <typename T>
    struct is_tuple : public stl::false_type {};

    template <typename T>
    struct is_tuple<T const> : public is_tuple<T> {};

    template <typename T>
    struct is_tuple<T volatile> : public is_tuple<T> {};

    template <typename T>
    struct is_tuple<T const volatile> : public is_tuple<T> {};

    template <typename... Types>
    struct is_tuple<stl::tuple<Types...>> : public stl::true_type {};

    template <typename T>
    concept Tuple = is_tuple<T>::value;

    //    template <template <typename> typename Concept, template <typename...> typename Tup, typename... T>
    //    concept TupleOf = Tuple<Tup<T...>>&& requires {
    //        (Concept<T>::value && ...);
    //    };

    template <template <typename> typename Concept, typename... Types>
    struct is_tuple_of {
        static constexpr bool value = false;
    };

    template <template <typename> typename Concept, typename... Types>
    struct is_tuple_of<Concept, stl::tuple<Types...>> {
        static constexpr bool value = (Concept<Types>::value && ...);
    };


    template <template <typename> typename Concept, typename Tup>
    concept TupleOf = is_tuple_of<Concept, Tup>::value;


    /**
     * Check if the type T is one of the TupleT's elements.
     * I'm short on internet bandwidth as of writing this; so forgive me if there's already another solution
     * of this in the STL, I don't have the luxury of searching it; so I'm just gonna implement it :)
     */
    template <typename TupleT, typename T, stl::size_t I = stl::tuple_size_v<TupleT> - 1>
    struct tuple_contains {
        static constexpr bool value =
          stl::is_same_v<stl::tuple_element_t<I, TupleT>, T> || tuple_contains<TupleT, T, I - 1>::value;
    };


    template <typename TupleT, typename T>
    struct tuple_contains<TupleT, T, 0> {
        static constexpr bool value = stl::is_same_v<stl::tuple_element_t<0, TupleT>, T>;
    };


} // namespace webpp::istl

#endif // WEBPP_TUPLE_H
