#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "cache.hpp"
#include "file_gate.hpp"
#include "memory_gate.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    struct lru_strategy {

        template <typename KeyT, typename ValueT, StorageGate SG>
        struct strategy {
            using key_type   = KeyT;
            using value_type = ValueT;
            struct entry_type {
                value_type  value;
                stl::size_t last_used_index = 0;
            };
            using storage_gate_type = typename SG::template storage_gate<key_type, entry_type>;
            using traits_type       = typename storage_gate_type::traits_type;


          private:
            stl::size_t       max_size;
            stl::size_t       next_usage = 1; // it's essentially a timestamp
            storage_gate_type gate;

          public:
            constexpr strategy(stl::size_t max_size_value = 1024) noexcept : max_size{max_size_value} {}

            void clean_up() {
                stl::size_t break_index = next_usage - max_size;
                for (auto it = gate.begin(); it != gate.end(); ++it) {
                    if (it->last_used_index < break_index) {
                        gate.erase(it);
                    }
                }
            }

            template <typename K, typename V>
            requires(stl::convertible_to<K, key_type>&&    // it's a key
                       stl::convertible_to<V, value_type>) // it's a value
              void set(K&& key, V&& value) {
                gate.set(stl::forward<K>(key),
                         entry_type{.value = stl::forward<V>(value), .last_used_index = next_usage++});
                if (next_usage >= max_size) {
                    clean_up();
                }
            }


            template <typename K>
            requires(stl::convertible_to<K, key_type>) // it's a key
              stl::optional<value_type> get(K&& key) {
                auto val = gate.get(key);
                if (!val)
                    return stl::nullopt;

                auto nval            = *val;
                nval.last_used_index = next_usage++;
                gate.set(key, stl::move(nval));

                return val->value;
            }
        };
    };



    template <CacheKey    KeyT         = stl::string,
              CacheValue  ValT         = stl::string,
              StorageGate StorageGateT = memory_gate<file_gate>>
    using lru_cache = cache<KeyT, ValT, lru_strategy, StorageGateT>;

    template <Traits      TraitsType   = default_traits,
              CacheKey    KeyT         = traits::general_string<TraitsType>,
              CacheValue  ValT         = traits::general_string<TraitsType>,
              StorageGate StorageGateT = memory_gate<file_gate>>
    using string_lru_cache = cache<KeyT, ValT, lru_strategy, StorageGateT>;

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
