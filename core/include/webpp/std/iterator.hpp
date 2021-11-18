// Created by moisrex on 10/7/21.

#ifndef WEBPP_STD_ITERATOR_HPP
#define WEBPP_STD_ITERATOR_HPP

#include "./concepts.hpp"

#include <iterator>

#if !__cpp_lib_concepts
namespace webpp::stl {

    // source: https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/include/bits/iterator_concepts.h

    namespace details {
        // An iterator such that iterator_traits<Iter> names a specialization
        // generated from the primary template.
        template <typename Iter>
        concept primary_traits_iter = is_base_of(iterator_traits<Iter, void>, iterator_traits<Iter>);

        template <typename Iter, typename T>
        struct iter_traits_impl {
            using type = iterator_traits<Iter>;
        };

        template <typename Iter, typename T>
        requires primary_traits_iter<Iter>
        struct iter_traits_impl<Iter, T> {
            using type = T;
        };

        // ITER_TRAITS
        template <typename Iter, typename T = Iter>
        using iter_traits = typename iter_traits_impl<Iter, T>::type;

        template <typename T>
        using iter_diff_t = typename iter_traits<T, incrementable_traits<T>>::difference_type;
    } // namespace details


    namespace details {
        template <typename Iter>
        struct iter_concept_impl;

        // ITER_CONCEPT(I) is ITER_TRAITS(I)::iterator_concept if that is valid.
        template <typename Iter>
        requires requires {
            typename iter_traits<Iter>::iterator_concept;
        }
        struct iter_concept_impl<Iter> {
            using type = typename iter_traits<Iter>::iterator_concept;
        };

        // Otherwise, ITER_TRAITS(I)::iterator_category if that is valid.
        template <typename Iter>
        requires(
          !requires { typename iter_traits<Iter>::iterator_concept; } &&
          requires { typename iter_traits<Iter>::iterator_category; }) struct iter_concept_impl<Iter> {
            using type = typename iter_traits<Iter>::iterator_category;
        };

        // Otherwise, random_access_tag if iterator_traits<I> is not specialized.
        template <typename Iter>
        requires(
          !requires { typename iter_traits<Iter>::iterator_concept; } &&
          !requires { typename iter_traits<Iter>::iterator_category; } &&
          primary_traits_iter<Iter>) struct iter_concept_impl<Iter> {
            using type = random_access_iterator_tag;
        };

        // Otherwise, there is no ITER_CONCEPT(I) type.
        template <typename Iter>
        struct iter_concept_impl {};

        // ITER_CONCEPT
        template <typename Iter>
        using iter_concept = typename iter_concept_impl<Iter>::type;

        template <typename _In>
        concept indirectly_readable_impl = requires {
            typename iter_value_t<_In>;
            typename iter_reference_t<_In>;
            typename iter_rvalue_reference_t<_In>;
            requires same_as<iter_reference_t<const _In>, iter_reference_t<_In>>;
            requires same_as<iter_rvalue_reference_t<const _In>, iter_rvalue_reference_t<_In>>;
        }
        &&common_reference_with<iter_reference_t<_In>&&, iter_value_t<_In>&>&&
            common_reference_with<iter_reference_t<_In>&&, iter_rvalue_reference_t<_In>&&>&&
            common_reference_with<iter_rvalue_reference_t<_In>&&, const iter_value_t<_In>&>;

    } // namespace details


    template <typename Iter>
    concept random_access_iterator = bidirectional_iterator<Iter> &&
      derived_from<details::iter_concept<Iter>, random_access_iterator_tag> && totally_ordered<Iter> &&
      sized_sentinel_for<Iter, Iter> && requires(Iter i, const Iter j, const iter_difference_t<Iter> n) {
        { i += n } -> same_as<Iter&>;
        { j + n } -> same_as<Iter>;
        { n + j } -> same_as<Iter>;
        { i -= n } -> same_as<Iter&>;
        { j - n } -> same_as<Iter>;
        { j[n] } -> same_as<iter_reference_t<Iter>>;
    };

} // namespace webpp::stl

#endif // __cpp_lib_concepts

#endif // WEBPP_STD_ITERATOR_HPP
