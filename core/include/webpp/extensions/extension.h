// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

#include "extension_concepts.h"

#include <tuple>
#include <type_traits>

namespace webpp {


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

} // namespace webpp

#endif // WEBPP_EXTENSION_H
