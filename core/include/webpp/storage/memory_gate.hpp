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


            void set(key_type const& key, value_type const& value) {}

            map_type map;
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_MEMORY_GATE_HPP
