// Created by moisrex on 5/11/20.

#ifndef WEBPP_TUPLE_H
#define WEBPP_TUPLE_H

#include "std.hpp"
#include "type_traits.hpp"

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
    concept TupleOf = Tuple<Tup> && is_tuple_of<Concept, Tup>::value;



    /**
     * Check if the type T is one of the TupleT's elements.
     * I'm short on internet bandwidth as of writing this; so forgive me if there's already another solution
     * of this in the STL, I don't have the luxury of searching it; so I'm just gonna implement it :)
     */
    template <typename TupleT, typename T, stl::size_t I = stl::tuple_size_v<TupleT> - 1>
    struct tuple_contains {
        static constexpr bool value = false;
    };

    template <typename TupleT, typename T, stl::size_t I>
        requires(stl::tuple_size_v<TupleT> > 0)
    struct tuple_contains<TupleT, T, I> {
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
        requires((tuple_contains<TupleT, stl::remove_cvref_t<T>>::value &&
                  ...)) // check if the types are okay
    [[nodiscard]] static constexpr TupleT make_tuple_no_order(T&&... args) noexcept {

        // this uses the TupleT's tuple-like type; std::tuple<T...>;
        using no_order_tuple = typename rebind_parameters<TupleT, stl::remove_cvref_t<T>...>::type;

        if constexpr (stl::is_same_v<TupleT, no_order_tuple>) {
            // It's in order, so there's no need of re-ordering
            return TupleT{stl::forward<T>(args)...};
        } else {
            // re-order, and default-construct those that don't exists in the args
            return (
              []<stl::size_t... ints>(stl::index_sequence<ints...>, T && ... sub_args) constexpr noexcept {
                  no_order_tuple bad_tuple{stl::forward<T>(sub_args)...};
                  // It's a free function and not a lambda because C++ is stupid and doesn't
                  // understand that "ints" in the lambda template is a parameter pack
                  return TupleT{details::tuple_get_value<TupleT, no_order_tuple, ints>(bad_tuple)...};
              })(stl::make_index_sequence<stl::tuple_size_v<TupleT>>{}, stl::forward<T>(args)...);
        }
    }





    /**
     * Run a function on the specified tuple and the index
     * Index can be gotten dynamically
     */
    template <stl::size_t I = 0, typename FuncT, template <typename...> typename Tup, typename... Tp>
        requires(I >= sizeof...(Tp))
    static constexpr void for_index(stl::size_t, Tup<Tp...> const&, FuncT&&) {
        // ending condition function
    }

    template <stl::size_t I = 0, typename FuncT, template <typename...> typename Tup, typename... Tp>
        requires(I < sizeof...(Tp))
    static constexpr void for_index(stl::size_t index, Tup<Tp...>& t, FuncT&& f) {
        if (index == 0)
            f(stl::get<I>(t));
        for_index<I + 1, FuncT, Tup, Tp...>(index - 1, t, stl::forward<FuncT>(f));
    }

    template <stl::size_t I = 0, typename FuncT, template <typename...> typename Tup, typename... Tp>
        requires(I < sizeof...(Tp))
    static constexpr void for_index(stl::size_t index, Tup<Tp...> const& t, FuncT&& f) {
        if (index == 0)
            f(stl::get<I>(t));
        for_index<I + 1, FuncT, Tup, Tp...>(index - 1, t, stl::forward<FuncT>(f));
    }


    template <stl::size_t From, stl::size_t... indices, typename T1, typename T2, typename Func>
    void tuple_transform(T1&& s, T2& t, Func f, stl::index_sequence<indices...>) {
        (void) stl::initializer_list<int>{
          (stl::get<indices + From>(t) = f(stl::get<indices>(stl::forward<T1>(s))), 0)...};
    }


    template <stl::size_t From, stl::size_t To, typename T1, typename T2, typename Func>
    void tuple_transform(T1&& s, T2& t, Func f) {
        tuple_transform<From>(stl::forward<T1>(s), t, f, stl::make_index_sequence<To - From + 1>());
    }


    template <stl::size_t... indices, typename Tup, typename Func>
    [[nodiscard]] static constexpr auto tuple_transform(Tup&& s, Func f, stl::index_sequence<indices...>) {
        return stl::tuple{(f(stl::get<indices>(stl::forward<Tup>(s))), ...)};
    }


    template <typename Tup, typename Func>
    [[nodiscard]] static constexpr auto tuple_transform(Tup&& s, Func f) {
        return tuple_transform(stl::forward<Tup>(s), f, stl::make_index_sequence<stl::tuple_size_v<Tup>>());
    }





    ////////////////////////////// ituple //////////////////////////////

    namespace details {
        template <stl::size_t N>
        struct size_holder : stl::integral_constant<stl::size_t, N> {};

        template <typename T>
        struct is_size_holder : stl::false_type {
            static constexpr stl::size_t size = 0;
        };

        template <stl::size_t N>
        struct is_size_holder<size_holder<N>> : stl::true_type {
            static constexpr stl::size_t size = N;
        };
    } // namespace details

    template <typename... T>
    struct ituple : last_type<T...>::template remove_if<stl::tuple, details::is_size_holder> {
        using last       = last_type<T...>;
        using this_tuple = typename last_type<T...>::template remove_if<stl::tuple, details::is_size_holder>;
        static constexpr stl::size_t native_tuple_size = stl::tuple_size_v<this_tuple>;
        static constexpr stl::size_t tuple_size =
          native_tuple_size + details::is_size_holder<typename last::type>::size;

        // using typename last_type<T...>::template remove<tuple>::tuple;

        template <typename... Args>
        constexpr ituple(Args&&... args) : this_tuple{stl::forward<Args>(args)...} {}

        template <typename... TupT>
        constexpr ituple(stl::tuple<T..., TupT...>&& tup) : this_tuple{} {
            ([this, &tup]<stl::size_t... I>(stl::index_sequence<I...>) constexpr noexcept {
                ((stl::get<I>(as_tuple()) = stl::get<I>(tup)), ...);
            })(stl::make_index_sequence<native_tuple_size>{});
        }

        constexpr this_tuple& as_tuple() noexcept {
            return *static_cast<this_tuple*>(this);
        }

        constexpr this_tuple as_tuple() const noexcept {
            return *static_cast<this_tuple const*>(this);
        }

        template <stl::size_t NewSize>
        [[nodiscard]] auto structured() const noexcept {
            if constexpr (NewSize > native_tuple_size) {
                return typename last_type<T...>::template put_if<
                  ituple,
                  details::is_size_holder,
                  details::size_holder<NewSize - native_tuple_size>>{as_tuple()};
            } else if constexpr (NewSize < native_tuple_size) {
                return typename last_type<T...>::template remove_limit<ituple, NewSize>{as_tuple()};
            } else {
                return *this;
            }
        }

        template <stl::size_t I>
        [[nodiscard]] constexpr auto get() const noexcept {
            if constexpr (I >= native_tuple_size) {
                return nothing_type{};
            } else {
                return stl::get<I>(as_tuple());
            }
        }


        template <stl::size_t I>
            requires(I < native_tuple_size)
        [[nodiscard]] constexpr auto& get() noexcept {
            return stl::get<I>(as_tuple());
        }


        template <stl::size_t I>
        [[nodiscard]] constexpr nothing_type get() noexcept {
            return {};
        }
    };



    /**
     */
    template <typename IterableT,
              stl::size_t TupleSize = stl::tuple_size_v<typename IterableT::iterator::value_type>>
    struct ituple_iterable : IterableT {

        // wrap the iterator type of the iterable:
        using native_iterator = typename IterableT::iterator;
        using iterator        = ituple_iterator<native_iterator>;

        using IterableT::IterableT;


        // move ctor
        template <stl::size_t NewSize>
        constexpr ituple_iterable(ituple_iterable<IterableT, NewSize>&& iterable) noexcept
          : IterableT{stl::move(iterable.native_iterable())} {}

        template <stl::size_t NewSize>
        constexpr ituple_iterable(ituple_iterable<IterableT, NewSize> const& iterable) noexcept
          : IterableT{iterable.native_iterable()} {}

        template <stl::size_t NewSize>
        [[nodiscard]] constexpr ituple_iterable<IterableT, NewSize> structured() noexcept {
            return {stl::move(*this)}; // move ctor
        }

        template <stl::size_t NewSize>
        [[nodiscard]] constexpr ituple_iterable<IterableT, NewSize> structured() const noexcept {
            return {*this}; // copy ctor
        }

        [[nodiscard]] constexpr IterableT& native_iterable() noexcept {
            return *static_cast<IterableT*>(this);
        }

        [[nodiscard]] constexpr IterableT const& native_iterable() const noexcept {
            return *static_cast<IterableT const*>(this);
        }


        [[nodiscard]] iterator begin() noexcept {
            return native_iterator().begin();
        }


        [[nodiscard]] iterator begin() const noexcept {
            return native_iterator().begin();
        }

        [[nodiscard]] iterator end() noexcept {
            return native_iterator().end();
        }

        [[nodiscard]] iterator end() const noexcept {
            return native_iterator().end();
        }
    };

} // namespace webpp::istl

namespace std {

    ////////////////////////////// ituple //////////////////////////////

    template <size_t I, class... T>
        requires(I < webpp::istl::ituple<T...>::native_tuple_size)
    struct tuple_element<I, webpp::istl::ituple<T...>>
      : tuple_element<I, typename webpp::istl::ituple<T...>::this_tuple> {};

    template <size_t I, class... T>
        requires(I >= webpp::istl::ituple<T...>::native_tuple_size)
    struct tuple_element<I, webpp::istl::ituple<T...>> {
        using type = webpp::istl::nothing_type;
    };

    template <class... T>
    struct tuple_size<webpp::istl::ituple<T...>>
      : integral_constant<size_t, webpp::istl::ituple<T...>::tuple_size> {};
} // namespace std

#endif // WEBPP_TUPLE_H
