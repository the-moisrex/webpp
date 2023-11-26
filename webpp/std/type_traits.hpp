// Created by moisrex on 9/27/20.

#ifndef WEBPP_TYPE_TRAITS_HPP
#define WEBPP_TYPE_TRAITS_HPP

#include "std.hpp"

#include <cstdint> // for std::size_t and what not
#include <type_traits>
#include <utility> // for move and forward

namespace webpp::istl {

    // NOLINTBEGIN(cppcoreguidelines-macro-usage)

    // Used where a tuple is needed, but you don't really want all features of tuple
    template <typename...>
    struct type_list {};

    /**
     * This can be used for when you need a quick anonymous struct within an expression.
     * Example:
     *
     *   using test = typename anonymous_struct({
     *       using one = ...;
     *       using two = ...;
     *       using type = ...;
     *   })::type;
     */
#define anonymous_struct(...)    \
    decltype([] {                \
        struct __VA_ARGS__ impl; \
        return impl;             \
    }())

    struct nothing_type {};

    template <typename T>
    using void_holder = stl::conditional_t<stl::is_void_v<T>, nothing_type, T>;

    template <typename T>
    struct type_holder {
        using type = T;
    };

    template <auto V>
    struct value_holder {};

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
            type                         value; // NOLINT(misc-non-private-member-variables-in-classes)

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
            static constexpr stl::int64_t rank  = ConditionOp<T>::value;
            static constexpr stl::size_t  index = Index;
            using type                          = T;

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
            constexpr auto operator|(NextType next) const noexcept {
                if constexpr (rank < NextType::rank) {
                    return *this;
                } else {
                    return next.add_one();
                }
            }

            template <typename NextType>
            constexpr auto operator&(NextType next) const noexcept {
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

    /**
     * Replace T's template typename with the NewTempl.
     * For example replace std::tuple<...> with std::variant<...>
     */
    template <typename T, template <typename...> typename NewType>
    using template_swap = typename template_swap_type<T, NewType>::type;


    template <typename T, template <typename> typename Transformer>
    struct transform_parameters_type;

    template <template <typename...> typename T, template <typename> typename Transformer, typename... Args>
    struct transform_parameters_type<T<Args...>, Transformer> {
        using type = T<typename Transformer<Args>::type...>;
    };

    template <typename T, template <typename> typename Transformer>
    using transform_parameters = typename transform_parameters_type<T, Transformer>::type;

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

// NOLINTBEGIN(bugprone-macro-parentheses)
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
                     stl::conditional_t<Replacer<This>::value, typename Replacer<This>::type, This> CVREF>, \
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

// NOLINTEND(bugprone-macro-parentheses)
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

        template <typename T, template <typename> typename Replacer>
        using repalce_parameter_t = typename replace_parameters<T, Replacer>::type;


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
            {
                Replacer<void>::value
            };
        }
    using recursive_parameter_replacer = typename details::replace_parameters<T, Replacer>::type;


    /// Finds the size of a given tuple-like type.
    template <typename T>
    struct parameter_count_type;

    /// class tuple_size
    template <template <typename...> typename TupleT, typename... Elements>
    struct parameter_count_type<TupleT<Elements...>>
      : public stl::integral_constant<stl::size_t, sizeof...(Elements)> {};

    template <typename T>
    static constexpr stl::size_t parameter_count = parameter_count_type<stl::remove_cvref_t<T>>::value;


    /// Gives the type of the ith element of a given tuple type.
    template <stl::size_t Index, typename... T>
    struct nth_parameter;

    // recursive case
    template <stl::size_t I, typename Head, typename... Tail>
    struct nth_parameter<I, Head, Tail...> : nth_parameter<I - 1, Tail...> {};

    // base case
    template <typename Head, typename... Tail>
    struct nth_parameter<0, Head, Tail...> {
        using type = Head;
    };

    template <stl::size_t Index, typename... T>
    using nth_parameter_t = typename nth_parameter<Index, T...>::type;

    template <stl::size_t Index, typename T>
    struct nth_parameter_of;

    template <stl::size_t Index, template <typename...> typename Templ, typename... T>
    struct nth_parameter_of<Index, Templ<T...>> : nth_parameter<Index, T...> {};

    // this same as std::tuple_element_t, but it supports any type of tuple not just std::tuple
    template <stl::size_t Index, typename T>
    using nth_parameter_of_t = typename nth_parameter_of<Index, T>::type;

    /**
     * Check if the type T is one of the TupleT's elements.
     * It's an alternative to "tuple_contains" in the tuple.hpp (more generalized version actually)
     */
    template <typename Find, typename... T>
    struct contains_parameter : stl::false_type {};

    template <typename Find, typename... T>
    struct contains_parameter<Find, Find, T...> : stl::true_type {};

    template <typename Find, typename Head, typename... T>
    struct contains_parameter<Find, Head, T...> : contains_parameter<Find, T...> {};

    namespace details {
        template <typename Tup, typename Find, typename... T>
        struct contains_parameter_of;

        template <template <typename...> typename TupTempl, typename Find, typename... T>
        struct contains_parameter_of<TupTempl<T...>, Find> : contains_parameter<Find, T...> {};
    } // namespace details

    template <typename TupleT, typename T>
    concept contains_parameter_of = details::contains_parameter_of<TupleT, T>::value;

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
        // Can't use same_as because it's going to be a circular dependency with <concepts> file
        if constexpr (stl::is_same_v<OldType, T> && stl::is_same_v<T, OldType>) {
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
            requires(Concept<This>::value)
        struct filter_parameters_impl<Concept, TupleType<This, Heads...>, TupleType<Tails...>, TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<Tails..., This>, TupleType> {};

        // remove the first one
        template <template <typename...> typename Concept,
                  typename... Heads,
                  typename This,
                  typename... Tails,
                  template <typename...>
                  typename TupleType>
            requires(!Concept<This>::value)
        struct filter_parameters_impl<Concept, TupleType<This, Heads...>, TupleType<Tails...>, TupleType>
          : filter_parameters_impl<Concept, TupleType<Heads...>, TupleType<Tails...>, TupleType> {};

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
    using first_parameter = nth_parameter_of_t<0, Tup>;

    // todo: add last_parameter as well

    /**
     * Get the first template parameter in the specified variadic template or void if nothing is there.
     */
    template <typename...>
    struct first_type {
        using type = void;
    };

    template <typename T>
    struct first_type<T> {
        using type = T;
    };

    template <typename... T>
    using first_type_t = typename first_type<T...>::type;

    /**
     * Get the last parameter type from a tuple-like type
     */
    template <typename... T>
    struct last_type {
        static_assert(sizeof...(T) > 0, "last_type won't work with empty type lists.");

        template <typename... FT>
        struct fake_tup {
            template <typename... L>
            using append = fake_tup<FT..., L...>;

            template <template <typename...> typename Tt, typename... Additional>
            using replace_template = Tt<FT..., Additional...>;

            template <template <typename...> typename Tt, typename... Additional>
            using replace_template_prepend = Tt<Additional..., FT...>;

            static constexpr stl::size_t size = sizeof...(FT);
        };

        template <typename TagT, typename Tup = fake_tup<>, stl::size_t Limit = static_cast<stl::size_t>(-1)>
        struct tag {
            // last type
            using last = TagT;

            // the rest of the types
            using rest = Tup;

            // all of them
            using all = typename rest::template append<last>;

            static constexpr stl::size_t all_size = all::size;

            // use TagT as the last type
            template <typename Tag>
                requires(all_size < Limit)
            constexpr tag<typename Tag::last, all> operator|(
              [[maybe_unused]] Tag&& inp_tag) const noexcept { // NOLINT(*-missing-std-forward)
                return {};
            }

            template <typename Tag>
            constexpr tag operator|(
              [[maybe_unused]] Tag&& inp_tag) const noexcept { // NOLINT(*-missing-std-forward)
                return {};
            }
        };

        // last type
        using type = typename decltype((tag<T>{}, ...))::last;

        // all except last type (remove the last type)
        template <template <typename...> typename Tt>
        using remove = typename decltype((... | tag<T>{}))::rest::template replace_template<Tt>;

        // put the last type first, and the rest is the rest
        template <template <typename...> typename Tt>
        using rotate = typename decltype((... | tag<T>{}))::rest::template replace_template_prepend<Tt, type>;

        // remove the last types ao there's only N types in the tuple
        template <template <typename...> typename Tt, stl::size_t N>
        using remove_limit =
          typename decltype((... | tag<T, fake_tup<>, N>{}))::all::template replace_template<Tt>;

        // remove the last type if
        template <template <typename...> typename Tt, template <typename> typename Condition>
        using remove_if =
          stl::conditional_t<Condition<type>::value,
                             typename decltype((... | tag<T>{}))::rest::template replace_template<Tt>,
                             Tt<T...>>;

        // replace last type
        template <template <typename...> typename Tt, typename... Replacements>
        using replace =
          typename decltype((... | tag<T>{}))::rest::template replace_template<Tt, Replacements...>;

        // replace last type if
        template <template <typename...> typename Tt,
                  template <typename>
                  typename Condition,
                  typename... Replacements>
        using replace_if = stl::conditional_t<Condition<type>::value, replace<Tt, Replacements...>, Tt<T...>>;


        // replace if exists, add if it doesn't
        template <template <typename...> typename Tt,
                  template <typename>
                  typename Condition,
                  typename... Replacements>
        using put_if =
          stl::conditional_t<Condition<type>::value, replace<Tt, Replacements...>, Tt<T..., Replacements...>>;
    };

    template <>
    struct last_type<> {
        // last type
        using type = void;

        // all except last type (remove the last type)
        template <template <typename...> typename Tt>
        using remove = Tt<>;

        // put the last type first, and the rest is the rest
        template <template <typename...> typename Tt>
        using rotate = Tt<>;

        // remove the last types ao there's only N types in the tuple
        template <template <typename...> typename Tt, stl::size_t>
        using remove_limit = Tt<>;

        // remove the last type if
        template <template <typename...> typename Tt, template <typename> typename>
        using remove_if = Tt<>;

        template <template <typename...> typename Tt, typename... Replacements>
        using replace = Tt<Replacements...>;

        // replace last type if
        template <template <typename...> typename Tt,
                  template <typename>
                  typename Condition,
                  typename... Replacements>
        using replace_if = stl::conditional_t<Condition<type>::value, replace<Tt, Replacements...>, Tt<>>;


        // replace if exists, add if it doesn't
        template <template <typename...> typename Tt,
                  template <typename>
                  typename Condition,
                  typename... Replacements>
        using put_if = Tt<Replacements...>;
    };

    template <typename... T>
    using last_type_t = typename last_type<T...>::type;



    /**
     * Merge two or more tuple-like types.
     */
    template <typename... T>
    struct merge_parameters_type;

    // one tuple
    template <template <typename...> typename TupleType, typename... FirstItems>
    struct merge_parameters_type<TupleType<FirstItems...>> {
        using type = TupleType<FirstItems...>;
    };

    // two tuples
    template <template <typename...> typename TupleType, typename... FirstItems, typename... SecondItems>
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

        template <typename TupleT, typename... T>
        struct append;

        template <template <typename...> typename TupleT, typename... F, typename... L>
        struct append<TupleT<L...>, F...> {
            using type = TupleT<L..., F...>;
        };

        template <typename TupleT>
        struct unique_types;

        template <template <typename...> typename TupleT, typename First, typename... U>
        struct unique_types<TupleT<First, U...>> {
            using the_rest = typename unique_types<TupleT<U...>>::type;
            using type     = stl::conditional_t<((!stl::is_same_v<First, U>) &&...),
                                            typename prepend<the_rest, First>::type,
                                            the_rest>;
        };

        // the end condition
        template <template <typename...> typename TupleT>
        struct unique_types<TupleT<>> {
            using type = TupleT<>;
        };

    } // namespace details

    // prepend the specified type T at the beginning of the specified tuple-like type "TupleT"
    template <typename TupleT, typename T>
    using prepend_parameter = typename details::prepend<TupleT, T>::type;

    // append the specified type T at the end of tuple-like type TupleT
    template <typename TupleT, typename... T>
    using append_parameter = typename details::append<TupleT, T...>::type;

    // make the template parameters of the specified tuple-like type, "unique" (remove the duplicates)
    template <typename T>
    using unique_parameters = typename details::unique_types<T>::type;


    // initially from: http://open-std.org/JTC1/SC22/WG21/docs/papers/2020/p2098r1.pdf
#define WEBPP_COMMA           ,
#define WEBPP_SINGLE_ARG(...) __VA_ARGS__
#define define_is_specialization_of(name, types, types_with_names, names) \
    template <typename T, template <types> typename Primary>              \
    struct name : stl::false_type {};                                     \
                                                                          \
    template <template <types> typename Primary, types_with_names>        \
    struct name<Primary<names>, Primary> : stl::true_type {};             \
                                                                          \
    template <typename T, template <types> typename Primary>              \
    concept name##_v = WEBPP_SINGLE_ARG(name)<T, Primary>::value

    define_is_specialization_of(is_specialization_of, typename..., typename... Args, Args...);

    // valued specialization is exactly the same as above, but it supports some auto values as well
    // this has a very limited use case, so don't worry about it if it seems useless to you.
    template <typename T, template <auto, typename...> typename Primary>
    struct is_valued_specialization_of : stl::false_type {};

    template <template <auto, typename...> typename Primary, auto Val, typename... Args>
    struct is_valued_specialization_of<Primary<Val, Args...>, Primary> : stl::true_type {};

    template <typename T, template <auto, typename...> typename Primary>
    concept is_valued_specialization_of_v = is_valued_specialization_of<T, Primary>::value;

    namespace details {

        template <typename FirstType, typename TheType, stl::size_t N>
        struct repeat_type : repeat_type<append_parameter<FirstType, TheType>, TheType, N - 1> {};

        template <typename FirstType, typename TheType>
        struct repeat_type<FirstType, TheType, 0> {
            using type = FirstType;
        };
    } // namespace details

    // Repeat Template Type
    template <stl::size_t N,
              typename TheType,
              template <typename...>
              typename TemplateType,
              typename... FirstTypes>
    using repeat_type = details::repeat_type<TemplateType<FirstTypes...>, TheType, N>;

    template <stl::size_t N,
              typename TheType,
              template <typename...>
              typename TemplateType,
              typename... FirstTypes>
    using repeat_type_t = typename repeat_type<N, TheType, TemplateType, FirstTypes...>::type;

    namespace details {
        /**
         * Get a list of all indexes that `Evaluator<T>::value` is true
         */
        template <template <typename> typename Evaluator,
                  stl::size_t From,
                  typename TypeList,
                  stl::size_t... Indexes>
        struct indexes_if;

        // No more
        template <template <typename> typename Evaluator, stl::size_t LastIndex, stl::size_t... Indexes>
        struct indexes_if<Evaluator, LastIndex, type_list<>, Indexes...> {
            using type = stl::index_sequence<Indexes...>;
        };

        // still looking
        template <template <typename> typename Evaluator,
                  stl::size_t From,
                  typename F,
                  typename... T,
                  stl::size_t... Indexes>
        struct indexes_if<Evaluator, From, type_list<F, T...>, Indexes...>
          : indexes_if<Evaluator, From + 1, type_list<T...>, Indexes...> {};

        // found another one
        template <template <typename> typename Evaluator,
                  stl::size_t From,
                  typename F,
                  typename... T,
                  stl::size_t... Indexes>
            requires(Evaluator<F>::value)
        struct indexes_if<Evaluator, From, type_list<F, T...>, Indexes...>
          : indexes_if<Evaluator, From + 1, type_list<T...>, Indexes..., From> {};


        template <template <typename> typename Evaluator, typename T>
        struct indexes_if_of;

        template <template <typename> typename Evaluator,
                  template <typename...>
                  typename Templ,
                  typename... T>
        struct indexes_if_of<Evaluator, Templ<T...>> : details::indexes_if<Evaluator, 0, type_list<T...>> {};

    } // namespace details

    template <template <typename> typename Evaluator, typename... T>
    using indexes_if = typename details::indexes_if<Evaluator, 0, type_list<T...>>::type;


    template <template <typename> typename Evaluator, typename T>
    using indexes_if_of = typename details::indexes_if_of<Evaluator, T>::type;

    template <typename...>
    struct index_of_type;

    // found it
    template <typename T, typename... R>
    struct index_of_type<T, T, R...> : stl::integral_constant<stl::size_t, 0> {};

    // still looking
    template <typename T, typename F, typename... R>
    struct index_of_type<T, F, R...>
      : stl::integral_constant<stl::size_t, 1 + index_of_type<T, R...>::value> {};



    template <auto...>
    struct index_of_item;

    // found it
    template <auto T, auto... R>
    struct index_of_item<T, T, R...> : stl::integral_constant<stl::size_t, 0> {};

    // still looking
    template <auto T, auto F, auto... R>
    struct index_of_item<T, F, R...>
      : stl::integral_constant<stl::size_t, 1 + index_of_item<T, R...>::value> {};


    template <typename T1, typename... T>
    struct integer_sequence_cat;

    template <typename Type, Type... I1, Type... I2, typename... Rest>
    struct integer_sequence_cat<stl::integer_sequence<Type, I1...>,
                                stl::integer_sequence<Type, I2...>,
                                Rest...>
      : integer_sequence_cat<stl::integer_sequence<Type, I1..., I2...>, Rest...> {};

    template <typename Type, Type... I>
    struct integer_sequence_cat<stl::integer_sequence<Type, I...>> {
        using type = stl::integer_sequence<Type, I...>;
    };

    template <typename T1, typename... T>
    using integer_sequence_cat_t = typename integer_sequence_cat<T1, T...>::type;

    namespace details {
        template <typename T, std::size_t = sizeof(T)>
        stl::true_type is_complete_impl(T*);

        stl::false_type is_complete_impl(...);
    } // namespace details

    /**
     * Check if the type is complete or not
     * Attention: the completeness of a type most-likely will be decided the first time you use it.
     * https://twitter.com/the_moisrex/status/1561091267592949763?s=20&t=G5IX6SAPkOoBY1pwQmVRLA
     */
    template <typename T>
    using is_complete = decltype(details::is_complete_impl(stl::declval<T*>()));

    template <typename T>
    concept is_complete_v = is_complete<T>::value;

    /**
     * Call a callable with the specified args, no matter in which order the arguments are specified.
     * @tparam Callable
     * @tparam Args
     */
    template <typename Callable, typename... Args>
    struct invocable_inorder : stl::false_type {
        static constexpr bool is_nothrow = false;
        using result                     = void;

        template <typename C>
        static constexpr auto call([[maybe_unused]] C callable, [[maybe_unused]] Args... args) {
            // static_assert_false
            static_assert(false && !sizeof(C*),
                          "The specified callable cannot be called with the specified arguments.");
        }
    };

    // valid args
    template <typename Callable, typename... Args>
        requires(stl::is_invocable_v<Callable, Args...>)
    struct invocable_inorder<Callable, Args...> : stl::true_type {
        using result                     = stl::invoke_result_t<Callable, Args...>;
        static constexpr bool is_nothrow = stl::is_nothrow_invocable_v<Callable, Args...>;

        static constexpr decltype(auto) call(Callable callable, Args... args) noexcept(is_nothrow) {
            return stl::forward<Callable>(callable)(stl::forward<Args>(args)...);
        }
    };

    // rotate args once
    template <typename Callable, typename Arg1, typename... Args>
        requires(
          invocable_inorder<Callable, Args..., Arg1>::value && !invocable_inorder<Callable, Args...>::value)
    struct invocable_inorder<Callable, Arg1, Args...> : invocable_inorder<Callable, Args..., Arg1> {
        using parent_type = invocable_inorder<Callable, Args..., Arg1>;

        static constexpr decltype(auto) call(Callable callable, Arg1 arg1, Args... args) noexcept(
          parent_type::is_nothrow) {
            return parent_type::call(stl::forward<Callable>(callable),
                                     stl::forward<Args>(args)...,
                                     stl::forward<Arg1>(arg1));
        }
    };

    // switch first and second args
    template <typename Callable, typename Arg1, typename Arg2, typename... Args>
        requires(invocable_inorder<Callable, Arg2, Arg1, Args...>::value)
    struct invocable_inorder<Callable, Arg1, Arg2, Args...>
      : invocable_inorder<Callable, Arg2, Arg1, Args...> {
        using parent_type = invocable_inorder<Callable, Arg2, Arg1, Args...>;

        static constexpr decltype(auto) call(Callable callable, Arg1 arg1, Arg2 arg2, Args... args) noexcept(
          parent_type::is_nothrow) {
            return parent_type::call(
              stl::forward<Callable>(callable),
              stl::forward<Arg2>(arg2),
              stl::forward<Arg1>(arg1),
              stl::forward<Args>(args)...);
        }
    };

    // ignore the first argument
    template <typename Callable, typename Arg1, typename... Args>
        requires(invocable_inorder<Callable, Args...>::value)
    struct invocable_inorder<Callable, Arg1, Args...> : invocable_inorder<Callable, Args...> {
        using parent_type = invocable_inorder<Callable, Args...>;

        static constexpr decltype(auto) call(Callable callable, Arg1, Args... args) noexcept(
          parent_type::is_nothrow) {
            return parent_type::call(stl::forward<Callable>(callable), stl::forward<Args>(args)...);
        }
    };

    template <typename Callable, typename... Args>
    concept invocable_inorder_v = invocable_inorder<Callable, Args...>::value;

    template <typename Callable, typename... Args>
    static constexpr decltype(auto) invoke_inorder(Callable&& callable, Args&&... args) noexcept(
      invocable_inorder<Callable, Args...>::is_nothrow) {
        return invocable_inorder<Callable, Args...>::call(stl::forward<Callable>(callable),
                                                          stl::forward<Args>(args)...);
    }

    /**
     * Give 2 param list, and check if Evaluator<L, R>::value on all of them are true or not
     *
     * @code
     *   static constexpr bool value = are_all<std::is_convertible, tuple<int, double>, tuple<double, int>>;
     * @endcode
     *
     * todo: we can manage to remove the necessity of "tuple-like" types here by splitting the values in half
     */
    template <template <typename, typename> typename Evaluator, typename, typename>
    struct are_all : stl::false_type {};

    // true condition, all are checked
    template <template <typename, typename> typename Evaluator, template <typename...> typename T>
    struct are_all<Evaluator, T<>, T<>> : stl::true_type {};

    // check one
    template <template <typename, typename> typename Evaluator,
              template <typename...>
              typename T,
              typename... L,
              typename... R,
              typename L1,
              typename R1>
        requires(sizeof...(L) == sizeof...(R) && Evaluator<L1, R1>::value)
    struct are_all<Evaluator, T<L1, L...>, T<R1, R...>> : are_all<Evaluator, T<L...>, T<R...>> {};

    template <template <typename, typename> typename Evaluator, typename L, typename R>
    concept are_all_v = are_all<Evaluator, L, R>::value;

    /**
     * Remove Templ out of T if it T is `Templ<NotT>`
     * @tparam Templ
     * @tparam T
     */
    template <template <typename> typename Templ, typename T>
    struct remove_template_of : stl::false_type {
        using type = T;
    };

    template <template <typename> typename Templ, typename T>
    struct remove_template_of<Templ, Templ<T>> : stl::true_type {
        using type = T;
    };

    template <template <typename> typename Templ, typename T>
    using remove_template_of_t = typename remove_template_of<Templ, T>::type;

    template <template <typename> typename Templ, typename T>
    concept remove_template_of_v = remove_template_of<Templ, T>::value;

    /**
     * Remove Any template out of T if it T is `Templ<NotT>`
     * @tparam T
     */
    template <typename T>
    struct remove_template : stl::false_type {
        using type = T;
    };

    template <template <typename> typename Templ, typename T>
    struct remove_template<Templ<T>> : stl::true_type {
        using type = T;
    };

    template <typename T>
    using remove_template_t = typename remove_template<T>::type;

    template <typename T>
    concept remove_template_v = remove_template<T>::value;

    /**
     * Check if T is `Templ<...>`
     * @tparam Templ
     * @tparam T
     */
    template <template <typename...> typename Templ, typename T>
    struct template_of : stl::false_type {};

    template <template <typename...> typename Templ, typename... T>
    struct template_of<Templ, Templ<T...>> : stl::true_type {};

    template <template <typename...> typename Templ, typename T>
    concept template_of_v = template_of<Templ, T>::value;

    /**
     * Pass Templ a non-cvref type, get the Templ<T>::type, then add the cvref again
     */
    template <template <typename> typename Templ, typename T>
    struct preserve_cvref {
        using type = typename Templ<T>::type;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T volatile> {
        using type = volatile typename Templ<T>::type;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T const> {
        using type = const typename Templ<T>::type;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T&> {
        using type = typename Templ<T>::type&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T&&> {
        using type = typename Templ<T>::type&&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, volatile T const> {
        using type = const volatile typename Templ<T>::type;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T const&> {
        using type = const typename Templ<T>::type&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T const&&> {
        using type = const typename Templ<T>::type&&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T volatile&> {
        using type = volatile typename Templ<T>::type&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, T volatile&&> {
        using type = volatile typename Templ<T>::type&&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, volatile T const&> {
        using type = const volatile typename Templ<T>::type&;
    };

    template <template <typename> typename Templ, typename T>
    struct preserve_cvref<Templ, volatile T const&&> {
        using type = const volatile typename Templ<T>::type&&;
    };

    template <template <typename> typename Templ, typename T>
    using preserve_cvref_t = typename preserve_cvref<Templ, T>::type;

    namespace details {
        // NOLINTBEGIN(*-runtime-int)
        template <typename T>
        struct remove_unsigned {
            using type = T;
        };

        template <>
        struct remove_unsigned<unsigned char> {
            using type = char;
        };

        template <>
        struct remove_unsigned<unsigned short> {
            using type = short;
        };

        template <>
        struct remove_unsigned<unsigned int> {
            using type = int;
        };

        template <>
        struct remove_unsigned<unsigned long> {
            using type = long;
        };

        template <>
        struct remove_unsigned<unsigned long long> {
            using type = long long;
        };

        // NOLINTEND(*-runtime-int)
    } // namespace details

    /**
     * Exactly like std::make_signed except it's not ill-formed when non-integral types are given.
     * Also, make_signed adds "signed" to it but we don't, we just remove the "unsigned"
     */
    template <typename T>
    struct remove_unsigned : preserve_cvref<details::remove_unsigned, T> {};

    template <typename T>
    using remove_unsigned_t = typename remove_unsigned<T>::type;

    template <typename T>
    struct remove_rvalue_reference {
        using type = T;
    };

    template <typename T>
    struct remove_rvalue_reference<T&&> {
        using type = T;
    };

    template <typename T>
    using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

    // NOLINTEND(cppcoreguidelines-macro-usage)
} // namespace webpp::istl

#endif // WEBPP_TYPE_TRAITS_HPP
