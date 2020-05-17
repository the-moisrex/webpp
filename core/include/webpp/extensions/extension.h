// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

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


    template <typename... Extensions>
    struct extension_pack : public ::std::decay_t<Extensions>... {};


    /**
     * Unpack std::tuple into extension pack
     */
    template <typename... Extensions>
    struct extension_pack<::std::tuple<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Unpack typelist into extension pack
     */
    template <typename... Extensions>
    struct extension_pack<typelist<Extensions...>>
      : public extension_pack<Extensions...> {};

    /**
     * Unpack the extension pack
     */
    template <typename... Extensions>
    struct extension_pack<extension_pack<Extensions...>>
      : public extension_pack<Extensions...> {};


} // namespace webpp

#endif // WEBPP_EXTENSION_H
