// Created by moisrex on 9/27/20.

#ifndef WEBPP_TYPE_TRAITS_HPP
#define WEBPP_TYPE_TRAITS_HPP

#include "./std.hpp"

#include <type_traits>
#include <utility> // for move and forward

namespace webpp::istl {

    struct nothing_type {};

    template <typename T>
    using void_holder = stl::conditional_t<stl::is_void_v<T>, nothing_type, T>;


    template <typename T>
    struct lazy_type {
        template <bool Condition>
        struct evaluate {
            using type = T;
        };
    };

    namespace details {

        template <bool Condition, template <typename...> typename Extractor, typename... Args>
        struct templated_lazy_evaluate {
            using type = void;
        };

        template <template <typename...> typename Extractor, typename... Args>
        struct templated_lazy_evaluate<true, Extractor, Args...> {
            using type = typename Extractor<Args...>::type;
        };
    } // namespace details

    template <template <typename...> typename Extractor, typename... Args>
    struct templated_lazy_type {
        // I know how it looks, but it's c++, what did you expect? :)
        template <bool Condition>
        using evaluate = details::templated_lazy_evaluate<Condition, Extractor, Args...>;
    };



    /**
     * Lazy Conditional is a class type that is designed to have the same usage as std::conditional but
     * the values are not going to be evaluated if the condition doesn't meet for that value.
     */
    template <bool Condition, typename T1, typename T2>
    struct lazy_conditional;

    template <typename T1, typename T2>
    struct lazy_conditional<true, T1, T2> {
        using type = typename T1::template evaluate<true>::type;
    };

    template <typename T1, typename T2>
    struct lazy_conditional<false, T1, T2> {
        using type = typename T2::template evaluate<true>::type;
    };

    template <bool Condition, typename T1, typename T2>
    using lazy_conditional_t = typename lazy_conditional<Condition, T1, T2>::type;


    ////////////////////////// Nth Types ////////////////////////////

    namespace details {
        template <stl::size_t Index, typename T, stl::size_t ThisIndex = 0>
        struct nth_of {
            using type                         = T;
            static constexpr stl::size_t index = ThisIndex;
            type                         value;

            // this is for adding one to the index
            constexpr auto add_one() noexcept {}

            template <typename NextType>
            constexpr auto operator|(NextType&& next) const noexcept {
                if constexpr (ThisIndex == Index) {
                    return *this;
                } else {
                    return nth_of<Index, NextType, ThisIndex + 1>{.value = stl::forward<NextType>(next)};
                }
            }
        };
    } // namespace details

    template <stl::size_t Index, typename First, typename... T>
    [[nodiscard]] constexpr auto nth_of(First&& first, T&&... objs) noexcept {
        static_assert(Index < (sizeof...(T) + 1), "The specified Index is out of range");
        return ((details::nth_of<Index, First>{.value = stl::forward<First>(first)}) | ... |
                stl::forward<T>(objs))
          .value;
    }

    ////////////////////////// Rank Types ////////////////////////////

    /**
     * Rank types
     * With the help of this struct you'll be able to rank your types based on the condition templated type
     * you specify.
     *
     * If the there's equality in between these types, you should expect the random type to be chosen and if
     * it's not what you wanted, you should change your conditions.
     *
     * Negative and Positive values are calculated.
     *
     * @example The code below shows an example that the rules are:
     *   - if it's a string (2 times more important that the other rules)
     *   - if it's allocator is not the std::allocator<char_type>
     * @code
     *   template <typename T>
     *   struct Cond {
     *       static constexpr stl::size_t value = int(istl::String<T>) * 2 + !requires {
     *           stl::same_as<typename T::allocator_type, stl::allocator<typename T::value_type>>;
     *       };
     *   };
     *
     *   static_assert(
     *       stl::is_same_v<
     *           std::string,
     *           istl::ranked_type<Cond, std::string, std::string_view>::best::type
     *       >
     *   );
     * @endcode
     */
    template <template <typename> typename ConditionOp, typename... Types>
    struct ranked_types {

        static_assert((stl::is_integral_v<stl::remove_cvref_t<decltype(ConditionOp<Types>::value)>> && ...),
                      "The specified Condition is not valid");

        template <typename T, stl::size_t Index = 0>
        struct a_type {
            static constexpr long long int rank  = ConditionOp<T>::value;
            static constexpr stl::size_t   index = Index;
            using type                           = T;

            template <typename... ItemType>
            [[nodiscard]] static constexpr auto get(ItemType&&... item) noexcept {
                return nth_of<index>(stl::forward<ItemType>(item)...);
            }

          private:
            friend ranked_types;

            // this is for adding one to the index
            static constexpr auto add_one() noexcept {
                return a_type<T, index + 1>();
            }

            template <typename NextType>
            constexpr auto operator|(NextType&& next) const noexcept {
                if constexpr (rank < NextType::rank) {
                    return *this;
                } else {
                    return next.add_one();
                }
            }

            template <typename NextType>
            constexpr auto operator&(NextType&& next) const noexcept {
                if constexpr (rank > NextType::rank) {
                    return *this;
                } else {
                    return next.add_one();
                }
            }
        };


        using best  = decltype((a_type<Types>() & ...));
        using worst = decltype((a_type<Types>() | ...));
    };




    ////////////////////////////////////////////////////////////////
    /////// changing template parameter type from Old to New ///////
    ////////////////////////////////////////////////////////////////

    template <typename T, template <typename...> typename NewType>
    struct template_swap_type {
        using type = T;
    };

    template <template <typename...> typename T, template <typename...> typename NewType, typename... Args>
    struct template_swap_type<T<Args...>, NewType> {
        using type = NewType<Args...>;
    };

    template <typename T, template <typename...> typename NewType>
    using template_swap = typename template_swap_type<T, NewType>::type;

    namespace details {

        template <typename... T>
        struct fake_tuple {};

        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename T1,
                  typename T2>
        struct param_replacer;

        // recursive case (move from T1 tuple to T2 tuple if it's not a match)
        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename... Heads,
                  typename This,
                  typename... Tails>
        struct param_replacer<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<This, Tails...>>
          : param_replacer<T, OldType, NewType, fake_tuple<Heads..., This>, fake_tuple<Tails...>> {};

        // base case (found the old type)
        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename... Heads,
                  typename... Tails>
        struct param_replacer<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<OldType, Tails...>>
          : public param_replacer<T, OldType, NewType, fake_tuple<Heads..., NewType>, fake_tuple<Tails...>> {
        };

        template <template <typename...> typename T, typename OldType, typename NewType, typename... Heads>
        struct param_replacer<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<>> {
            using type = T<Heads...>;
        };





        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename T1,
                  typename T2>
        struct templated_param_replacer;

        // recursive case (move from T1 tuple to T2 tuple if it's not a match)
        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Heads,
                  typename This,
                  typename... Tails>
        struct templated_param_replacer<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<This, Tails...>>
          : templated_param_replacer<T, OldType, NewType, fake_tuple<Heads..., This>, fake_tuple<Tails...>> {
        };

        // base case (found the old type)
        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Heads,
                  typename... Tails,
                  typename... OldArgs>
        struct templated_param_replacer<T,
                                        OldType,
                                        NewType,
                                        fake_tuple<Heads...>,
                                        fake_tuple<OldType<OldArgs...>, Tails...>>
          : public templated_param_replacer<T,
                                            OldType,
                                            NewType,
                                            fake_tuple<Heads..., NewType<OldArgs...>>,
                                            fake_tuple<Tails...>> {};

        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Heads>
        struct templated_param_replacer<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<>> {
            using type = T<Heads...>;
        };





        template <template <typename...> typename T,
                  template <typename>
                  typename Replacer,
                  typename T1,
                  typename T2>
        struct parameter_replacer;


#define WEBPP_REMOVE_CVREF(CVREF)                                                                           \
    template <template <typename...> typename T,                                                            \
              template <typename>                                                                           \
              typename Replacer,                                                                            \
              typename... Heads,                                                                            \
              typename This,                                                                                \
              typename... Tails>                                                                            \
    struct parameter_replacer<T, Replacer, fake_tuple<Heads...>, fake_tuple<This CVREF, Tails...>>          \
      : parameter_replacer<                                                                                 \
          T,                                                                                                \
          Replacer,                                                                                         \
          fake_tuple<Heads...,                                                                              \
                     stl::conditional_t<Replacer<This>::value, typename Replacer<This>::type, This>> CVREF, \
          fake_tuple<Tails...>> {};                                                                         \
                                                                                                            \
                                                                                                            \
    template <template <typename...> typename T,                                                            \
              template <typename>                                                                           \
              typename Replacer,                                                                            \
              typename... Heads,                                                                            \
              template <typename...>                                                                        \
              typename This,                                                                                \
              typename... Tails,                                                                            \
              typename... ThisArgs>                                                                         \
    struct parameter_replacer<T,                                                                            \
                              Replacer,                                                                     \
                              fake_tuple<Heads...>,                                                         \
                              fake_tuple<This<ThisArgs...> CVREF, Tails...>>                                \
      : parameter_replacer<                                                                                 \
          T,                                                                                                \
          Replacer,                                                                                         \
          fake_tuple<                                                                                       \
            Heads...,                                                                                       \
            stl::conditional_t<                                                                             \
              Replacer<typename parameter_replacer<This, Replacer, fake_tuple<>, fake_tuple<ThisArgs...>>:: \
                         type>::value,                                                                      \
              typename Replacer<                                                                            \
                typename parameter_replacer<This, Replacer, fake_tuple<>, fake_tuple<ThisArgs...>>::type>:: \
                type,                                                                                       \
              typename parameter_replacer<This, Replacer, fake_tuple<>, fake_tuple<ThisArgs...>>::type>     \
              CVREF>,                                                                                       \
          fake_tuple<Tails...>> {};


        WEBPP_REMOVE_CVREF()
        WEBPP_REMOVE_CVREF(const)
        WEBPP_REMOVE_CVREF(volatile)
        WEBPP_REMOVE_CVREF(const volatile)
        WEBPP_REMOVE_CVREF(&)
        WEBPP_REMOVE_CVREF(&&)
        WEBPP_REMOVE_CVREF(const&)
        WEBPP_REMOVE_CVREF(const&&)
        WEBPP_REMOVE_CVREF(volatile&)
        WEBPP_REMOVE_CVREF(volatile&&)
        WEBPP_REMOVE_CVREF(const volatile&)
        WEBPP_REMOVE_CVREF(const volatile&&)

#undef WEBPP_REMOVE_CVREF

        template <template <typename...> typename T, template <typename> typename Replacer, typename... Heads>
        struct parameter_replacer<T, Replacer, fake_tuple<Heads...>, fake_tuple<>> {
            using type = stl::
              conditional_t<Replacer<T<Heads...>>::value, typename Replacer<T<Heads...>>::type, T<Heads...>>;
        };





        template <typename T, typename OldType, typename NewType>
        struct change_parameter;

        template <template <typename...> typename T, typename OldType, typename NewType, typename... Types>
        struct change_parameter<T<Types...>, OldType, NewType> {
            using the_type = T<Types...>;
            using type =
              typename details::param_replacer<T, OldType, NewType, fake_tuple<>, fake_tuple<Types...>>::type;
        };


        template <typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType>
        struct change_templated_parameter;

        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Types>
        struct change_templated_parameter<T<Types...>, OldType, NewType> {
            using the_type = T<Types...>;
            using type     = typename details::
              templated_param_replacer<T, OldType, NewType, fake_tuple<>, fake_tuple<Types...>>::type;
        };


        ////////////////////////////// replace_parameters //////////////////////////////

        template <typename T, template <typename> typename Replacer>
        struct replace_parameters {
            using type = stl::conditional_t<Replacer<T>::value, typename Replacer<T>::type, T>;
        };

        template <template <typename...> typename T, template <typename> typename Replacer, typename... Types>
        struct replace_parameters<T<Types...>, Replacer> {
            using type = typename parameter_replacer<T, Replacer, fake_tuple<>, fake_tuple<Types...>>::type;
        };



        ////////////////////////////// recursively_change_parameter //////////////////////////////



        template <typename T, typename OldType, typename NewType>
        struct recursively_change_parameter;

        template <template <typename...> typename T, typename OldType, typename NewType, typename... Types>
        struct recursively_change_parameter<T<Types...>, OldType, NewType> {
            template <typename TT>
            struct recursively_change_parameter_replacer {
                static constexpr bool value = stl::is_same_v<TT, OldType>;
                using type                  = NewType;
            };

            using type = typename parameter_replacer<T,
                                                     recursively_change_parameter_replacer,
                                                     fake_tuple<>,
                                                     fake_tuple<Types...>>::type;
        };


        ////////////////////////////// recursively_change_templated_parameter //////////////////////////////

        template <template <typename...> typename OldType,
                  template <typename...>
                  typename NewType,
                  typename TT>
        struct recursively_change_templated_parameter_replacer {
            using type                  = void;
            static constexpr bool value = false;
        };

        template <template <typename...> typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Args>
        struct recursively_change_templated_parameter_replacer<OldType, NewType, OldType<Args...>> {
            using type                  = NewType<Args...>;
            static constexpr bool value = true;
        };

        template <typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType>
        struct recursively_change_templated_parameter;

        template <template <typename...> typename T,
                  template <typename...>
                  typename OldType,
                  template <typename...>
                  typename NewType,
                  typename... Types>
        struct recursively_change_templated_parameter<T<Types...>, OldType, NewType> {
            template <typename TT>
            struct replacer {
                using result = recursively_change_templated_parameter_replacer<OldType, NewType, TT>;
                using type   = typename result::type;
                static constexpr bool value = result::value;
            };

            using type = typename parameter_replacer<T, replacer, fake_tuple<>, fake_tuple<Types...>>::type;
        };


    } // namespace details

    /**
     * Replace a type parameter in a tuple-like from OldType to NewType
     * fixme: doesn't support CVREF
     */
    template <typename T, typename OldType, typename NewType>
    using replace_parameter = typename details::change_parameter<T, OldType, NewType>::type;

    /**
     * Replace a templated parameter in a tuple-like from OldType to NewType
     * fixme: doesn't support CVREF
     */
    template <typename T, template <typename...> typename OldType, template <typename...> typename NewType>
    using replace_templated_parameter =
      typename details::change_templated_parameter<T, OldType, NewType>::type;

    /**
     * Replace a type parameter in a tuple-like from OldType to NewType recursively
     */
    template <typename T, typename OldType, typename NewType>
    using recursively_replace_parameter =
      typename details::recursively_change_parameter<T, OldType, NewType>::type;

    /**
     * Replace a templated parameter in a tuple-like from OldType to NewType recursively
     */
    template <typename T, template <typename...> typename OldType, template <typename...> typename NewType>
    using recursively_replace_templated_parameter =
      typename details::recursively_change_templated_parameter<T, OldType, NewType>::type;


    /**
     * Replace parameters recursively;
     *   Replacer<CVREF_T>::value   -> if we should replace type CVREF_T
     *   Replacer<CVREF_T>::type    -> the type that we should replace it with
     */
    template <typename T, template <typename> typename Replacer>
    requires requires {
        typename Replacer<void>::type;
        {Replacer<void>::value};
    }
    using recursive_parameter_replacer = typename details::replace_parameters<T, Replacer>::type;


    /// Finds the size of a given tuple-like type.
    template <typename T>
    struct parameter_count_type;

    template <typename T>
    struct parameter_count_type<const T> : public parameter_count_type<T> {};

    template <typename T>
    struct parameter_count_type<volatile T> : public parameter_count_type<T> {};

    template <typename T>
    struct parameter_count_type<const volatile T> : public parameter_count_type<T> {};

    /// class tuple_size
    template <template <typename...> typename TupleT, typename... Elements>
    struct parameter_count_type<TupleT<Elements...>>
      : public stl::integral_constant<stl::size_t, sizeof...(Elements)> {};


    template <typename T>
    static constexpr stl::size_t parameter_count = parameter_count_type<T>::value;


    /// Gives the type of the ith element of a given tuple type.
    template <stl::size_t Index, typename T>
    struct nth_parameter_type;

    template <stl::size_t Index, typename T>
    struct nth_parameter_type<Index, const T>
      : public stl::add_const<typename nth_parameter_type<Index, T>::type> {};

    template <stl::size_t Index, typename T>
    struct nth_parameter_type<Index, volatile T>
      : public stl::add_volatile<typename nth_parameter_type<Index, T>::type> {};

    template <stl::size_t Index, typename T>
    struct nth_parameter_type<Index, const volatile T>
      : public stl::add_cv<typename nth_parameter_type<Index, T>::type> {};

    // recursive case
    template <template <typename...> typename TupleT, stl::size_t I, typename Head, typename... Tail>
    struct nth_parameter_type<I, TupleT<Head, Tail...>> : nth_parameter_type<I - 1, TupleT<Tail...>> {};

    // base case
    template <template <typename...> typename TupleT, typename Head, typename... Tail>
    struct nth_parameter_type<0, TupleT<Head, Tail...>> {
        using type = Head;
    };

    template <stl::size_t Index, typename T>
    using nth_parameter = typename nth_parameter_type<Index, T>::type;


    /**
     * Check if the type T is one of the TupleT's elements.
     * It's an alternative to "tuple_contains" in the tuple.hpp (more generalized version actually)
     */
    template <typename TupleT, typename T, stl::size_t I = parameter_count<TupleT> - 1>
    struct contains_parameter_type {
        static constexpr bool value = false;
    };

    template <typename TupleT, typename T, stl::size_t I>
    requires(parameter_count<TupleT> > 0) struct contains_parameter_type<TupleT, T, I> {
        static constexpr bool value =
          stl::is_same_v<nth_parameter<I, TupleT>, T> || contains_parameter_type<TupleT, T, I - 1>::value;
    };


    template <typename TupleT, typename T>
    struct contains_parameter_type<TupleT, T, 0> {
        static constexpr bool value = stl::is_same_v<nth_parameter<0, TupleT>, T>;
    };

    template <typename TupleT, typename T>
    static constexpr bool contains_parameter = contains_parameter_type<TupleT, T>::value;


    /**
     * With the help of this, you can replace a "placeholder" type with the new object you give it.
     * It's good for using in codes like this:
     *
     * @code
     *   struct placeholder{};
     *
     *   template <typename ...Args>
     *   void replace(Args&&...args) {
     *      return something{replace_object<placeholder, new_type>(stl::forward<Args>(args), new_obj)...};
     *   }
     * @endcode
     */
    template <typename OldType, typename NewType, typename T>
    static constexpr auto replace_object(T&& obj, NewType const& new_obj) noexcept {
        if constexpr (stl::same_as<OldType, T>) {
            return new_obj;
        } else {
            return stl::forward<T>(obj);
        }
    }


    // todo: add replace_parameter_all as well

    // details for filter_parameters
    namespace details {
        template <template <typename...> typename Concept,
                  typename Head,
                  typename Tail,
                  template <typename...>
                  typename TupleType>
        struct filter_parameters_impl;

        // moving This from Heads to the Tails list
        template <template <typename...> typename Concept,
                  typename... Heads,
                  typename This,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
        requires(Concept<This>::value) struct filter_parameters_impl<Concept,
                                                                     TupleType<This, Heads...>,
                                                                     TupleType<Tails...>,
                                                                     TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<This, Tails...>, TupleType> {
        };

        // remove the first one
        template <template <typename...> typename Concept,
                  typename... Heads,
                  typename This,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
        requires(!Concept<This>::value) struct filter_parameters_impl<Concept,
                                                                      TupleType<This, Heads...>,
                                                                      TupleType<Tails...>,
                                                                      TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<Tails...>, TupleType> {
        };

        // We're at the end of the line, no Heads left to check
        template <template <typename...> typename Concept,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
        struct filter_parameters_impl<Concept, TupleType<>, TupleType<Tails...>, TupleType> {
            using type = TupleType<Tails...>;
        };
    } // namespace details

    /**
     * Filter tuple types based on the Concept templated type (which contains a bool "value")
     * Example of the Concept type:
     *
     * @code
     *   template <typename T>
     *   struct is_not_void {
     *      static constexpr bool value = !std::is_void_v<T>; // or use a C++20 requires clause here
     *   };
     * @endcode
     */
    template <template <typename...> typename Concept, typename Tup>
    struct filter_parameters;

    template <template <typename...> typename Concept,
              typename... Types,
              template <typename...>
              typename TupleType>
    struct filter_parameters<Concept, TupleType<Types...>>
      : public details::filter_parameters_impl<Concept, TupleType<Types...>, TupleType<>, TupleType> {};


    template <template <typename...> typename Concept, typename Tup>
    using filter_parameters_t = typename filter_parameters<Concept, Tup>::type;


    template <typename Tup>
    using first_parameter = nth_parameter<0, Tup>;
    // todo: add last_parameter as well


    /**
     * Merge two or more tuple-like types.
     */
    template <typename... T>
    struct merge_parameters_type;

    // one tuple
    template <template <typename> typename TupleType, typename... FirstItems>
    struct merge_parameters_type<TupleType<FirstItems...>> {
        using type = TupleType<FirstItems...>;
    };

    // two tuples
    template <template <typename> typename TupleType, typename... FirstItems, typename... SecondItems>
    struct merge_parameters_type<TupleType<FirstItems...>, TupleType<SecondItems...>> {
        using type = TupleType<FirstItems..., SecondItems...>;
    };

    // more than 2 tuples
    template <typename First, typename Second, typename Third, typename... Rest>
    struct merge_parameters_type<First, Second, Third, Rest...>
      : merge_parameters_type<typename merge_parameters_type<First, Second>::type, Third, Rest...> {};

    template <typename... T>
    using merge_parameters = typename merge_parameters_type<T...>::type;



    /**
     * Same as stl::negation, but it takes a templated class instead of a class.
     */
    template <template <typename...> typename ConceptType>
    struct templated_negation {
        template <typename... T>
        using type = stl::negation<ConceptType<T...>>;
    };

    /**
     * Replace TupleT<OldT...> with TupleT<NewT...>
     */
    template <typename TupleT, typename... T>
    struct rebind_parameters;

    template <template <typename...> typename TupleT, typename... OldTs, typename... NewTs>
    struct rebind_parameters<TupleT<OldTs...>, NewTs...> {
        using type = TupleT<NewTs...>;
    };





    namespace details {

        template <typename TupleT, typename T>
        struct prepend;

        template <template <typename...> typename TupleT, typename F, typename... L>
        struct prepend<TupleT<L...>, F> {
            using type = TupleT<F, L...>;
        };

        template <typename TupleT, typename T>
        struct append;

        template <template <typename...> typename TupleT, typename F, typename... L>
        struct append<TupleT<L...>, F> {
            using type = TupleT<L..., F>;
        };

        struct special_unique_type;

        template <typename TupleT>
        struct unique_types;


        template <template <typename...> typename TupleT, typename First, typename... U>
        struct unique_types<TupleT<First, U...>> {
            using the_rest = typename unique_types<TupleT<U...>>::type;
            using type     = stl::conditional_t<((!stl::is_same_v<First, U>) &&...),
                                            typename prepend<the_rest, First>::type,
                                            the_rest>;
        };

        template <template <typename...> typename TupleT, typename... U>
        struct unique_types<TupleT<special_unique_type, U...>> {
            using type = TupleT<U...>;
        };

    } // namespace details


    template <typename TupleT, typename T>
    using prepend_parameter = typename details::prepend<TupleT, T>::type;

    template <typename TupleT, typename T>
    using append_parameter = typename details::append<TupleT, T>::type;

    template <typename T>
    using unique_parameters = typename details::unique_types<T>::type;


} // namespace webpp::istl

#endif // WEBPP_TYPE_TRAITS_HPP
