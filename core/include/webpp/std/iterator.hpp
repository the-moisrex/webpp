// Created by moisrex on 10/7/21.

#ifndef WEBPP_STD_ITERATOR_HPP
#define WEBPP_STD_ITERATOR_HPP

#include "./concepts.hpp"

#include <iterator>

#if !__cpp_lib_concepts
namespace webpp::stl {

    // source: https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/include/bits/iterator_concepts.h

    namespace details {
        template <typename T>
        using with_ref = T&;

        template <typename T>
        concept can_reference = requires {
            typename with_ref<T>;
        };

        template <typename T>
        concept dereferenceable = requires(T& t) {
            { *t } -> can_reference;
        };
    } // namespace details

    template <details::dereferenceable T>
    using iter_reference_t = decltype(*std::declval<T&>());

    template <typename>
    struct incrementable_traits {};

    template <typename T>
    requires is_object_v<T>
    struct incrementable_traits<T*> {
        using difference_type = ptrdiff_t;
    };

    template <typename Iter>
    struct incrementable_traits<const Iter> : incrementable_traits<Iter> {};

    template <typename T>
    requires requires {
        typename T::difference_type;
    }
    struct incrementable_traits<T> {
        using difference_type = typename T::difference_type;
    };

    template <typename T>
    requires(
      !requires { typename T::difference_type; } &&
      requires(const T& a, const T& b) {
          { a - b } -> integral;
      }) struct incrementable_traits<T> {
        using difference_type = make_signed_t<decltype(std::declval<T>() - std::declval<T>())>;
    };


#    if defined __STRICT_ANSI__ && defined __SIZEOF_INT128__
    // __int128 is incrementable even if !integral<__int128>
    template <>
    struct incrementable_traits<__int128> {
        using difference_type = __int128;
    };

    template <>
    struct incrementable_traits<unsigned __int128> {
        using difference_type = __int128;
    };
#    endif



    namespace details {
        // An iterator such that iterator_traits<Iter> names a specialization
        // generated from the primary template.
        template <typename Iter>
        concept primary_traits_iter = is_base_of_v<iterator_traits<Iter>, iterator_traits<Iter>>;

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

        template <typename T>
        using iter_diff_t = typename iter_traits<T, incrementable_traits<T>>::difference_type;
    } // namespace details


    template <typename T>
    using iter_difference_t = details::iter_diff_t<remove_cvref_t<T>>;


    namespace details {
        class max_diff_type;
        class max_size_type;

        __extension__ template <typename T>
        concept is_signed_int128
#    if __SIZEOF_INT128__
          = same_as<T, __int128>;
#    else
          = false;
#    endif

        __extension__ template <typename T>
        concept is_unsigned_int128
#    if __SIZEOF_INT128__
          = same_as<T, unsigned __int128>;
#    else
          = false;
#    endif

        template <typename T>
        concept cv_bool = same_as<const volatile T, const volatile bool>;

        template <typename T>
        concept integral_nonbool = integral<T> && !cv_bool<T>;

        template <typename T>
        concept is_int128 = is_signed_int128<T> || is_unsigned_int128<T>;

        template <typename T>
        concept is_integer_like =
          integral_nonbool<T> || is_int128<T> || same_as<T, max_diff_type> || same_as<T, max_size_type>;

        template <typename T>
        concept is_signed_integer_like =
          signed_integral<T> || is_signed_int128<T> || same_as<T, max_diff_type>;

        template <class In>
        concept __IndirectlyReadableImpl = requires(const In in) {
            typename iter_value_t<In>;
            typename iter_reference_t<In>;
            typename iter_rvalue_reference_t<In>;
            { *in } -> same_as<iter_reference_t<In>>;
            { ranges::iter_move(in) } -> same_as<iter_rvalue_reference_t<In>>;
        }
        &&common_reference_with<iter_reference_t<In>&&, iter_value_t<In>&>&&
            common_reference_with<iter_reference_t<In>&&, iter_rvalue_reference_t<In>&&>&&
            common_reference_with<iter_rvalue_reference_t<In>&&, const iter_value_t<In>&>;


    } // namespace details

    template <class In>
    concept indirectly_readable = details::__IndirectlyReadableImpl<remove_cvref_t<In>>;


    /// Requirements on types that can be incremented with ++.
    template <typename Iter>
    concept weakly_incrementable = movable<Iter> && requires(Iter i) {
        typename iter_difference_t<Iter>;
        requires details::is_signed_integer_like<iter_difference_t<Iter>>;
        { ++i } -> same_as<Iter&>;
        i++;
    };

    template <typename Iter>
    concept incrementable = regular<Iter> && weakly_incrementable<Iter> && requires(Iter i) {
        { i++ } -> same_as<Iter>;
    };

    template <typename Iter>
    concept input_or_output_iterator = requires(Iter i) {
        { *i } -> details::can_reference;
    }
    &&weakly_incrementable<Iter>;

    template <typename Sent, typename Iter>
    concept sentinel_for = semiregular<Sent> && input_or_output_iterator<Iter> &&
      details::WeaklyEqualityComparableWith<Sent, Iter>;

    template <typename Sent, typename Iter>
    inline constexpr bool disable_sized_sentinel_for = false;

    template <typename Sent, typename Iter>
    concept sized_sentinel_for =
      sentinel_for<Sent, Iter> && !disable_sized_sentinel_for<remove_cv_t<Sent>, remove_cv_t<Iter>> &&
      requires(const Iter& i, const Sent& s) {
        { s - i } -> same_as<iter_difference_t<Iter>>;
        { i - s } -> same_as<iter_difference_t<Iter>>;
    };

    template <typename Iter>
    concept input_iterator = input_or_output_iterator<Iter> && indirectly_readable<Iter> && requires {
        typename details::iter_concept<Iter>;
    } && derived_from<details::iter_concept<Iter>, input_iterator_tag>;

    template <typename Iter, typename T>
    concept output_iterator = input_or_output_iterator<Iter> && indirectly_writable<Iter, T> &&
      requires(Iter i, T&& t) {
        *i++ = std::forward<T>(t);
    };

    template <typename Iter>
    concept forward_iterator =
      input_iterator<Iter> && derived_from<details::__iter_concept<Iter>, forward_iterator_tag> &&
      incrementable<Iter> && sentinel_for<Iter, Iter>;

    template <typename Iter>
    concept bidirectional_iterator = forward_iterator<Iter> &&
      derived_from<details::iter_concept<Iter>, bidirectional_iterator_tag> && requires(Iter i) {
        { --i } -> same_as<Iter&>;
        { i-- } -> same_as<Iter>;
    };


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

#endif // cpp_lib_concepts

#endif // WEBPP_STD_ITERATOR_HPP
