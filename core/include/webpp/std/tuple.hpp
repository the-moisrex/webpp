// Created by moisrex on 5/11/20.

#ifndef WEBPP_TUPLE_H
#define WEBPP_TUPLE_H

#include "./std.hpp"
#include "./type_traits.hpp"

#include <tuple>

namespace webpp::istl {

    template <typename T>
    struct is_pair : public stl::false_type {};

    template <typename T>
    struct is_pair<T const> : public is_pair<T> {};

    template <typename T>
    struct is_pair<T volatile> : public is_pair<T> {};

    template <typename T>
    struct is_pair<T const volatile> : public is_pair<T> {};

    template <typename... Types>
    struct is_pair<stl::pair<Types...>> : public stl::true_type {};

    template <typename T>
    concept Pair = is_pair<T>::value;


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

    template <template <typename> typename Concept, typename... Types>
    struct is_tuple_of {
        static constexpr bool value = false;
    };

    template <template <typename> typename Concept,
              typename... Types,
              template <typename...>
              typename TupleType>
    struct is_tuple_of<Concept, TupleType<Types...>> {
        static constexpr bool value = (Concept<Types>::value && ...);
    };


    template <template <typename> typename Concept, typename Tup>
    concept TupleOf = Tuple<Tup>&& is_tuple_of<Concept, Tup>::value;



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


    namespace details {
        template <typename TupleT, typename no_order_tuple, stl::size_t index>
        constexpr auto tuple_get_value(no_order_tuple& bad_tuple) noexcept {
            using this_type = stl::tuple_element_t<index, TupleT>;
            if constexpr (tuple_contains<no_order_tuple, this_type>::value) {
                return stl::get<this_type>(bad_tuple);
            } else {
                // default construct the type
                return this_type{};
            }
        }
    } // namespace details

    /**
     * With tuples that have no similar types, this function will able to help you pass the arguments of the
     * tuple in the right oder if you give the arguments in the wrong order.
     * The types that don't exists in the args, will be default constructed.
     */
    template <Tuple TupleT, typename... T>
    requires((tuple_contains<TupleT, T>::value && ...)) // check if the types are okay
      [[nodiscard]] constexpr TupleT make_tuple_no_order(T&&... args) noexcept {
        using no_order_tuple = stl::tuple<T...>;
        if constexpr (stl::same_as<TupleT, no_order_tuple>) {
            // It's in order, so there's no need of re-ordering
            return TupleT{stl::forward<T>(args)...};
        } else {
            // re-order, and default-construct those that don't exists in the args
            return ([]<stl::size_t... ints>(stl::index_sequence<ints...>, T && ... args) constexpr noexcept {
                no_order_tuple bad_tuple{stl::forward<T>(args)...};
                // It's a free function and not a lambda because C++ is stupid and doesn't
                // understand that "ints" in the lambda template is a parameter pack
                return TupleT{details::tuple_get_value<TupleT, no_order_tuple, ints>(bad_tuple)...};
            })(stl::make_index_sequence<stl::tuple_size_v<TupleT>>{}, stl::forward<T>(args)...);
        }
    }


} // namespace webpp::istl

#endif // WEBPP_TUPLE_H
