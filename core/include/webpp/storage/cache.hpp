#ifndef WEBPP_CACHE_CACHE_H
#define WEBPP_CACHE_CACHE_H

#include "cache_concepts.hpp"


namespace webpp {


    template <typename KeyT, typename ValT, CacheStrategy CS>
    struct cache : CS {
        using key_type          = KeyT;
        using value_type        = ValT;
        using strategy_type     = CS;
        using storage_gate_type = typename strategy_type::storage_gate_type;
        using traits_type       = typename strategy_type::traits_type;

        struct cache_reault {};

        using CS::CS;


        auto operator[](key_type key) {}


        template <CacheKey K, CacheValue V>
        requires(stl::is_convertible_v<K, key_type>&& stl::is_convertible_v<V, value_type>) value_type
          get(K&& key, V&& default_value) {
            return storage_gate_type::get(stl::forward<K>(key)).value_or(stl::forward<V>(default_value));
        }
    };

    template <typename CacheSystem>
    auto set(CacheSystem&                       cache_system,
             typename CacheSystem::key_type&&   key,
             typename CacheSystem::value_type&& value) {
        return cache_system.set(std::forward<typename CacheSystem::key_type>(key),
                                std::forward<typename CacheSystem::value_type>(value));
    }

} // namespace webpp

#endif
