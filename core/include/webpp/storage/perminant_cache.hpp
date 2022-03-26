#ifndef WEBPP_STORAGE_PERMINANT_CACHE_HPP
#define WEBPP_STORAGE_PERMINANT_CACHE_HPP

#include "memory_gate.hpp"

namespace webpp {

    /**
     * Perminant Cache (essentially a std::map)
     */
    template <typename KeyT, typename ValueT, StorageGate SG = memory_gate>
    struct perminant_cache {
        using key_type          = KeyT;
        using value_type        = ValueT;
        using storage_gate_type = typename SG::storage_gate<key_type, value_type>;
        using traits_type       = typename storage_gate_type::traits_type;


      private:
        storage_gate_type gate;

      public:
        template <typename K, typename V>
        perminant_cache& set(K&& key, V&& value) {
            gate.set(stl::forward<K>(key), stl::forward<V>(value));
            return *this;
        }


        template <typename K>
        stl::optional<value_type> get(K&& key) {
            return gate.get(stl::forward<K>(key));
        }
    };

} // namespace webpp

#endif // WEBPP_STORAGE_PERMINANT_CACHE_HPP
