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

        template <bool Condition, template<typename...> typename Extractor, typename...Args>
        struct templated_lazy_evaluate {
            using type = void;
        };

        template <template<typename...>typename Extractor, typename ...Args>
        struct templated_lazy_evaluate<true, Extractor, Args...> {
            using type = typename Extractor<Args...>::type;
        };
    }

    template <template <typename...> typename Extractor, typename ...Args>
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
            constexpr auto add_one() noexcept {
            }

            template <typename NextType>
            constexpr auto operator|(NextType&& next) const noexcept {
                if constexpr (ThisIndex == Index) {
                    return *this;
                } else {
                    return nth_of<Index, NextType, ThisIndex + 1>{.value = stl::forward<NextType>(next)};
                }
            }
        };
    }

    template <stl::size_t Index, typename First, typename ...T>
    [[nodiscard]] constexpr auto nth_of(First&& first, T&& ...objs) noexcept {
        static_assert(Index < (sizeof...(T) + 1), "The specified Index is out of range");
        return ((details::nth_of<Index, First>{.value = stl::forward<First>(first)}) | ... | stl::forward<T>(objs)).value;
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
     * template <typename T>
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
    template <template<typename> typename ConditionOp, typename ...Types>
    struct ranked_types {

        static_assert(
        (stl::is_integral_v<stl::remove_cvref_t<decltype(ConditionOp<Types>::value)>> && ...),
          "The specified Condition is not valid"
        );

        template <typename T, stl::size_t Index = 0>
        struct a_type {
            static constexpr long long int rank = ConditionOp<T>::value;
            static constexpr stl::size_t index = Index;
            using type = T;

            template <typename ...ItemType>
            [[nodiscard]] static constexpr auto get(ItemType&& ...item) noexcept {
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


        using best = decltype((a_type<Types>() & ...));
        using worst = decltype((a_type<Types>() | ...));

    };



}

#endif // WEBPP_TYPE_TRAITS_HPP
