#ifndef WEBPP_STORAGE_NULL_GATE_HPP
#define WEBPP_STORAGE_NULL_GATE_HPP

#include "../std/optional.hpp"
#include "../traits/default_traits.hpp"
#include "cache_concepts.hpp"

namespace webpp {

    struct null_gate {

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT>
        struct storage_gate {
            using key_type    = KeyT;
            using value_type  = ValueT;
            using traits_type = TraitsType;

            template <CacheValue V>
            stl::optional<value_type> get([[maybe_unused]] V&& value) {
                return stl::nullopt;
            }

            template <CacheKey K, CacheValue V>
            void set([maybe_unused]] K&& key, [[maybe_unused]] V&& value) {
            }


            template <typename K>
            void erase([[maybe_unused]] K&& input) {}
        };
    };

} // namespace webpp

#endif
