#ifndef WEBPP_STORAGE_CACHE_CONCEPTS_HPP
#define WEBPP_STORAGE_CACHE_CONCEPTS_HPP

#include "../std/type_traits.hpp"
#include "../traits/default_traits.hpp"

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

    } // namespace details

    template <typename T>
    concept StorageGate = requires {
        typename T::template storage_gate<int, int, default_traits>;
        requires details::StorageGateType<typename T::template storage_gate<int, int, default_traits>>;
    };

} // namespace webpp

#endif // WEBPP_STORAGE_CACHE_CONCEPTS_HPP
