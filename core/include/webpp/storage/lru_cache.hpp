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

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT, StorageGate SG>
        struct strategy {
            using key_type   = KeyT;
            using value_type = ValueT;
            struct entry_type {
                value_type  value;
                stl::size_t last_used_index = 0;
            };
            using traits_type       = TraitsType;
            using storage_gate_type = typename SG::template storage_gate<traits_type, key_type, entry_type>;


          private:
            stl::size_t       max_size;
            stl::size_t       next_usage = 1; // it's essentially a timestamp
            storage_gate_type gate;

          public:
            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, strategy const&> &&
                         !stl::same_as<ET, strategy &&>)
            constexpr strategy(ET&& et, stl::size_t max_size_value = 1024) noexcept
              : max_size{max_size_value},
                gate{et} {}

            constexpr strategy(storage_gate_type&& input_gate, stl::size_t max_size_value = 1024) noexcept
              : max_size{max_size_value},
                gate{input_gate} {}

            void clean_up() {
                stl::size_t break_index = next_usage - max_size;
                for (auto it = gate.begin(); it != gate.end(); ++it) {
                    if (it->second.last_used_index < break_index) {
                        gate.erase(it);
                    }
                }
            }

            template <typename K, typename V>
                requires(stl::convertible_to<K, key_type> && // it's a key
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

                val->last_used_index = next_usage++;

                return val->value;
            }
        };
    };




    template <Traits      TraitsType   = default_traits,
              CacheKey    KeyT         = traits::general_string<TraitsType>,
              CacheValue  ValT         = traits::general_string<TraitsType>,
              StorageGate StorageGateT = memory_gate<file_gate>>
    using lru_cache = cache<TraitsType, KeyT, ValT, lru_strategy, StorageGateT>;

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
