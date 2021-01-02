// Created by moisrex on 9/27/20.

#ifndef WEBPP_TYPE_TRAITS_HPP
#define WEBPP_TYPE_TRAITS_HPP

#include "./std.hpp"

#include <type_traits>
#include <utility> // for move and forward

namespace webpp::istl {

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


    namespace details {

        template <typename... T>
        struct fake_tuple {};

        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename T1,
                  typename T2>
        struct template_element;

        // recursive case (move from T1 tuple to T2 tuple if it's not a match)
        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename... Heads,
                  typename This,
                  typename... Tails>
        struct template_element<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<This, Tails...>>
          : template_element<T, OldType, NewType, fake_tuple<Heads..., This>, fake_tuple<Tails...>> {};

        // to check if it exists.
        template <template <typename...> typename T, typename OldType, typename NewType, typename... Heads>
        struct template_element<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<>> : stl::false_type {
            using type = void;
        };

        // base case (found the old type)
        template <template <typename...> typename T,
                  typename OldType,
                  typename NewType,
                  typename... Heads,
                  typename... Tails>
        struct template_element<T, OldType, NewType, fake_tuple<Heads...>, fake_tuple<OldType, Tails...>> {
            using type = T<Heads..., NewType, Tails...>;
        };


        template <typename T, typename OldType, typename NewType>
        struct change_template_parameter;

        template <template <typename...> typename T, typename OldType, typename NewType, typename... Types>
        struct change_template_parameter<T<Types...>, OldType, NewType> {
            using the_type = T<Types...>;
            using type     = typename details::
              template_element<T, OldType, NewType, fake_tuple<>, fake_tuple<Types...>>::type;
        };

    } // namespace details

    template <typename T, typename OldType, typename NewType>
    using replace_parameter = typename details::change_template_parameter<T, OldType, NewType>::type;


    /// Finds the size of a given tuple-like type.
    template <typename T>
    struct parameter_count;

    template <typename T>
    struct parameter_count<const T> : public parameter_count<T> {};

    template <typename T>
    struct parameter_count<volatile T> : public parameter_count<T> {};

    template <typename T>
    struct parameter_count<const volatile T> : public parameter_count<T> {};

    /// class tuple_size
    template <template <typename...> typename TupleT, typename... Elements>
    struct parameter_count<TupleT<Elements...>>
      : public stl::integral_constant<stl::size_t, sizeof...(Elements)> {};


    template <typename T>
    static constexpr stl::size_t parameter_count_v = parameter_count<T>::value;


    /// Gives the type of the ith element of a given tuple type.
    template <stl::size_t Index, typename T>
    struct get_parameter;

    template <stl::size_t Index, typename T>
    struct get_parameter<Index, const T> : public stl::add_const<typename get_parameter<Index, T>::type> {};

    template <stl::size_t Index, typename T>
    struct get_parameter<Index, volatile T>
      : public stl::add_volatile<typename get_parameter<Index, T>::type> {};

    template <stl::size_t Index, typename T>
    struct get_parameter<Index, const volatile T>
      : public stl::add_cv<typename get_parameter<Index, T>::type> {};

    // recursive case
    template <template <typename...> typename TupleT, stl::size_t I, typename Head, typename... Tail>
    struct get_parameter<I, TupleT<Head, Tail...>> : get_parameter<I - 1, TupleT<Tail...>> {};

    // base case
    template <template <typename...> typename TupleT, typename Head, typename... Tail>
    struct get_parameter<0, TupleT<Head, Tail...>> {
        using type = Head;
    };

    template <stl::size_t Index, typename T>
    using get_parameter_t = typename get_parameter<Index, T>::type;


    /**
     * Check if the type T is one of the TupleT's elements.
     * I'm short on internet bandwidth as of writing this; so forgive me if there's already another solution
     * of this in the STL, I don't have the luxury of searching it; so I'm just gonna implement it :)
     */
    template <typename TupleT, typename T, stl::size_t I = parameter_count_v<TupleT> - 1>
    struct contains_parameter {
        static constexpr bool value =
          stl::is_same_v<get_parameter_t<I, TupleT>, T> || contains_parameter<TupleT, T, I - 1>::value;
    };


    template <typename TupleT, typename T>
    struct contains_parameter<TupleT, T, 0> {
        static constexpr bool value = stl::is_same_v<get_parameter_t<0, TupleT>, T>;
    };

    template <typename TupleT, typename T>
    static constexpr bool contains_parameter_v = contains_parameter<TupleT, T>::value;


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
        template <template <typename> typename Concept,
                  typename Head,
                  typename Tail,
                  template <typename...>
                  typename TupleType>
        struct filter_parameters_impl;

        // moving This from Heads to the Tails list
        template <template <typename> typename Concept,
                  typename... Heads,
                  typename This,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
        requires(Concept<This>::value) struct filter_parameters_impl<Concept,
                                                                     TupleType<This, Heads...>,
                                                                     TupleType<Tails...>,
                                                                     TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<This, Tails...>, TupleType> {};

        // remove the first one
        template <template <typename> typename Concept,
                  typename... Heads,
                  typename This,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
        requires(!Concept<This>::value) struct filter_parameters_impl<Concept,
                                                                      TupleType<This, Heads...>,
                                                                      TupleType<Tails...>,
                                                                      TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<Tails...>, TupleType> {};

        // We're at the end of the line, no Heads left to check
        template <template <typename> typename Concept,
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
    template <template <typename> typename Concept, typename Tup>
    struct filter_parameters;

    template <template <typename> typename Concept,
              typename... Types,
              template <typename...>
              typename TupleType>
    struct filter_parameters<Concept, TupleType<Types...>>
      : public details::filter_parameters_impl<Concept, TupleType<Types...>, TupleType<>, TupleType> {};


    template <template <typename> typename Concept, typename Tup>
    using filter_parameters_t = typename filter_parameters<Concept, Tup>::type;


    /**
     * Merge two or more tuple-like types.
     */
    template <typename... T>
    struct merge_parameters;

    // one tuple
    template <template <typename> typename TupleType, typename... FirstItems>
    struct merge_parameters<TupleType<FirstItems...>> {
        using type = TupleType<FirstItems...>;
    };

    // two tuples
    template <template <typename> typename TupleType, typename... FirstItems, typename... SecondItems>
    struct merge_parameters<TupleType<FirstItems...>, TupleType<SecondItems...>> {
        using type = TupleType<FirstItems..., SecondItems...>;
    };

    // more than 2 tuples
    template <typename First, typename Second, typename Third, typename... Rest>
    struct merge_parameters<First, Second, Third, Rest...>
      : merge_parameters<typename merge_parameters<First, Second>::type, Third, Rest...> {};

    template <typename... T>
    using merge_parameters_t = typename merge_parameters<T...>::type;



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


} // namespace webpp::istl

#endif // WEBPP_TYPE_TRAITS_HPP
