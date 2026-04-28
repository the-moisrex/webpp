#ifndef WEBPP_STORAGE_FILE_GATE_HPP
#define WEBPP_STORAGE_FILE_GATE_HPP

#include "./cache_concepts.hpp"

#include <filesystem>
#include <fstream>
#include <system_error>

namespace webpp {

    /**
     * File Gate stores the cached data in a file
     */
    struct file_gate {
        template <CacheFileKey     KeyT,
                  CacheFileValue   ValueT,
                  CacheFileOptions OptsT,
                  istl::CharType   CharT,
                  Allocator        AllocT>
        struct storage_gate {
            using key_type     = KeyT;
            using value_type   = ValueT;
            using options_type = OptsT;
            using path_type    = stl::filesystem::path;
            using bundle_type  = cache_tuple<key_type, value_type, options_type>;

          private:
            path_type cache_dir;

            static constexpr bool empty_default_options =
              stl::is_empty_v<options_type> && stl::default_initializable<options_type>;

            path_type get_path(key_type const& key) const {
                // Using a hash of the string representation to ensure filesystem-safe filenames
                auto const str_key  = lexical::cast<stl::string>(key);
                auto const hash_val = stl::hash<stl::string>{}(str_key);
                return cache_dir / stl::to_string(hash_val);
            }

            stl::string serialize_options([[maybe_unused]] options_type const& opts) const {
                if constexpr (empty_default_options) {
                    return {};
                } else {
                    return lexical::cast<stl::string>(opts);
                }
            }

            options_type deserialize_options([[maybe_unused]] stl::string const& opts) const {
                if constexpr (empty_default_options) {
                    return {};
                } else {
                    return lexical::cast<options_type>(opts);
                }
            }

          public:
            explicit storage_gate(path_type output_dir) : cache_dir{stl::move(output_dir)} {
                stl::error_code err;
                stl::filesystem::create_directories(cache_dir, err);
            }

            template <typename K>
            stl::optional<bundle_type> get(K&& key) {
                auto const filepath = get_path(key);

                // Open at the end to get file size for efficient allocation
                stl::ifstream file(filepath, stl::ios::binary | stl::ios::ate);
                if (!file) {
                    return stl::nullopt;
                }

                auto const file_size = file.tellg();
                if (file_size <= 0) {
                    return stl::nullopt;
                }
                file.seekg(0, stl::ios::beg);

                stl::string key_str;
                stl::string opt_str;
                stl::string val_str;

                // Format: [key string] \n [opts string] \n [value string]
                stl::getline(file, key_str);
                stl::getline(file, opt_str);

                if (file.fail()) {
                    return stl::nullopt;
                }
                auto const header_size    = file.tellg();
                auto const remaining_size = file_size - header_size;

                if (remaining_size > 0) {
                    val_str.resize(static_cast<stl::size_t>(remaining_size));
                    file.read(val_str.data(), remaining_size);
                }

                return bundle_type{.key     = stl::forward<K>(key),
                                   .value   = lexical::cast<value_type>(val_str),
                                   .options = deserialize_options(opt_str)};
            }

            template <typename K, typename V>
            void set(K&& key, V&& value, options_type opts = {}) {
                auto const    filepath = get_path(stl::forward<K>(key));
                stl::ofstream file(filepath, stl::ios::binary | stl::ios::trunc);
                if (file) {
                    file << lexical::cast<stl::string>(stl::forward<K>(key)) << '\n'
                         << serialize_options(opts) << '\n'
                         << lexical::cast<stl::string>(stl::forward<V>(value));
                }
            }

            void set_options(key_type const& key, options_type opts) {
                if (auto bundle_opt = get(key)) {
                    set(key, bundle_opt->value, stl::move(opts));
                }
            }

            template <typename K>
            void erase(K&& input) {
                stl::error_code err;
                stl::filesystem::remove(get_path(stl::forward<K>(input)), err);
            }

            template <typename Pred>
            void erase_if(Pred&& predicate) {
                stl::error_code err;
                for (auto const& entry : stl::filesystem::directory_iterator(cache_dir, err)) {
                    if (!entry.is_regular_file(err)) {
                        continue;
                    }

                    stl::ifstream file(entry.path(), stl::ios::binary | stl::ios::ate);
                    if (!file) {
                        continue;
                    }

                    auto const file_size = file.tellg();
                    file.seekg(0, stl::ios::beg);

                    stl::string key_str, opt_str, val_str;
                    stl::getline(file, key_str);
                    stl::getline(file, opt_str);

                    auto const remaining_size = file_size - file.tellg();
                    if (remaining_size > 0) {
                        val_str.resize(static_cast<stl::size_t>(remaining_size));
                        file.read(val_str.data(), remaining_size);
                    }

                    file.close(); // Close file before potentially removing it

                    bundle_type bundle{.key     = lexical::cast<key_type>(key_str),
                                       .value   = lexical::cast<value_type>(val_str),
                                       .options = deserialize_options(opt_str)};

                    if (predicate(bundle)) {
                        stl::filesystem::remove(entry.path(), err);
                    }
                }
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_FILE_GATE_HPP
