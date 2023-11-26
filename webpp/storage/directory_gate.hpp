#ifndef WEBPP_STORAGE_DIRECTORY_GATE_HPP
#define WEBPP_STORAGE_DIRECTORY_GATE_HPP

#include "../convert/lexical_cast.hpp"
#include "../crypto/base64.hpp"
#include "../memory/object.hpp"
#include "../storage/file.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "../version.hpp"
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
        static constexpr stl::string_view DIR_GATE_CAT           = "DirGate";
        static constexpr stl::string_view directory_gate_version = "1.0.0";

        struct gate_options {
            stl::string extension      = ".cache";
            bool        encode_options = true; // todo: see if you need to remove this
            bool        hash_keys      = true;
            bool        encrypt_values = false;
        };

        template <typename StorageGateType>
        struct file_iterator;

        template <typename StorageGateType>
        struct file_iterator {
            using dir_iter_type     = stl::filesystem::directory_iterator;
            using storage_gate_type = StorageGateType;
            using data_key_type     = typename storage_gate_type::key_type;
            using data_value_type   = typename storage_gate_type::value_type;

            using value_type        = stl::pair<data_key_type, data_value_type>;
            using difference_type   = ptrdiff_t;
            using pointer           = value_type const*;
            using reference         = value_type const&;
            using iterator_category = stl::input_iterator_tag;

            using string_view_type = typename storage_gate_type::string_view_type;

          private:
            dir_iter_type             dir_iter{};
            storage_gate_type*        gate;
            stl::optional<value_type> data{};
            stl::error_code           ec;

            void deserialize() {
                if (!dir_iter->exists(ec)) {
                    return;
                }
                if (ec) {
                    gate->logger.error(DIR_GATE_CAT, "Cannot check the existence of a cache file.", ec);
                    return;
                }
                auto const real_data = gate->get_file(dir_iter->path());
                if (!real_data) {
                    data = stl::nullopt;
                }
                data = value_type{real_data->key, real_data->value};
            }

            [[nodiscard]] bool is_valid() const {
                return data.has_value();
            }

            void reset() {
                dir_iter = dir_iter_type{};
                data     = stl::nullopt;
            }

          public:
            file_iterator(storage_gate_type& the_gate) : dir_iter{}, gate{&the_gate} {}

            file_iterator(stl::filesystem::path const& dir, storage_gate_type& the_gate)
              : dir_iter{dir, ec},
                gate{&the_gate} {
                if (ec) {
                    gate->logger.error(DIR_GATE_CAT, "Cache directory traverse error.", ec);
                } else {
                    deserialize();
                }
            }

            file_iterator(file_iterator const&)                = default;
            file_iterator(file_iterator&&) noexcept            = default;
            file_iterator& operator=(file_iterator const&)     = default;
            file_iterator& operator=(file_iterator&&) noexcept = default;
            ~file_iterator()                                   = default;

            bool operator==(file_iterator const& rhs) const {
                return dir_iter == rhs.dir_iter && gate == rhs.gate;
            }

            bool operator!=(file_iterator const& rhs) const {
                return !operator==(rhs);
            }

            value_type const& operator*() const noexcept {
                return *data;
            }

            stl::optional<value_type> operator->() const noexcept {
                return data;
            }

            file_iterator& operator++() {
                dir_iter.increment(ec);
                if (ec || dir_iter == dir_iter_type{}) {
                    // we got to the end
                    reset();
                    return *this;
                }
                if (!gate->is_our_cache(dir_iter->path())) {
                    return operator++();
                }
                deserialize();
                if (!is_valid()) {
                    return operator++();
                }
                return *this;
            }
        };

        template <Traits TraitsType, CacheFileKey KeyT, CacheFileValue ValueT, CacheFileOptions OptsT>
        struct storage_gate : enable_traits<TraitsType> {
            using traits_type      = TraitsType;
            using etraits          = enable_traits<TraitsType>;
            using path_type        = stl::filesystem::path;
            using key_type         = KeyT;
            using value_type       = ValueT;
            using options_type     = OptsT;
            using string_type      = traits::general_string<traits_type>;
            using iterator         = file_iterator<storage_gate>;
            using const_iterator   = iterator;
            using char_type        = typename string_type::value_type;
            using string_view_type = traits::string_view<traits_type>;
            using bundle_type      = cache_tuple<key_type, value_type, options_type>;

          private:
            friend iterator;

            path_type    dir{};
            string_type  hashed_name;
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

            string_type serialize_file(bundle_type const& data) {
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

            /**
             * A cache file's syntax is like this:
             *   key\n
             *   options\n
             *   value
             */
            stl::optional<bundle_type> deserialize_file(string_view_type data) {
                auto const end_key_index = data.find_first_of('\n');
                if (end_key_index == string_type::npos) {
                    this->logger.error(
                      DIR_GATE_CAT,
                      "Cache data is invalid. Cannot find the key name inside the cache file.");
                    return stl::nullopt;
                }

                auto const end_options_index = data.find_first_of('\n', end_key_index + 1);
                if (end_options_index == string_type::npos) {
                    this->logger.error(
                      DIR_GATE_CAT,
                      "Cache data is invalid. Cannot find the options inside the cache file.");
                    return stl::nullopt;
                }
                string_view_type key_str = data.substr(0, end_key_index);

                // decoding options
                options_type opts;
                if (!gate_opts.encode_options) {
                    opts = lexical::cast<options_type>(
                      data.substr(end_key_index + 1, end_options_index - (end_key_index + 1)),
                      this->alloc_pack);
                } else {
                    auto opts_str = data.substr(end_key_index + 1, end_options_index - (end_key_index + 1));
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

                return bundle_type{
                  .key     = lexical::cast<key_type>(key_str, this->alloc_pack),
                  .value   = lexical::cast<value_type>(data.substr(end_options_index + 1), this->alloc_pack),
                  .options = opts};
            }

            void set_temp_dir() {
                static constexpr stl::size_t temp_dir_size = 32;
                stl::error_code              ec;
                dir = stl::filesystem::temp_directory_path(ec);
                if (ec) {
                    this->logger.error(DIR_GATE_CAT,
                                       "Cannot get the OS's temp directory and you're not passing us any "
                                       "directory path to use as the cache directory.",
                                       ec);
                    return;
                }
                auto random_str = object::make_local<string_type>(
                  this->alloc_pack,
                  "0123456789"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz");
                std::shuffle(random_str.begin(), random_str.end(), stl::mt19937{stl::random_device{}()});
                random_str.resize(temp_dir_size);
                dir.append(random_str.begin(), random_str.end());
                if (stl::filesystem::exists(dir)) {
                    set_temp_dir();
                } else {
                    stl::filesystem::create_directory(dir, ec);
                    if (ec) {
                        this->logger.error(
                          DIR_GATE_CAT,
                          "Cannot create a temp directory for cache files. Pass a good directory as input.",
                          ec);
                    }
                }
            }

            template <typename NameT>
            string_type hash_name(auto&& et, NameT&& name) const {
                return lexical::cast<string_type>(
                  stl::hash<string_type>{}(
                    lexical::cast<string_type>(stl::forward<NameT>(name), et.alloc_pack)),
                  et);
            }

          public:
            template <typename ET, typename NameT>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&&         et,
                         path_type    cache_dir, // empty string will create a temp directory
                         NameT&&      name,
                         gate_options input_opts)
              : etraits{et},
                dir{stl::move(cache_dir)},
                hashed_name{hash_name(et, stl::forward<NameT>(name))},
                gate_opts{stl::move(input_opts)} {
                // create a temp directory and use that if the specified path is empty
                if (dir.empty()) {
                    set_temp_dir();
                }
            }

            template <typename ET, typename NameT>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&&      et,
                         path_type cache_dir, // empty string will create a temp directory
                         NameT&&   name)
              : etraits{et},
                dir{stl::move(cache_dir)},
                hashed_name{hash_name(et, stl::forward<NameT>(name))} {
                // create a temp directory and use that if the specified path is empty
                if (dir.empty()) {
                    set_temp_dir();
                }
            }

            template <typename ET>
                requires(EnabledTraits<ET> && !stl::same_as<ET, storage_gate const&> &&
                         !stl::same_as<ET, storage_gate &&>)
            storage_gate(ET&& et, path_type cache_dir)
              : etraits{et},
                dir{stl::move(cache_dir)} {
                if (dir.empty()) {
                    set_temp_dir();
                }
            }

            // NOLINTBEGIN(bugprone-forwarding-reference-overload)
            template <EnabledTraits ET>
                requires(!stl::same_as<stl::remove_cvref_t<ET>, storage_gate>)
            storage_gate(ET&& et) : etraits{et},
                                    hashed_name{hash_name(et, "default")} {
                set_temp_dir();
            }

            // NOLINTEND(bugprone-forwarding-reference-overload)


            bool operator==(storage_gate const& rhs) const {
                return dir == rhs.dir;
            }

            bool operator!=(storage_gate const& rhs) const {
                return dir != rhs.dir;
            }

            path_type key_path(key_type const& key) {
                path_type file  = dir;
                file           /= hashed_name;
                file           += "-";
                file           += serialize_key(key);
                file           += ".v";
                file           += directory_gate_version;
                file           += gate_opts.extension;
                return file;
            }

            [[nodiscard]] bool is_our_cache(path_type const& path) const {
                // These things are checked:
                //  - extension
                //  - hashed name
                //  - directory cache version
                return (path.extension() == gate_opts.extension) &&
                       (path.stem().string().starts_with(hashed_name)) &&
                       (path.stem().string().ends_with(directory_gate_version));
            }

            // check if the specified key exists
            [[nodiscard]] bool has(key_type const& key) {
                return stl::filesystem::exists(key_path(key));
            }

            stl::optional<bundle_type> get_file(path_type const& filepath) {
                auto       result = object::make_general<string_type>(*this);
                bool const res    = file::read_to(filepath, result);
                if (res) {
                    return deserialize_file(result);
                } else {
                    this->logger.error(DIR_GATE_CAT,
                                       fmt::format("Cannot read the cache file {}", filepath.string()));
                    return stl::nullopt;
                }
            }

            stl::optional<bundle_type> get(key_type const& key) {
                if (!has(key)) {
                    return stl::nullopt;
                }

                path_type file = key_path(key);
                return get_file(file);
            }

            template <typename K, typename V>
            void set(K&& key, V&& value, options_type opts = {}) {
                path_type file = key_path(key);
                if (stl::basic_ofstream<char_type> ofs(file); ofs.good()) {
                    // todo: handle errors
                    auto const data = serialize_file({.key = key, .value = value, .options = opts});
                    ofs.write(data.data(), static_cast<stl::streamsize>(data.size() * sizeof(char_type)));
                    ofs.close();
                } else {
                    this->logger.error(DIR_GATE_CAT,
                                       fmt::format("Cannot write the cache to the file {}", file.string()));
                }
            }

            void set_options(key_type const& key, options_type opts) {
                // I'm disabling the encryption for performance gains
                gate_options const old_gate_opts = gate_opts;
                gate_opts.encrypt_values         = false;
                auto data                        = get(key).value_or(bundle_type{key, value_type{}, opts});
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

            void clear() {
                stl::error_code ec;
                for (auto const& file : stl::filesystem::directory_iterator(dir, ec)) {
                    if (!is_our_cache(file.path())) {
                        continue;
                    }
                    stl::filesystem::remove(file, ec);
                    if (ec) {
                        this->logger.error(DIR_GATE_CAT, "Cannot remove cache file.", ec);
                        ec.clear();
                    }
                }
                if (ec) {
                    this->logger.error(DIR_GATE_CAT, "Cannot traverse the directory", ec);
                }
            }

            template <typename Pred>
            void erase_if(Pred&& predicate) {
                namespace fs = stl::filesystem;

                stl::error_code ec;
                for (auto const& file : stl::filesystem::directory_iterator(dir, ec)) {
                    if (!is_our_cache(file.path())) {
                        continue;
                    }
                    auto const data = get_file(file);
                    if (!data) {
                        continue;
                    }
                    if (predicate(data.value())) {
                        fs::remove(file.path(), ec);
                        if (ec) {
                            this->logger.error(
                              DIR_GATE_CAT,
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
                return {*this};
            }

            iterator begin() {
                return {dir, *this};
            }

            iterator end() {
                return {*this};
            }
        };
    };

} // namespace webpp

#endif // WEBPP_STORAGE_DIRECTORY_GATE_HPP
