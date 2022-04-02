#ifndef WEBPP_STORAGE_CACHE_CONCEPTS_HPP
#define WEBPP_STORAGE_CACHE_CONCEPTS_HPP

#include "../std/type_traits.hpp"
#include "../traits/default_traits.hpp"
#include "null_gate.hpp"

namespace webpp {


    template <typename K>
    concept CacheKey = !stl::is_void_v<K>;

    template <typename K>
    concept CacheValue = !stl::is_void_v<K>;

    namespace details {
        template <typename S>
        concept StorageGateType = requires(S g) {
            typename S::traits_type;
            requires Traits<typename S::traits_type>;
        };


        template <typename T>
        concept CacheStrategy = requires {
            typename T::key_type;
            typename T::value_type;
            requires requires(T st, typename T::key_type key, typename T::value_type value) {
                st.set(key, value);
                st.set(key);
            };
        };

    } // namespace details

    template <typename T>
    concept StorageGate = requires {
        typename T::template storage_gate<default_traits, int, int>;
        requires details::StorageGateType<typename T::template storage_gate<default_traits, int, int>>;
    };


    template <typename T>
    concept CacheStrategy = requires {
        typename T::strategy<default_traits, int, int, null_gate>;
        requires details::CacheStrategy<typename T::template strategy<default_traits, int, int, null_gate>>;
    };
} // namespace webpp

#endif // WEBPP_STORAGE_CACHE_CONCEPTS_HPP
