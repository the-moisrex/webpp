#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "cache_concepts.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    template <typename KeyT, typename ValueT, StorageGate SG = memory_storage_gate>
    struct basic_lru_cache {
        using key_type          = KeyT;
        using value_type        = ValueT;
        using storage_gate_type = typename SG::storage_gate<key_type, value_type>;
        using traits_type       = typename storage_gate_type::traits_type;


      private:
        storage_gate_type gate;

      public:
        template <typename K, typename V>
        basic_lru_cache& set(K&& key, V&& value) {
            gate.emplace(stl::forward<K>(key), stl::forward<V>(value));
        }


        template <typename K>
        stl::optional<value_type> get(K&& key) {
            if (auto res = gate.find(stl::forward<K>(key)); res != gate.end())
                return res;
            return stl::nullopt;
        }
    };

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
