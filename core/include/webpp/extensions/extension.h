// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

#include "../std/tuple.h"

#include <type_traits>

namespace webpp {

    template <typename T>
    concept Extension =
      !::std::is_final_v<T> && ::std::is_default_constructible_v<T> &&
      ::std::is_move_constructible_v<T> && ::std::is_move_assignable_v<T> &&
      ::std::is_copy_assignable_v<T> && ::std::is_class_v<T> &&
      !::stl::is_tuple<T>::value && !::std::is_integral_v<T>;


    template <typename... T>
    struct is_extension_list {
        static constexpr bool value = false;
    };

    template <Extension T>
    struct is_extension_list<T> {
        static constexpr bool value = true;
    };

    template <Extension... T>
    struct is_extension_list<::std::tuple<T...>> {
        static constexpr bool value = true;
    };

    template <typename T, template <typename...>
                          typename IsExtensionList = is_extension_list>
    concept ExtensionList = IsExtensionList<T>::value ||
                            (::stl::Tuple<T> && IsExtensionList<T>::value);


    template <typename T>
    concept ExtensionWithDependencies = Extension<T>&& requires {
        typename T::required_extensions;
    }
    &&Extension<typename T::required_extensions>;


    template <typename... T>
    struct typelist {
      private:
        template <typename... F>
        struct prepend {
            using type = typelist<F...>;
        };

        template <typename F, typename... L>
        struct prepend<F, typelist<L...>> {
            using type = typelist<F, L...>;
        };

        template <typename First = void, typename... U>
        struct unique_types {
            using type = ::std::conditional_t<
              ((!std::is_same_v<First, U>)&&...),
              typename prepend<First, typename typelist<U...>::unique>::type,
              typename typelist<U...>::unique>;
        };

        template <typename... U>
        struct unique_types<void, U...> {
            using type = typelist<U...>;
        };

      public:
        using unique = typename unique_types<T...>::type;
    };


    template <Extension... Extensions>
    struct extension_pack : public ::std::decay_t<Extensions>... {};


    /**
     * Unpack std::tuple into extension pack
     */
    template <Extension... Extensions>
    struct extension_pack<::std::tuple<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Unpack typelist into extension pack
     */
    template <Extension... Extensions>
    struct extension_pack<typelist<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Unpack the extension pack
     */
    template <Extension... Extensions>
    struct extension_pack<extension_pack<Extensions...>>
      : public extension_pack<Extensions...> {};

    template <typename... E>
    struct flattened_extension_pack;

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<extension_pack<EP1...>, typelist<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<extension_pack<EP1...>,
                                    ::std::tuple<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<::std::tuple<EP1...>, typelist<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<::std::tuple<EP1...>,
                                    extension_pack<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<typelist<EP1...>, extension_pack<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };

    template <Extension... EP1, Extension... EP2>
    struct flattened_extension_pack<typelist<EP1...>, ::std::tuple<EP2...>> {
        using type = extension_pack<EP1..., EP2...>;
    };


    ///////////////////// Extending the is_extension_list ////////////////////

    template <Extension... T>
    struct is_extension_list<extension_pack<T...>> {
        static constexpr bool value = true;
    };

    template <Extension... T>
    struct is_extension_list<typelist<T...>> {
        static constexpr bool value = true;
    };


    //////////////////////////////////////////////////////////////////////////

    using empty_extension_pack = extension_pack<>;

} // namespace webpp

#endif // WEBPP_EXTENSION_H
