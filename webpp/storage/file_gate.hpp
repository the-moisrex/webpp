#ifndef WEBPP_STORAGE_FILE_GATE_HPP
#define WEBPP_STORAGE_FILE_GATE_HPP

#include "../traits/default_traits.hpp"
#include "cache_concepts.hpp"

#include <filesystem>

namespace webpp {

    /**
     * File Gate stores the cached data in a file
     *
     * The file gate doesn't support a parent because why should it?
     */
    struct file_gate {
        template <Traits TraitsType, CacheFileKey KeyT, CacheFileValue ValueT, CacheFileOptions OptsT>
        struct storage_gate {
            using key_type     = KeyT;
            using value_type   = ValueT;
            using options_type = OptsT;
            using traits_type  = TraitsType;
            using path_type    = stl::filesystem::path;
            using bundle_type  = cache_tuple<key_type, value_type, options_type>;

          private:
            path_type cache_file;

          public:
            explicit storage_gate(path_type output_file) : cache_file{stl::move(output_file)} {}

            template <typename V>
            stl::optional<bundle_type> get(V&& value) {
                return stl::nullopt;
            }

            template <typename K, typename V>
            void set(K&& key, V&& value, options_type opts = {}) {
                // todo
            }

            void set_options(key_type const& key, options_type opts) {
                // todo
            }

            template <typename K>
            void erase(K&& input) {
                // todo
            }

            template <typename Pred>
            void erase_if(Pred&& predicate) {
                // todo
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_FILE_GATE_HPP
