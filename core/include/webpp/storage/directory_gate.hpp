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
            stl::filesystem::path::string_type extension      = ".cache";
            bool                               hash_keys      = true;
            bool                               encrypt_values = false;
        };

        template <CacheFileKey KeyT, CacheFileValue ValT>
        struct file_iterator : stl::filesystem::directory_iterator {

            using key_type   = KeyT;
            using value_type = ValT;
            using key_ref    = stl::add_lvalue_reference_t<key_type>;
            using value_ref  = stl::add_lvalue_reference_t<value_type>;

            key_type key() const {
                return this->path()->stem();
            }

            value_type value() const {
                return;
            }
        };

        template <Traits TraitsType, CacheFileKey KeyT, CacheFileValue ValueT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type    = TraitsType;
            using etraits        = enable_traits<TraitsType>;
            using path_type      = stl::filesystem::path;
            using key_type       = traits::generalify_allocators<traits_type, KeyT>;
            using value_type     = traits::generalify_allocators<traits_type, ValueT>;
            using string_type    = typename path_type::string_type;
            using iterator       = file_iterator<key_type, value_type>;
            using const_iterator = const iterator;

          private:
            path_type dir;
            options   opts;

            string_type serialize_key(key_type const& key) {
                if (opts.hash_keys) {
                    return lexical::cast<string_type>(stl::hash<key_type>{}(key), this->alloc_pack);
                }
                return lexical::cast<string_type>(key, this->alloc_pack);
            }

            key_type deserialize_key(string_type const& key) {
                if (opts.hash_keys) {
                    // todo: hash the keys
                }
                return lexical::cast<key_type>(key, this->alloc_pack);
            }

          public:
            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&& et, options input_opts = {}) // NOLINT(cppcoreguidelines-pro-type-member-init)
              : etraits{et},
                opts{input_opts} {}

            path_type key_path(key_type const& key) {
                path_type file = dir;
                file /= serialize_key(key);
                file += opts.extension;
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
                    this->logger.warning(
                      DIR_GATE_CAT,
                      fmt::format("Cannot remove cache file {} (key name: {})", key_file, key),
                      ec);
                    return false;
                }
                return true;
            }

            template <typename Pred>
            void erase_if(Pred&& predicate) {
                namespace fs = stl::filesystem;

                stl::error_code ec;
                for (auto const& file : *this) {
                    if (file.path().extension() != opts.extension)
                        continue;
                    const key_type   key = deserialize_key(file.path().stem());
                    const value_type value{}; // todo
                    if (predicate(stl::pair<key_type, value_type>{key, value})) {
                        fs::remove(file.path(), ec);
                        if (ec) {
                            this->logger.warning(DIR_GATE_CAT,
                                                 fmt::format("Cannot remove cache file {} (key name: {})",
                                                             file.path().string(),
                                                             key),
                                                 ec);
                        }
                    }
                }
            }

            const_iterator begin() const {
                return {dir};
            }

            const_iterator end() const {
                return {};
            }

            iterator begin() {
                return {dir};
            }

            iterator end() {
                return {};
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_DIRECTORY_GATE_HPP
