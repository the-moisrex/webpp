#ifndef WEBPP_STORAGE_NULL_GATE_HPP
#define WEBPP_STORAGE_NULL_GATE_HPP

#include "../std/optional.hpp"
#include "../traits/default_traits.hpp"

namespace webpp {

    struct null_gate {

        template <Traits TraitsType, typename KeyT, typename ValueT, typename>
        struct storage_gate {
            using key_type    = KeyT;
            using value_type  = ValueT;
            using traits_type = TraitsType;

            template <typename V>
            stl::optional<value_type> get([[maybe_unused]] V&& value) {
                return stl::nullopt;
            }

            template <typename K, typename V>
            void set([[maybe_unused]] K&& key, [[maybe_unused]] V&& value) {}

            template <typename K>
            void set([[maybe_unused]] K&& key) {}

            template <typename K>
            void erase([[maybe_unused]] K&& input) {}
        };
    };

} // namespace webpp

#endif
