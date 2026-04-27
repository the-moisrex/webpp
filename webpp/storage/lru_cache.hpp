#ifndef WEBPP_STORAGE_LRU_CACHE_HPP
#define WEBPP_STORAGE_LRU_CACHE_HPP

#include "cache.hpp"
#include "directory_gate.hpp"
#include "memory_gate.hpp"

namespace webpp {

    /**
     * LRU Cache (Least Recently Used Cache)
     */
    struct [[nodiscard]] lru_strategy {
        template <CacheKey KeyT, CacheValue ValueT, StorageGate SG, istl::CharType CharT, Allocator AllocT>
        struct strategy {
            using key_type   = KeyT;
            using value_type = ValueT;
            using storage_gate_type =
              typename SG::template storage_gate<key_type, value_type, stl::size_t, CharT, AllocT>;
            using bundle_type = typename storage_gate_type::bundle_type;


            static constexpr stl::size_t default_max_size = 1024U;

          private:
            stl::size_t       max_size;
            stl::size_t       next_usage = 1; // it's essentially a timestamp
            storage_gate_type gate;

          protected:
            constexpr storage_gate_type& get_gate() noexcept {
                return gate;
            }

          private:
            // clean up the old data
            void clean_up() {
                if (next_usage <= max_size) {
                    return;
                }
                stl::size_t break_index = next_usage - max_size;
                gate.erase_if([break_index](auto const& item) noexcept {
                    auto const last_used_index = item.options;
                    return last_used_index < break_index;
                });
            }

          public:
            template <typename... Args>
            explicit constexpr strategy(stl::size_t const max_size_value = default_max_size, Args&&... args) noexcept
              : max_size{max_size_value},
                gate{stl::forward<Args>(args)...} {}

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
                stl::optional<bundle_type> const data = gate.get(key);
                if (!data) {
                    return stl::nullopt;
                }

                gate.set_options(key, next_usage++);

                return data->value;
            }

            template <typename K>
                requires(details::StorageGatePointerSupport<storage_gate_type> && stl::convertible_to<K, key_type>)
            constexpr auto* get_ptr(K&& key) {
                // we can't use return type directly in the signature because we're using "value_ptr_type"
                // which may not be present in every storage gate type.
                using return_type = typename storage_gate_type::value_ptr_type;
                auto const data   = gate.get_ptr(key); // data is optional<bundle_ref_type>
                if (!data) {
                    return return_type{nullptr};
                }

                // todo: we have a reference to the options here, don't need to re-write it.
                gate.set_options(key, next_usage++);

                // return optional<value_ptr_type>
                return return_type{data->value};
            }
        };
    };

    template <CacheKey    KeyT         = stl::basic_string<char, stl::char_traits<char>, default_allocator_t<char>>,
              CacheValue  ValT         = stl::basic_string<char, stl::char_traits<char>, default_allocator_t<char>>,
              StorageGate StorageGateT = memory_gate<directory_gate>>
    using lru_cache = cache<KeyT, ValT, lru_strategy, StorageGateT>;

} // namespace webpp

#endif // WEBPP_STORAGE_LRU_CACHE_HPP
