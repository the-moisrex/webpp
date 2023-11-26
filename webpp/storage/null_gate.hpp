#ifndef WEBPP_STORAGE_NULL_GATE_HPP
#define WEBPP_STORAGE_NULL_GATE_HPP

#include "../std/optional.hpp"
#include "../traits/traits.hpp"

namespace webpp {

    struct null_gate {
        template <Traits TraitsType, typename KeyT, typename ValueT, typename OptsT>
        struct storage_gate {
            using key_type     = KeyT;
            using value_type   = ValueT;
            using options_type = OptsT;
            using traits_type  = TraitsType;

            // we should be using cache_tuple, but we don't want to create a circular dependency by importing
            // cache_concepts.hpp file here.
            struct bundle_type {
                key_type     key;
                value_type   value;
                options_type options;
            };

            constexpr storage_gate(auto&&...) {}

            template <typename V>
            constexpr stl::optional<bundle_type> get([[maybe_unused]] V&& value) {
                return stl::nullopt;
            }

            template <typename K, typename V>
            constexpr void set([[maybe_unused]] K&&          key,
                               [[maybe_unused]] V&&          value,
                               [[maybe_unused]] options_type opts = {}) {}

            constexpr void set_options([[maybe_unused]] key_type const& key,
                                       [[maybe_unused]] options_type    opts) {}

            template <typename K>
            constexpr void erase([[maybe_unused]] K&& input) {}

            template <typename Pred>
            constexpr void erase_if([[maybe_unused]] Pred&& predicate) {}
        };
    };

} // namespace webpp

#endif
