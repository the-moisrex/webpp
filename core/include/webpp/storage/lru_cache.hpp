#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "memory_gate.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    template <typename KeyT, typename ValueT, StorageGate SG = memory_gate>
    struct lru_strategy {
        using key_type          = KeyT;
        using value_type        = ValueT;
        using storage_gate_type = typename SG::storage_gate<key_type, value_type>;
        using traits_type       = typename storage_gate_type::traits_type;


      private:
        storage_gate_type gate;

      public:
        template <typename K, typename V>
        requires(stl::convertible_to<K, key_type>&&    // it's a key
                   stl::convertible_to<V, value_type>) // it's a value
          void set(K&& key, V&& value) {
            gate.set(stl::forward<K>(key), stl::forward<V>(value));
        }


        template <typename K>
        requires(stl::convertible_to<K, key_type>) // it's a key
          stl::optional<value_type> get(K&& key) {
            return gate.get(stl::forward<K>(key));
        }
    };



} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
