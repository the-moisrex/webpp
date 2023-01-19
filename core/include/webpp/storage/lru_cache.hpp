#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "cache.hpp"
#include "directory_gate.hpp"
#include "memory_gate.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    struct lru_strategy {

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT, StorageGate SG>
        struct strategy {
            using traits_type = TraitsType;
            using key_type    = traits::generalify_allocators<traits_type, KeyT>;
            using value_type  = traits::generalify_allocators<traits_type, ValueT>;
            using storage_gate_type =
              typename SG::template storage_gate<traits_type, key_type, value_type, stl::size_t>;
            using data_type = typename storage_gate_type::data_type;



          private:
            stl::size_t max_size;
            stl::size_t next_usage = 1; // it's essentially a timestamp
          public:
            storage_gate_type gate;

          private:
            // clean up the old data
            void clean_up() {
                if (next_usage <= max_size)
                    return;
                stl::size_t break_index = next_usage - max_size;
                gate.erase_if([break_index](auto const& item) noexcept {
                    const auto last_used_index = item.options;
                    return last_used_index < break_index;
                });
            }

          public:
            template <typename ET, typename... Args>
                requires(EnabledTraits<ET> && !stl::same_as<ET, strategy const&> &&
                         !stl::same_as<ET, strategy &&>)
            constexpr strategy(ET&& et, stl::size_t max_size_value = 1024, Args&&... args) noexcept
              : max_size{max_size_value},
                gate{et, stl::forward<Args>(args)...} {}

            constexpr strategy(storage_gate_type&& input_gate, stl::size_t max_size_value = 1024) noexcept
              : max_size{max_size_value},
                gate{input_gate} {}

            template <typename K, typename V>
                requires(stl::convertible_to<K, key_type> && // it's a key
                         stl::convertible_to<V, value_type>) // it's a value
            constexpr void set(K&& key, V&& value) {
                gate.set(stl::forward<K>(key), stl::forward<V>(value), next_usage++);
                clean_up();
            }


            template <typename K>
                requires(stl::convertible_to<K, key_type>) // it's a key
            constexpr stl::optional<value_type> get(K&& key) {
                stl::optional<data_type> data = gate.get(key);
                if (!data)
                    return stl::nullopt;

                gate.set_options(key, next_usage++);

                return data->value;
            }
        };
    };




    template <Traits      TraitsType   = default_traits,
              CacheKey    KeyT         = traits::general_string<TraitsType>,
              CacheValue  ValT         = traits::general_string<TraitsType>,
              StorageGate StorageGateT = memory_gate<directory_gate>>
    using lru_cache = cache<TraitsType, KeyT, ValT, lru_strategy, StorageGateT>;

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
