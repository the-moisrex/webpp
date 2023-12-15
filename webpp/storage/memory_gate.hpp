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
        struct storage_gate {
            using traits_type      = TraitsType;
            using value_pack_type  = stl::pair<OptsT, ValueT>;
            using map_type         = istl::map<traits_type, KeyT, value_pack_type>;
            using mapped_type      = typename map_type::mapped_type;
            using key_type         = typename map_type::key_type;
            using value_type       = typename map_type::mapped_type::second_type;
            using options_type     = typename mapped_type::first_type;
            using bundle_type      = cache_tuple<key_type, value_type, options_type>;
            using key_ptr_type     = stl::add_pointer_t<stl::add_const_t<key_type>>;
            using value_ptr_type   = stl::add_pointer_t<value_type>;
            using options_ptr_type = stl::add_pointer_t<options_type>;
            using bundle_ptr_type  = cache_tuple<key_ptr_type, value_ptr_type, options_ptr_type>;

            // NOLINTBEGIN(bugprone-forwarding-reference-overload)
            template <EnabledTraits ET>
                requires(!stl::same_as<stl::remove_cvref_t<ET>, storage_gate>)
            explicit constexpr storage_gate(ET&& et) : map{get_alloc_for<map_type>(et)} {}

            // NOLINTEND(bugprone-forwarding-reference-overload)

            template <typename K>
            constexpr stl::optional<bundle_type> get(K&& key) {
                if (auto it = map.find(stl::forward<K>(key)); it != map.end()) {
                    return bundle_type{.key     = stl::move(it->first),
                                       .value   = stl::move(it->second.second),
                                       .options = stl::move(it->second.first)};
                }
                return stl::nullopt;
            }

            // Get a reference instead of copying them
            template <typename K>
            constexpr stl::optional<bundle_ptr_type> get_ptr(K&& key) {
                if (auto it = map.find(key); it != map.end()) {
                    return bundle_ptr_type{.key     = &it->first,
                                           .value   = &it->second.second,
                                           .options = &it->second.first};
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
                    return predicate(bundle_type{key, value, opts});
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


          private:
            map_type map;
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
