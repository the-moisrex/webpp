// Created by moisrex on 5/11/20.

#ifndef WEBPP_TUPLE_H
#define WEBPP_TUPLE_H

#include "std.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

#include <tuple>

namespace webpp::istl {

    template <typename T>
    struct is_pair : stl::false_type {};

    template <typename T>
    struct is_pair<T const> : is_pair<T> {};

    template <typename T>
    struct is_pair<T volatile> : is_pair<T> {};

    template <typename T>
    struct is_pair<T const volatile> : is_pair<T> {};

    template <typename... Types>
    struct is_pair<stl::pair<Types...>> : stl::true_type {};

    template <typename T>
    concept Pair = is_pair<T>::value;

    template <typename T>
    struct is_tuple : stl::false_type {};

    template <typename T>
    struct is_tuple<T const> : is_tuple<T> {};

    template <typename T>
    struct is_tuple<T&> : is_tuple<T> {};

    template <typename T>
    struct is_tuple<T&&> : is_tuple<T> {};

    template <typename T>
    struct is_tuple<T volatile> : is_tuple<T> {};

    template <typename T>
    struct is_tuple<T const volatile> : is_tuple<T> {};

    template <typename... Types>
    struct is_tuple<stl::tuple<Types...>> : stl::true_type {};

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
        requires(
          (tuple_contains<TupleT, stl::remove_cvref_t<T>>::value && ...)) // check if the types are okay
    [[nodiscard]] static constexpr TupleT make_tuple_no_order(T&&... args) {
        // this uses the TupleT's tuple-like type; std::tuple<T...>;
        using no_order_tuple = typename rebind_parameters<TupleT, stl::remove_cvref_t<T>...>::type;

        if constexpr (stl::is_same_v<TupleT, no_order_tuple>) {
            // It's in order, so there's no need of re-ordering
            return TupleT{stl::forward<T>(args)...};
        } else {
            // re-order, and default-construct those that don't exist in the args
            return ([]<stl::size_t... ints>(stl::index_sequence<ints...>, T&&... sub_args) constexpr {
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
    static constexpr void for_index(
      [[maybe_unused]] stl::size_t       index,
      [[maybe_unused]] Tup<Tp...> const& inp_tp,
      [[maybe_unused]] FuncT&&           inp_func) {
        // ending condition function
    }

    template <stl::size_t I = 0, typename FuncT, template <typename...> typename Tup, typename... Tp>
        requires(I < sizeof...(Tp))
    static constexpr void for_index(stl::size_t const index, Tup<Tp...>& inp_tup, FuncT&& inp_func) {
        if (index == 0) {
            inp_func(stl::get<I>(inp_tup));
        }
        for_index<I + 1, FuncT, Tup, Tp...>(index - 1, inp_tup, stl::forward<FuncT>(inp_func));
    }

    template <stl::size_t I = 0, typename FuncT, template <typename...> typename Tup, typename... Tp>
        requires(I < sizeof...(Tp))
    static constexpr void for_index(stl::size_t const index, Tup<Tp...> const& inp_tup, FuncT&& inp_func) {
        if (index == 0) {
            inp_func(stl::get<I>(inp_tup));
        }
        for_index<I + 1, FuncT, Tup, Tp...>(index - 1, inp_tup, stl::forward<FuncT>(inp_func));
    }

    template <stl::size_t From, stl::size_t... indices, typename T1, typename T2, typename Func>
    void tuple_transform(T1&&                                             inp_tup1,
                         T2&                                              inp_tup2,
                         Func                                             inp_func,
                         [[maybe_unused]] stl::index_sequence<indices...> indeces) {
        stl::ignore = stl::initializer_list<int>{
          (stl::get<indices + From>(inp_tup2) = inp_func(stl::forward_like<T1>(get<indices>(inp_tup1))),
           0)...};
    }

    template <stl::size_t From, stl::size_t To, typename T1, typename T2, typename Func>
    void tuple_transform(T1&& inp_tup1, T2& inp_tup2, Func inp_func) {
        tuple_transform<From>(
          stl::forward<T1>(inp_tup1),
          inp_tup2,
          inp_func,
          stl::make_index_sequence<To - From + 1>());
    }

    template <stl::size_t... Indeces, typename Tup, typename Func>
    [[nodiscard]] static constexpr auto tuple_transform(
      Tup&&                                            inp_tup1,
      Func                                             inp_func,
      [[maybe_unused]] stl::index_sequence<Indeces...> inp_indeces) {
        return stl::tuple{(inp_func(stl::forward_like<Tup>(get<Indeces>(inp_tup1))), ...)};
    }

    template <typename Tup, typename Func>
    [[nodiscard]] static constexpr auto tuple_transform(Tup&& inp_tup1, Func inp_func) {
        return tuple_transform(stl::forward<Tup>(inp_tup1),
                               inp_func,
                               stl::make_index_sequence<stl::tuple_size_v<Tup>>());
    }

    ////////////////////////////// ituple //////////////////////////////


    template <typename T>
    concept ItupleOptions = requires(T opts) {
        typename T::default_type;
        T::size;
        typename T::template resize<2>;
    };

    template <typename T>
    struct is_ituple_options {
        static constexpr bool        value = false;
        static constexpr stl::size_t size  = 0;
    };

    template <typename T>
        requires(ItupleOptions<T>)
    struct is_ituple_options<T> {
        static constexpr bool        value = true;
        static constexpr stl::size_t size  = T::size;
    };

    template <stl::size_t N = 0>
    struct default_ituple_options {
        using default_type                = istl::nothing_type;
        static constexpr stl::size_t size = N;

        template <stl::size_t NewN>
        using resize = default_ituple_options<NewN>;
    };

    template <typename... T>
    struct ituple : last_type<T...>::template remove_if<stl::tuple, is_ituple_options> {
        // we're separating the ituple_type and ituple_template to fix gcc's confusion
        using ituple_type = ituple;

        template <typename... TT>
        using ituple_template = ituple<TT...>;

        using last         = last_type<T...>;
        using options      = stl::conditional_t<is_ituple_options<typename last::type>::value,
                                                typename last::type,
                                                default_ituple_options<>>;
        using this_tuple   = typename last_type<T...>::template remove_if<stl::tuple, is_ituple_options>;
        using default_type = typename options::default_type;
        static constexpr stl::size_t native_tuple_size = stl::tuple_size_v<this_tuple>;
        static constexpr stl::size_t tuple_size =
          native_tuple_size + is_ituple_options<typename last::type>::size;

        template <typename NewOpts>
        using replace_options =
          typename last_type<T...>::template put_if<ituple_template, is_ituple_options, NewOpts>;

        template <stl::size_t NewSize>
        using restructured_type = stl::conditional_t<
          (NewSize > native_tuple_size),
          replace_options<typename options::template resize<NewSize - native_tuple_size>>,
          stl::conditional_t<(NewSize < native_tuple_size),
                             typename last_type<T...>::template remove_limit<ituple_template, NewSize>,
                             ituple_type>>;

        // using typename last_type<T...>::template remove<tuple>::tuple;

        template <typename... Args>
        explicit constexpr ituple(Args&&... args) : this_tuple{stl::forward<Args>(args)...} {}

        template <template <typename...> typename Tup, typename... Args>
        explicit constexpr ituple(ituple<Args...>&& tup)
          : this_tuple{stl::forward<Args>(stl::move(stl::get<Args>(tup)))...} {} // todo: forward_like?

        template <typename... TupT>
        explicit constexpr ituple(stl::tuple<T..., TupT...>&& tup) : this_tuple{} {
            ([this, &tup]<stl::size_t... I>(stl::index_sequence<I...>) constexpr noexcept {
                ((stl::get<I>(as_tuple()) = stl::move(stl::get<I>(tup))), ...);
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
                return replace_options<typename options::template resize<NewSize - native_tuple_size>>{
                  as_tuple()};
            } else if constexpr (NewSize < native_tuple_size) {
                return typename last_type<T...>::template remove_limit<ituple, NewSize>{as_tuple()};
            } else {
                return *this;
            }
        }

        template <stl::size_t I>
        [[nodiscard]] constexpr auto get() const noexcept {
            if constexpr (I >= native_tuple_size) {
                return get_default<I>();
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
        [[nodiscard]] constexpr default_type get() noexcept {
            return get_default<I>();
        }

        template <stl::size_t I = 0>
        [[nodiscard]] constexpr default_type get_default() const noexcept {
            if constexpr (requires { options::template get_default<I>(*this); }) {
                return options::template get_default<I>(*this);
            } else if constexpr (requires { options::template get_default<I>(); }) {
                return options::template get_default<I>();
            } else {
                return {};
            }
        }
    };

    template <typename... T>
    ituple(T&&...) -> ituple<T...>;

    template <template <typename...> typename Tup, typename... T>
    ituple(Tup<T...>&&) -> ituple<T...>;

    template <typename... T>
    struct ituplify {
        using type = ituple<T...>;
    };

    template <template <typename...> typename Tt, typename... T>
    struct ituplify<Tt<T...>> {
        using type = ituple<T...>;
    };

    template <template <typename...> typename Tt, typename Opts, typename... T>
    struct ituplify<Tt<T...>, Opts> {
        using type = typename ituple<T...>::template replace_options<Opts>;
    };

    template <template <typename...> typename Tt, typename Opts, typename... T>
    struct ituplify<Tt<T...> const&, Opts> : ituplify<ituple<T...>, Opts> {};

    template <template <typename...> typename Tt, typename Opts, typename... T>
    struct ituplify<Tt<T...>&&, Opts> : ituplify<ituple<T...>, Opts> {};

    template <template <typename...> typename Tt, typename Opts, typename... T>
    struct ituplify<Tt<T...> const, Opts> : ituplify<ituple<T...>, Opts> {};

    template <template <typename...> typename Tt, typename Opts, typename... T>
    struct ituplify<Tt<T...>&, Opts> : ituplify<ituple<T...>, Opts> {};

    /**
     * This is a wrapper for any type of iterator that holds an ituple
     */
    template <typename Iter, ItupleOptions OptsT = default_ituple_options<0>>
    struct ituple_iterator : Iter {
        using options = OptsT;

        using Iter::Iter;

        explicit constexpr ituple_iterator(Iter const& iter) : Iter{iter} {}

        explicit constexpr ituple_iterator(Iter&& iter) : Iter{stl::move(iter)} {}

        // value type is an ituple
        using value_type =
          typename ituplify<typename Iter::value_type>::type::template restructured_type<OptsT::size>;
        using reference        = value_type&;
        using pointer          = value_type*;
        using native_reference = typename Iter::reference;

        static constexpr bool is_nothing = stl::is_same_v<value_type, typename OptsT::default_type>;

        [[nodiscard]] constexpr Iter& native_iterator() noexcept {
            return *static_cast<Iter*>(this);
        }

        [[nodiscard]] constexpr Iter const& native_iterator() const noexcept {
            return *static_cast<Iter const*>(this);
        }

        [[nodiscard]] constexpr auto operator*() noexcept {
            if constexpr (requires { options::ituplify(native_iterator().operator*()); }) {
                return options::ituplify(native_iterator().operator*());
            } else {
                using ituple_type = typename ituplify<native_reference, options>::type;
                return ituple_type{native_iterator().operator*()};
            }
        }

        [[nodiscard]] constexpr auto operator*() const noexcept {
            if constexpr (requires { options::ituplify(native_iterator().operator*()); }) {
                return options::ituplify(native_iterator().operator*());
            } else {
                using ituple_type = typename ituplify<native_reference, options>::type;
                return ituple_type{native_iterator().operator*()};
            }
        }
    };

    /**
     * This struct will change the iterator and provides restructuring features for the user to use
     * "structured bindings" in for loops.
     */
    template <typename IterableT, ItupleOptions OptsT = default_ituple_options<0>>
    struct ituple_iterable {
        // wrap the iterator type of the iterable:
        using iterable              = stl::remove_cvref_t<IterableT>;
        using native_iterator       = typename iterable::iterator;
        using const_native_iterator = typename iterable::const_iterator;
        using iterator              = ituple_iterator<native_iterator, OptsT>;
        using const_iterator        = ituple_iterator<const_native_iterator, OptsT>;
        using iterable_ref          = stl::add_lvalue_reference_t<iterable>;

        iterable_ref object;

        explicit constexpr ituple_iterable(iterable_ref obj) noexcept : object{obj} {}

        constexpr ituple_iterable(ituple_iterable const&)     = default;
        constexpr ituple_iterable(ituple_iterable&&) noexcept = default;

        // move ctor
        template <stl::size_t NewSize>
        explicit constexpr ituple_iterable(
          ituple_iterable<IterableT, typename OptsT::template resize<NewSize>>& in_iterable) noexcept
          : object{in_iterable.object} {}

        template <stl::size_t NewSize>
        [[nodiscard]] constexpr ituple_iterable<IterableT, typename OptsT::template resize<NewSize>>
        structured() noexcept {
            return {*this};
        }

        [[nodiscard]] iterator begin() noexcept {
            return iterator{object.begin()};
        }

        [[nodiscard]] const_iterator begin() const noexcept {
            return const_iterator{object.begin()};
        }

        [[nodiscard]] iterator end() noexcept {
            return iterator{object.end()};
        }

        [[nodiscard]] const_iterator end() const noexcept {
            return const_iterator{object.end()};
        }
    };



    ////////////////////////////// Tuple of type T with N elements //////////////////////////////


    template <typename, typename>
    struct join_tuples;

    template <typename... Left, typename... Right>
    struct join_tuples<stl::tuple<Left...>, stl::tuple<Right...>> {
        using type = stl::tuple<Left..., Right...>;
    };

    template <typename T, unsigned N>
    struct generate_tuple_type {
        using left  = typename generate_tuple_type<T, N / 2>::type;
        using right = typename generate_tuple_type<T, N / 2 + N % 2>::type;
        using type  = typename join_tuples<left, right>::type;
    };

    template <typename T>
    struct generate_tuple_type<T, 1> {
        using type = stl::tuple<T>;
    };

    template <typename T>
    struct generate_tuple_type<T, 0> {
        using type = stl::tuple<>;
    };

    template <typename Tuple, typename FuncT>
        requires(stl::tuple_size_v<stl::remove_cvref_t<Tuple>> >= 2)
    constexpr void adjacent_apply(Tuple&& tup, FuncT&& func) {
        using tuple_type = stl::remove_cvref_t<Tuple>;
        ([&]<stl::size_t... I>(stl::index_sequence<I...>) constexpr {
            ((func(stl::forward_like<Tuple>(stl::get<I>(tup)),
                   stl::forward_like<Tuple>(stl::get<I + 1>(tup)))),
             ...);
        })(stl::make_index_sequence<stl::tuple_size_v<tuple_type> - 1>{});
    }

    namespace details {
        template <typename T>
        constexpr auto explode(T&& inp_tup, [[maybe_unused]] char inp) {
            return stl::forward_as_tuple(stl::forward<T>(inp_tup));
        }

        template <typename T, stl::size_t I = stl::tuple_size_v<stl::decay_t<T>>>
        constexpr auto explode(T&& inp_tup, [[maybe_unused]] int inp);

        template <typename T, stl::size_t... Is>
        constexpr auto explode(T&& inp_tup, [[maybe_unused]] stl::index_sequence<Is...> indeces) {
            return tuple_cat(explode(stl::forward_like<T>(get<Is>(inp_tup)), 0)...);
        }

        template <typename T, stl::size_t I>
        constexpr auto explode(T&& inp_tup, [[maybe_unused]] int inp) {
            return explode(stl::forward<T>(inp_tup), stl::make_index_sequence<I>{});
        }

        template <typename T, stl::size_t... Is>
        constexpr auto decay_tuple(T&& inp_tup, [[maybe_unused]] stl::index_sequence<Is...> indeces) {
            return stl::make_tuple(stl::forward_like<T>(get<Is>(inp_tup))...);
        }

        template <typename T>
        constexpr auto decay_tuple(T&& inp_tup) {
            return decay_tuple(stl::forward<T>(inp_tup),
                               stl::make_index_sequence<stl::tuple_size_v<stl::decay_t<T>>>{});
        }

        template <typename T, stl::size_t... Is>
        constexpr auto flatten_tuple(T&& inp_tup, [[maybe_unused]] stl::index_sequence<Is...> inp) {
            return decay_tuple(tuple_cat(explode(stl::forward_like<T>(get<Is>(inp_tup)), 0)...));
        }

    } // namespace details

    // todo: check this, it's not tested.
    template <typename T>
    constexpr auto flatten_tuple(T&& inp_tup) {
        return details::flatten_tuple(stl::forward<T>(inp_tup),
                                      stl::make_index_sequence<stl::tuple_size_v<stl::decay_t<T>>>{});
    }

    template <template <typename...> typename TupTempl, typename... T, stl::size_t... I>
    [[nodiscard]] constexpr TupTempl<nth_parameter_t<I, T...>...>
    sub_tuple(TupTempl<T...>&& tup, [[maybe_unused]] stl::index_sequence<I...> indeces) noexcept(
      stl::is_nothrow_constructible_v<TupTempl<T...>, nth_parameter_t<I, T...>...>) {
        return {get<I>(stl::move(tup))...};
    }

    template <template <typename...> typename TupTempl, typename... T, stl::size_t... I>
    [[nodiscard]] constexpr TupTempl<nth_parameter_t<I, T...>...>
    sub_tuple(TupTempl<T...> const& tup, [[maybe_unused]] stl::index_sequence<I...> indeces) noexcept(
      stl::is_nothrow_constructible_v<TupTempl<T...>, nth_parameter_t<I, T...>...>) {
        return {get<I>(tup)...};
    }

    template <stl::size_t Start = 0,
              stl::size_t End   = Start,
              template <typename...>
              typename TupTempl,
              typename... T>
    [[nodiscard]] constexpr auto sub_tuple(TupTempl<T...> const& tup) noexcept(
      noexcept(sub_tuple(tup, make_index_range<Start, (End < sizeof...(T) ? End : sizeof...(T))>{}))) {
        return sub_tuple(tup, make_index_range<Start, (End < sizeof...(T) ? End : sizeof...(T))>{});
    }

    template <stl::size_t Start = 0,
              stl::size_t End   = Start,
              template <typename...>
              typename TupTempl,
              typename... T>
    [[nodiscard]] constexpr auto sub_tuple(TupTempl<T...>&& tup) noexcept(noexcept(
      sub_tuple(stl::move(tup), make_index_range<Start, (End < sizeof...(T) ? End : sizeof...(T))>{}))) {
        return sub_tuple(stl::move(tup),
                         make_index_range<Start, (End < sizeof...(T) ? End : sizeof...(T))>{});
    }

    template <stl::size_t From, stl::size_t To, Tuple T>
        requires(From == To)
    [[nodiscard]] constexpr auto move_element_to(T&& tup) {
        return stl::forward<T>(tup);
    }

    template <stl::size_t From, stl::size_t To, Tuple T>
        requires(From < To)
    [[nodiscard]] constexpr auto move_element_to(T&& tup) {
        return stl::tuple_cat(
          sub_tuple<0, From>(tup),
          sub_tuple<From + 1, To>(tup),
          stl::make_tuple(stl::forward_like<T>(get<From>(tup))),
          sub_tuple<To, stl::tuple_size_v<stl::remove_cvref_t<T>>>(tup));
    }

    template <stl::size_t From, stl::size_t To, Tuple T>
        requires(From > To)
    [[nodiscard]] constexpr auto move_element_to(T&& tup) {
        return stl::tuple_cat(
          sub_tuple<0, To>(tup),
          stl::make_tuple(stl::forward_like<T>(get<From>(tup))),
          sub_tuple<To, From>(tup),
          sub_tuple<From + 1, stl::tuple_size_v<stl::remove_cvref_t<T>>>(tup));
    }

    template <stl::size_t From, Tuple T>
    [[nodiscard]] constexpr auto move_element_to_back(T&& tup) {
        return move_element_to<From, stl::tuple_size_v<stl::remove_cvref_t<T>>>(stl::forward<T>(tup));
    }

    template <stl::size_t From, Tuple T>
    [[nodiscard]] constexpr auto move_element_to_front(T&& tup) {
        return move_element_to<From, 0>(stl::forward<T>(tup));
    }

    /**
     * Re-Order the tuple's element based on the specified index_sequence
     */
    template <typename Tup, stl::size_t... I>
    [[nodiscard]] constexpr auto tuple_reorder_elements(Tup&&                                      tup,
                                                        [[maybe_unused]] stl::index_sequence<I...> indeces) {
        return stl::make_tuple(stl::forward_like<Tup>(get<I>(tup))...);
    }

    namespace details {
        template <typename TupleT, typename F, stl::size_t... Indices>
        static constexpr void
        for_each_impl(F&& inp_func, TupleT&& tup, [[maybe_unused]] stl::index_sequence<Indices...> indeces) {
            using swallow = int[]; // NOLINT(*-avoid-c-arrays)
            stl::ignore   = swallow{
              1,
              (stl::forward<F>(inp_func)(stl::forward_like<TupleT>(get<Indices>(tup))), void(), int{})...};
        }
    } // namespace details

    // initially from https://codereview.stackexchange.com/questions/51407/stdtuple-foreach-implementation
    // The order of the elements are reversed to match std::apply
    template <typename F, typename... Args, template <typename...> typename TupleT>
        requires((stl::is_invocable_v<F, stl::add_const_t<stl::add_lvalue_reference_t<Args>>> && ...))
    static constexpr void for_each_element(F&& inp_func, TupleT<Args...> const& tup) noexcept(
      (stl::is_nothrow_invocable_v<F, stl::add_const_t<stl::add_lvalue_reference_t<Args>>> && ...)) {
        details::for_each_impl(stl::forward<F>(inp_func), tup, stl::index_sequence_for<Args...>{});
    }

    template <typename F, typename... Args, template <typename...> typename TupleT>
        requires((stl::is_invocable_v<F, stl::add_lvalue_reference_t<Args>> && ...))
    static constexpr void for_each_element(F&& inp_func, TupleT<Args...>& tup) noexcept(
      (stl::is_nothrow_invocable_v<F, stl::add_lvalue_reference_t<Args>> && ...)) {
        details::for_each_impl(stl::forward<F>(inp_func), tup, stl::index_sequence_for<Args...>{});
    }

    template <typename F, typename... Args, template <typename...> typename TupleT>
        requires((stl::is_invocable_v<F, stl::add_rvalue_reference_t<Args>> && ...))
    static constexpr void for_each_element(F&& inp_func, TupleT<Args...>&& tup) noexcept(
      (stl::is_nothrow_invocable_v<F, stl::add_rvalue_reference_t<Args>> && ...)) {
        details::for_each_impl(stl::forward<F>(inp_func), tup, stl::index_sequence_for<Args...>{});
    }
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
        using type = typename webpp::istl::ituple<T...>::default_type;
    };

    template <class... T>
    struct tuple_size<webpp::istl::ituple<T...>>
      : integral_constant<size_t, webpp::istl::ituple<T...>::tuple_size> {};
} // namespace std

#endif // WEBPP_TUPLE_H
