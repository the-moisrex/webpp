#ifndef WEBPP_STORAGE_MEMORY_GATE_HPP
#define WEBPP_STORAGE_MEMORY_GATE_HPP

#include "../traits/default_traits.hpp"

namespace webpp {

    struct memory_gate {
        template <typename KeyT, typename ValueT, Traits TraitsType = default_traits>
        struct storage_gate {
            using key_type           = KeyT;
            using value_type         = ValueT;
            using traits_type        = TraitsType;
            using map_pair_type      = stl::pair<const key_type, value_type>;
            using map_allocator_type = traits::general_allocator<traits_type, map_pair_type>;
            using map_type = stl::map<key_type, value_type, stl::less<key_type>, map_allocator_type>;

            template <typename V>
            stl::optional<value_type> get(V&& value) {
                if (auto it = map.find(stl::forward<V>(value)); it != map.end()) {
                    return *it;
                }
                return stl::nullopt;
            }

            template <typename K, typename V>
            void set(K&& key, V&& value) {
                map.emplace(stl::forward<K>(key), stl::forward<V>(value));
            }


            template <typename K>
            void erase(K&& input) {
                map.erase(stl::forward<K>(input));
            }

            map_type map;
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
