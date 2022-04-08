#ifndef WEBPP_STORAGE_MEMORY_GATE_HPP
#define WEBPP_STORAGE_MEMORY_GATE_HPP

#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "null_gate.hpp"

namespace webpp {

    template <StorageGate ParentGate = null_gate>
    struct memory_gate {

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT>
        struct storage_gate : enable_traits<TraitsType> {
            using key_type           = KeyT;
            using value_type         = ValueT;
            using traits_type        = TraitsType;
            using map_pair_type      = stl::pair<const key_type, value_type>;
            using map_allocator_type = traits::general_allocator<traits_type, map_pair_type>;
            using map_type = stl::map<key_type, value_type, stl::less<key_type>, map_allocator_type>;
            using etraits  = enable_traits<TraitsType>;

            template <typename ET>
            requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> && !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&& et) : etraits{et},
                                    map{et.alloc_pack.template general_allocator<map_pair_type>()} {}

            template <typename V>
            stl::optional<value_type> get(V&& value) {
                if (auto it = map.find(stl::forward<V>(value)); it != map.end()) {
                    return it->second;
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

            auto begin() const {
                return map.begin();
            }

            auto end() const {
                return map.begin();
            }


            map_type map;
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
