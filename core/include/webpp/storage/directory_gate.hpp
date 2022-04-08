#ifndef WEBPP_STORAGE_DIRECTORY_GATE_HPP
#define WEBPP_STORAGE_DIRECTORY_GATE_HPP

#include "../convert/lexical_cast.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "cache_concepts.hpp"

#include <filesystem>

namespace webpp {

    /**
     * Directory Gate
     *
     * This class will help the cache to store data in a directory;
     * Each file is a cache of its own.
     */
    struct directory_gate {

        static constexpr stl::string_view DIR_GATE_CAT = "DirGate";

        struct options {
            bool hash_keys      = true;
            bool encrypt_values = false;
        };

        template <Traits TraitsType, CacheKey KeyT, CacheValue ValueT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type = TraitsType;
            using etraits     = enable_traits<TraitsType>;
            using path_type   = stl::filesystem::path;
            using key_type    = traits::generalify_allocators<traits_type, KeyT>;
            using value_type  = traits::generalify_allocators<traits_type, ValueT>;
            using string_type = typename path_type::string_type;

          private:
            path_type dir;
            options   opts;

          public:
            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&& et, options input_opts = {}) // NOLINT(cppcoreguidelines-pro-type-member-init)
              : etraits{et},
                opts{input_opts} {}

            path_type key_path(key_type const& key) {
                path_type file = dir;
                if (opts.hash_keys) {
                    // todo: hash the keys
                }
                file /= lexical::cast<string_type>(key);
                return file;
            }

            // check if the specified key exists
            bool exists(key_type const& key) {
                return stl::filesystem::exists(key_path(key));
            }

            stl::optional<value_type> get(key_type const& key) {
                return stl::nullopt;
            }

            template <typename K, typename V>
            void set(K&& key, V&& value) {}


            bool erase(key_type const& key) noexcept {
                stl::error_code ec;
                auto const      key_file = key_path(key);
                stl::filesystem::remove(key_file, ec);
                if (ec) {
                    this->logger.warning(DIR_GATE_CAT,
                                         fmt::format("Cannot remove cache file {}", key_file),
                                         ec);
                    return false;
                }
                return true;
            }

            template <typename Pred>
            void erase_if(Pred&& predicate) {
                namespace fs = stl::filesystem;

                stl::error_code ec;
                for (auto const& file : fs::directory_iterator{dir}) {
                    if (predicate(stl::pair<key_type, value_type>{key, value})) {
                        fs::remove(file.path(), ec);
                        if (ec) {
                            this->logger.warning(DIR_GATE_CAT,
                                                 fmt::format("Cannot remove cache file {}", key_file),
                                                 ec);
                        }
                    }
                }
            }

            auto begin() const {
                return fs::directory_iterator{dir};
            }

            auto end() const {
                return map.begin();
            }

            auto begin() {
                return map.begin();
            }

            auto end() {
                return map.begin();
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_DIRECTORY_GATE_HPP
