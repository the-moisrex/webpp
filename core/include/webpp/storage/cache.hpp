#ifndef WEBPP_CACHE_CACHE_HPP
#define WEBPP_CACHE_CACHE_HPP

#include "cache_concepts.hpp"


namespace webpp {


    /**
     * This class is mother of all caches.
     *
     * Cache Strategy: The type of cache; e.g. LRU, Expired LRU, ...
     * Storage Gate:   How the cache data is stored (and where); e.g. Memory, File, ...
     */
    template <Traits TraitsType, CacheKey KeyT, CacheValue ValT, CacheStrategy CS, StorageGate SG>
    struct cache : public CS::template strategy<TraitsType, KeyT, ValT, SG> {
        using key_type          = KeyT;
        using value_type        = ValT;
        using traits_type       = TraitsType;
        using storage_gate_type = typename SG::template storage_gate<traits_type, key_type, value_type>;
        using strategy_type     = typename CS::template strategy<traits_type, key_type, value_type, SG>;

        struct cache_result {};

        // ctor
        using CS::template strategy<traits_type, key_type, value_type, SG>::strategy;


        auto operator[](key_type key) {}


        template <CacheKey K, CacheValue V>
            requires(stl::is_convertible_v<K, key_type>&& stl::is_convertible_v<V, value_type>)
        value_type get(K&& key, V&& default_value) {
            return strategy_type::get(stl::forward<K>(key)).value_or(stl::forward<V>(default_value));
        }


        template <CacheKey K>
            requires(stl::is_convertible_v<K, key_type>) // it's convertible to key
        stl::optional<value_type> get(K&& key) {
            return strategy_type::get(stl::forward<K>(key));
        }
    };


} // namespace webpp

#endif // WEBPP_CACHE_CACHE_HPP
