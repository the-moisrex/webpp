#ifndef WEBPP_STORAGE_MEMORY_GATE_HPP
#define WEBPP_STORAGE_MEMORY_GATE_HPP

#include "../std/map.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "null_gate.hpp"

namespace webpp {

    // we're using typename instead of StorageGate to fix an IDE error while the compiler is happy
    template </* StorageGate */ typename ParentGate = null_gate>
    struct memory_gate {
        using parent_gate_type = ParentGate;

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT, CacheOptions OptsT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type     = TraitsType;
            using value_pack_type = stl::pair<OptsT, ValueT>;
            using map_type        = traits::general_object<traits_type, stl::map<KeyT, value_pack_type>>;
            using mapped_type     = typename map_type::mapped_type;
            using key_type        = typename map_type::key_type;
            using value_type      = typename map_type::mapped_type::second_type;
            using options_type    = typename mapped_type::first_type;
            using etraits         = enable_traits<TraitsType>;
            using data_type       = cache_tuple<key_type, value_type, options_type>;


            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate&&>)
            constexpr storage_gate(ET&& et) // NOLINT(cppcoreguidelines-pro-type-member-init)
              : etraits{et},
                map(et.alloc_pack, et.alloc_pack.general_resource()) {}

            template <typename K>
            constexpr stl::optional<data_type> get(K&& key) {
                if (auto it = map.find(stl::forward<K>(key)); it != map.end()) {
                    return data_type{.key     = it->first,
                                     .value   = it->second.second,
                                     .options = it->second.first};
                }
                return stl::nullopt;
            }

            template <typename K, typename V>
            constexpr void set(K&& key, V&& value, options_type opts = {}) {
                map.insert_or_assign(stl::forward<K>(key),
                                     mapped_type{stl::move(opts), stl::forward<V>(value)});
            }

            constexpr void set_options(key_type const& key, options_type const& opts) {
                if (auto it = map.find(key); it != map.end()) {
                    it->second.first = opts;
                }
            }


            template <typename K>
            constexpr void erase(K&& input) {
                map.erase(stl::forward<K>(input));
            }

            template <typename Pred>
            constexpr void erase_if(Pred&& predicate) {
                stl::erase_if(map, [predicate](auto&& item) {
                    auto [key, value_pack] = item;
                    auto [opts, value]     = value_pack;
                    return predicate(data_type{key, value, opts});
                });
            }

            constexpr auto begin() const {
                return map.begin();
            }

            constexpr auto end() const {
                return map.begin();
            }

            constexpr auto begin() {
                return map.begin();
            }

            constexpr auto end() {
                return map.begin();
            }



            map_type map;
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
