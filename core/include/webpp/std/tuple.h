// Created by moisrex on 5/11/20.

#ifndef WEBPP_TUPLE_H
#define WEBPP_TUPLE_H

#include <tuple>
#include <type_traits>

namespace stl {

    template <typename T>
    struct is_tuple : public ::std::false_type {};

    template <typename T>
    struct is_tuple<T const> : public is_tuple<T> {};

    template <typename T>
    struct is_tuple<T volatile> : public is_tuple<T> {};

    template <typename T>
    struct is_tuple<T const volatile> : public is_tuple<T> {};

    template <typename... Types>
    struct is_tuple<::std::tuple<Types...>> : public ::std::true_type {};

    template <typename T>
    concept Tuple = is_tuple<T>::value;

    template <template <typename> typename Concept,
              template <typename...> typename Tup, typename... T>
    concept TupleOf = Tuple<Tup<T...>>&& requires(Tup<T...> tup) {
        (Concept<T>::value && ...);
    };


} // namespace stl

#endif // WEBPP_TUPLE_H
