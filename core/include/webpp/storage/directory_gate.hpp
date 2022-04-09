#ifndef WEBPP_STORAGE_DIRECTORY_GATE_HPP
#define WEBPP_STORAGE_DIRECTORY_GATE_HPP

#include "../convert/lexical_cast.hpp"
#include "../crypto/base64.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "cache_concepts.hpp"

#include <filesystem>
#include <fstream>

namespace webpp {

    /**
     * Directory Gate
     *
     * This class will help the cache to store data in a directory;
     * Each file is a cache of its own.
     */
    struct directory_gate {

        static constexpr stl::string_view DIR_GATE_CAT = "DirGate";

        struct gate_options {
            stl::filesystem::path::string_type extension      = ".cache";
            bool                               encode_options = true;
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

        template <Traits TraitsType, CacheFileKey KeyT, CacheFileValue ValueT, CacheFileOptions OptsT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type    = TraitsType;
            using etraits        = enable_traits<TraitsType>;
            using path_type      = stl::filesystem::path;
            using key_type       = traits::generalify_allocators<traits_type, KeyT>;
            using value_type     = traits::generalify_allocators<traits_type, ValueT>;
            using options_type   = traits::generalify_allocators<traits_type, OptsT>;
            using string_type    = typename path_type::string_type;
            using iterator       = file_iterator<key_type, value_type>;
            using const_iterator = const iterator;
            using char_type      = typename string_type::value_type;

          private:
            path_type    dir;
            gate_options gate_opts;

            string_type serialize_key(key_type const& key) {
                if (gate_opts.hash_keys) {
                    return lexical::cast<string_type>(stl::hash<key_type>{}(key), this->alloc_pack);
                }
                return lexical::cast<string_type>(key, this->alloc_pack);
            }

            string_type serialize_value(value_type const& val, options_type const& opts) {
                auto data             = object::make_general<string_type>(this->alloc_pack);
                using new_string_type = typename stl::remove_cvref_t<decltype(data)>::object_type;
                auto value_str        = lexical::cast<new_string_type>(val, this->alloc_pack);
                auto opts_str         = lexical::cast<new_string_type>(opts, this->alloc_pack);
                if (gate_opts.encode_options) {
                    base64::encode(opts_str, opts_str);
                }
                if (gate_opts.encrypt_values) {
                    // todo
                }
                data.reserve(opts_str.size() + 1 + value_str.size());
                data.append(opts_str);
                data.append('\n');
                data.append(value_str);
                return data;
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
            storage_gate(ET&&         et,
                         gate_options input_opts = {}) // NOLINT(cppcoreguidelines-pro-type-member-init)
              : etraits{et},
                gate_opts{input_opts} {}

            path_type key_path(key_type const& key) {
                path_type file = dir;
                file /= serialize_key(key);
                file += gate_opts.extension;
                return file;
            }

            // check if the specified key exists
            bool has(key_type const& key) {
                return stl::filesystem::exists(key_path(key));
            }

            stl::optional<value_type> get(key_type const& key) {
                if (!has(key))
                    return stl::nullopt;

                path_type file = key_path(key);
                if (stl::basic_ifstream<char_type> ifs(file); ifs.is_open()) {
                    const auto size   = ifs.tellg();
                    auto       result = object::make_general<string_type>(*this);
                    result.reserve(static_cast<stl::size_t>(size));
                    ifs.seekg(0);
                    ifs.read(result.data(), size);
                    ifs.close();
                    auto [_, value] = deserialize_value(result);
                    return stl::move(value);
                } else {
                    this->logger.error(DIR_GATE_CAT,
                                       fmt::format("Cannot read the cache file {}", file.string()));
                    return stl::nullopt;
                }
            }

            template <typename K, typename V>
            void set(K&& key, V&& value, options_type opts = {}) {
                path_type file = key_path(key);
                if (stl::basic_ofstream<char_type> ofs(file); ofs.good()) {
                    // todo: handle errors
                    auto const data = serialize_value(value, opts);
                    ofs.write(data.data(), data.size() * sizeof(char_type));
                    ofs.close();
                } else {
                    this->logger.error(DIR_GATE_CAT,
                                       fmt::format("Cannot write the cache to the file {}", file.string()));
                }
            }


            bool erase(key_type const& key) noexcept {
                stl::error_code ec;
                auto const      key_file = key_path(key);
                stl::filesystem::remove(key_file, ec);
                if (ec) {
                    this->logger.error(
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
                    if (file.path().extension() != gate_opts.extension)
                        continue;
                    const key_type   key = deserialize_key(file.path().stem());
                    const value_type value{}; // todo
                    if (predicate(stl::pair<key_type, value_type>{key, value})) {
                        fs::remove(file.path(), ec);
                        if (ec) {
                            this->logger.error(DIR_GATE_CAT,
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
