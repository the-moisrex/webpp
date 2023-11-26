#ifndef WEBPP_STORAGE_PERMANENT_CACHE_HPP
#define WEBPP_STORAGE_PERMANENT_CACHE_HPP

#include "cache_concepts.hpp"
#include "memory_gate.hpp"
#include "null_gate.hpp"

namespace webpp {

    /**
     * Non-Evicting Cache: A cache that doesn't drop the caches unless manually told.
     */
    template <typename KeyT, typename ValueT, StorageGate SG = memory_gate<null_gate>>
    struct non_evicting_cache {
        using key_type          = KeyT;
        using value_type        = ValueT;
        using storage_gate_type = typename SG::template storage_gate<key_type, value_type>;
        using traits_type       = typename storage_gate_type::traits_type;


      private:
        storage_gate_type gate;

      public:
        template <typename K, typename V>
        constexpr non_evicting_cache& set(K&& key, V&& value) {
            gate.set(stl::forward<K>(key), stl::forward<V>(value));
            return *this;
        }

        template <typename K>
        constexpr stl::optional<value_type> get(K&& key) {
            return gate.get(stl::forward<K>(key));
        }
    };

} // namespace webpp

#endif // WEBPP_STORAGE_PERMANENT_CACHE_HPP
