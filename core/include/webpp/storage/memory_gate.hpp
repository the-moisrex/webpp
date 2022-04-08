#ifndef WEBPP_STORAGE_MEMORY_GATE_HPP
#define WEBPP_STORAGE_MEMORY_GATE_HPP

#include "../std/map.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "null_gate.hpp"

namespace webpp {

    template <StorageGate ParentGate = null_gate>
    struct memory_gate {

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type = TraitsType;
            using map_type    = traits::general_object<traits_type, stl::map<KeyT, ValueT>>;
            using key_type    = typename map_type::key_type;
            using value_type  = typename map_type::mapped_type;
            using etraits     = enable_traits<TraitsType>;

            template <typename ET>
            requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                     !stl::same_as<ET, storage_gate&&>) storage_gate(ET&& et)
              : etraits{et},
                map(et) {}

            template <typename K>
            stl::optional<value_type> get(K&& key) {
                if (auto it = map.find(stl::forward<K>(key)); it != map.end()) {
                    return it->second;
                }
                return stl::nullopt;
            }

            template <typename K, typename V>
            void set(K&& key, V&& value) {
                map.insert_or_assign(stl::forward<K>(key), stl::forward<V>(value));
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
