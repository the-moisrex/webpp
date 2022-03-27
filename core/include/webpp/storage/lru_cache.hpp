#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "memory_gate.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    template <typename KeyT, typename ValueT, StorageGate SG = memory_gate>
    struct lru_strategy {
        using key_type   = KeyT;
        using value_type = ValueT;
        struct entry_type {
            value_type  value;
            stl::size_t last_used_index = 0;
        };
        using storage_gate_type = typename SG::storage_gate<key_type, entry_type>;
        using traits_type       = typename storage_gate_type::traits_type;


      private:
        stl::size_t       max_size   = 100;
        stl::size_t       next_usage = 1;
        storage_gate_type gate;

      public:
        template <typename K, typename V>
        requires(stl::convertible_to<K, key_type>&&    // it's a key
                   stl::convertible_to<V, value_type>) // it's a value
          void set(K&& key, V&& value) {
            gate.set(stl::forward<K>(key),
                     entry_type{.value = stl::forward<V>(value), .last_used_index = next_usage++});
        }


        template <typename K>
        requires(stl::convertible_to<K, key_type>) // it's a key
          stl::optional<value_type> get(K&& key) {
            return gate.get(stl::forward<K>(key));
        }
    };



} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
