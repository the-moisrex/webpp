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

    //    template <template <typename> typename Concept,
    //              template <typename...> typename Tup, typename... T>
    //    concept TupleOf = Tuple<Tup<T...>>&& requires(Tup<T...> tup) {
    //        (Concept<T>::value && ...);
    //    };
    //

} // namespace webpp::istl

#endif // WEBPP_TUPLE_H
