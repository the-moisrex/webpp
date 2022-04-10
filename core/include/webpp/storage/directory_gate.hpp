#ifndef WEBPP_STORAGE_DIRECTORY_GATE_HPP
#define WEBPP_STORAGE_DIRECTORY_GATE_HPP

#include "../convert/lexical_cast.hpp"
#include "../crypto/base64.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "cache_concepts.hpp"

#include <filesystem>
#include <fstream>
#include <random>

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
            stl::string_view extension      = ".cache"; // todo: add webpp version number here
            bool             encode_options = true;     // todo: see if you need to remove this
            bool             hash_keys      = true;
            bool             encrypt_values = false;
        };

        template <typename StorageGateType>
        struct file_iterator : stl::filesystem::directory_iterator {
            using dir_iter          = stl::filesystem::directory_iterator;
            using storage_gate_type = StorageGateType;
            using key_type          = typename storage_gate_type::key_type;
            using value_type        = typename storage_gate_type::value_type;
            using options_type      = typename storage_gate_type::options_type;
            using data_type         = typename storage_gate_type::data_type;
            using string_view_type  = typename storage_gate_type::string_view_type;
            using path_string_type  = stl::string;

          private:
            storage_gate_type&       gate;
            stl::optional<data_type> data;

          public:
            file_iterator(path_string_type const& dir, storage_gate_type& the_gate)
              : dir_iter{dir},
                gate{the_gate} {}

            file_iterator& operator++() {
                dir_iter::operator++();

                if (this->operator->()->path().extension() != gate.gate_opts.extension) {
                    return operator++();
                }
                auto const key_str = this->operator->()->path().stem().string();
                data = gate.get(gate.deserialize_key(istl::string_viewify_of<string_view_type>(key_str)));
                if (!data) {
                    return operator++();
                }

                return *this;
            }

            key_type key() const {
                return data->key;
            }

            value_type value() const {
                return data->value;
            }

            options_type options() const {
                return data->options;
            }
        };

        template <Traits TraitsType, CacheFileKey KeyT, CacheFileValue ValueT, CacheFileOptions OptsT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type      = TraitsType;
            using etraits          = enable_traits<TraitsType>;
            using path_type        = stl::filesystem::path;
            using key_type         = traits::generalify_allocators<traits_type, KeyT>;
            using value_type       = traits::generalify_allocators<traits_type, ValueT>;
            using options_type     = traits::generalify_allocators<traits_type, OptsT>;
            using string_type      = traits::general_string<traits_type>;
            using iterator         = file_iterator<storage_gate>;
            using const_iterator   = iterator;
            using char_type        = typename string_type::value_type;
            using string_view_type = traits::string_view<traits_type>;
            using data_type        = cache_tuple<key_type, value_type, options_type>;

          private:
            friend iterator;

            path_type    dir{};
            gate_options gate_opts{};

            string_type serialize_key(key_type const& key) {
                if (gate_opts.hash_keys) {
                    return lexical::cast<string_type>(stl::hash<key_type>{}(key), this->alloc_pack);
                }
                return lexical::cast<string_type>(key, this->alloc_pack);
            }

            string_type serialize_opts(options_type const& opts) {
                auto opts_str = lexical::cast<traits::general_string<traits_type>>(opts, this->alloc_pack);
                if (gate_opts.encode_options) {
                    base64::encode(opts_str, opts_str);
                }
                return opts_str;
            }


            string_type serialize_file(data_type const& data) {
                string_type data_str  = object::make_general<string_type>(this->alloc_pack);
                string_type key_str   = lexical::cast<string_type>(data.key, this->alloc_pack);
                auto        value_str = lexical::cast<string_type>(data.value, this->alloc_pack);
                auto        opts_str  = serialize_opts(data.options);
                if (gate_opts.encrypt_values) {
                    // todo
                }
                data_str.reserve(key_str.size() + 1 + opts_str.size() + 1 + value_str.size());
                data_str.append(key_str);
                data_str.push_back('\n');
                data_str.append(opts_str);
                data_str.push_back('\n');
                data_str.append(value_str);
                return data_str;
            }

            key_type deserialize_key(string_view_type key) {
                return lexical::cast<key_type>(key, this->alloc_pack);
            }

            /**
             * A cache file's syntax is like this:
             *   key\n
             *   options\n
             *   value
             */
            stl::optional<data_type> deserialize_file(string_view_type data) {
                const auto end_key_index = data.find_first_of('\n');
                if (end_key_index == string_type::npos) {
                    this->logger.error(
                      DIR_GATE_CAT,
                      "Cache data is invalid. Cannot find the key name inside the cache file.");
                    return stl::nullopt;
                }

                const auto end_options_index = data.find_first_of('\n', end_key_index + 1);
                if (end_options_index == string_type::npos) {
                    this->logger.error(
                      DIR_GATE_CAT,
                      "Cache data is invalid. Cannot find the options inside the cache file.");
                    return stl::nullopt;
                }
                string_view_type key_str = data.substr(0, end_key_index);

                // decoding options
                options_type opts;
                if (gate_opts.encode_options) {
                    opts = lexical::cast<options_type>(data.substr(end_key_index + 1, end_options_index),
                                                       this->alloc_pack);
                } else {
                    auto        opts_str     = data.substr(end_key_index + 1, end_options_index);
                    string_type decoded_opts = object::make_general<string_type>(this->alloc_pack);
                    if (base64::decode(opts_str, decoded_opts)) {
                        opts = lexical::cast<options_type>(decoded_opts, this->alloc_pack);
                    } else {
                        this->logger.error(DIR_GATE_CAT, "Error decoding options.");
                        return stl::nullopt;
                    }
                }

                // decoding value
                if (gate_opts.encrypt_values) {
                    // todo
                }

                return data_type{
                  .key     = lexical::cast<key_type>(key_str, this->alloc_pack),
                  .value   = lexical::cast<value_type>(data.substr(end_options_index + 1), this->alloc_pack),
                  .options = opts};
            }

            void set_temp_dir() {
                stl::error_code ec;
                dir = stl::filesystem::temp_directory_path(ec);
                if (ec) {
                    this->logger.error(
                      DIR_GATE_CAT,
                      "Cannot get the OS's temp directory and you're not passing us any directory path to use as the cache directory.");
                    return;
                }
                auto random_str = object::make_local<string_type>(this->alloc_pack,
                                                                  "0123456789"
                                                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                                  "abcdefghijklmnopqrstuvwxyz");
                std::shuffle(random_str.begin(), random_str.end(), stl::mt19937{stl::random_device{}()});
                random_str.resize(32);
                dir.append(random_str.begin(), random_str.end());
                if (stl::filesystem::exists(dir)) {
                    set_temp_dir();
                } else {
                    stl::filesystem::create_directory(dir, ec);
                    if (ec) {
                        this->logger.error(
                          DIR_GATE_CAT,
                          "Cannot create a temp directory for cache files. Pass a good directory as input.");
                    }
                }
            }

          public:
            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&&         et,
                         path_type    cache_dir, // empty string will create a temp directory
                         gate_options input_opts)
              : etraits{et},
                dir{stl::move(cache_dir)},
                gate_opts{input_opts} {

                // create a temp directory and use that if the specified path is empty
                if (dir.empty()) {
                    set_temp_dir();
                }
            }


            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&&      et,
                         path_type cache_dir // empty string will create a temp directory
                         )
              : etraits{et},
                dir{stl::move(cache_dir)} {

                // create a temp directory and use that if the specified path is empty
                if (dir.empty()) {
                    set_temp_dir();
                }
            }


            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&& et) : etraits{et} {
                set_temp_dir();
            }


            path_type cache_dir() const noexcept {
                return dir;
            }

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

            stl::optional<data_type> get(key_type const& key) {
                if (!has(key))
                    return stl::nullopt;

                path_type file = key_path(key);
                if (stl::basic_ifstream<char_type> ifs(file); ifs.is_open()) {
                    ifs.seekg(0, ifs.end);
                    const auto size   = ifs.tellg();
                    auto       result = object::make_general<string_type>(*this);
                    // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                    result.resize(static_cast<stl::size_t>(size));
                    ifs.seekg(0);
                    ifs.read(result.data(), size);
                    ifs.close();
                    return deserialize_file(result);
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
                    auto const data = serialize_file({.key = key, .value = value, .options = opts});
                    ofs.write(data.data(), data.size() * sizeof(char_type));
                    ofs.close();
                } else {
                    this->logger.error(DIR_GATE_CAT,
                                       fmt::format("Cannot write the cache to the file {}", file.string()));
                }
            }


            void set_options(key_type const& key, options_type opts) {
                // I'm disabling the encryption for performance gains
                const gate_options old_gate_opts = gate_opts;
                gate_opts.encrypt_values         = false;
                auto data                        = get(key).value_or(data_type{key, value_type{}, opts});
                set(data.key, std::move(data.value), opts);
                gate_opts = old_gate_opts; // restore the old gate options
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
                    auto const key_str = file.path().stem().string();
                    auto const data =
                      get(deserialize_key(istl::string_viewify_of<string_view_type>(key_str)));
                    if (!data)
                        continue;
                    if (predicate(data.value())) {
                        fs::remove(file.path(), ec);
                        if (ec) {
                            this->logger.error(DIR_GATE_CAT,
                                               fmt::format("Cannot remove cache file {} (key name: {})",
                                                           file.path().string(),
                                                           data->key),
                                               ec);
                        }
                    }
                }
            }

            const_iterator begin() const {
                return {dir, *this};
            }

            const_iterator end() const {
                return {"", *this};
            }

            iterator begin() {
                return {dir, *this};
            }

            iterator end() {
                return {"", *this};
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_DIRECTORY_GATE_HPP
