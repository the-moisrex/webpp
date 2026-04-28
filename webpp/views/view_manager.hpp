// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_MANAGER_HPP
#define WEBPP_VIEW_MANAGER_HPP

#include "../http/http_concepts.hpp"
#include "../std/format.hpp"
#include "../std/string.hpp"
#include "../storage/file.hpp"
#include "../storage/lru_cache.hpp"
#include "../storage/memory_gate.hpp"
#include "../storage/null_gate.hpp"
#include "data_view_caster.hpp"
#include "file_view.hpp"
#include "mustache_view.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <system_error>
#include <type_traits>
#include <variant>

namespace webpp::views {


    /**
     * View Manager
     */
    template <istl::CharType CharT, Allocator AllocT>
    struct view_manager {
        using char_type        = CharT;
        using string_type      = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;
        using string_view_type = stl::basic_string_view<CharT>;
        using path_type        = stl::filesystem::path;
        using view_roots_type =
          stl::vector<path_type, typename stl::allocator_traits<AllocT>::template rebind_alloc<path_type>>;
        using ifstream_type = stl::basic_ifstream<char_type>;

        using mustache_view_type = mustache_view<CharT, AllocT>;
        using allocator_type     = AllocT;
        using file_view_type     = file_view<char_type, allocator_type>;

        static constexpr stl::size_t default_cache_limit = 100u;
        static constexpr auto        logging_category    = "ViewMan";

      private:
        // fixme: variant is not the best strategy, a file doesn't require as much storage as a mustache view
        using view_types = stl::variant<mustache_view_type, /*json_view_type,*/ file_view_type>;


        using mustache_data_type = typename mustache_view_type::data_type;
        // using json_data_type = typename json_view_type::data_type;
        using file_data_type     = typename file_view_type::data_type;
        using cache_type         = lru_cache<path_type, view_types, memory_gate<null_gate>>;

        static constexpr stl::array<string_view_type, 1> valid_extensions{".mustache"};


        cache_type cached_views;


      public:
        // the root directories where we can find the views
        view_roots_type view_roots; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

        explicit constexpr view_manager(stl::size_t    cache_limit = default_cache_limit,
                                        allocator_type inp_alloc   = alloc) noexcept
          : cached_views{cache_limit},
            view_roots{inp_alloc} {}


      private:
        /**
         * Find the file based on the specified view name.
         *
         * Features:
         *   - Auto extension detection based on the stem name
         *   - Variables Support
         *   - Recursive finder: Add '*' at the start, and then let us recursively search for it
         *
         * Possible syntax:
         *   - [ ] /absolute/path/to/file.json
         *   - [ ] file_in_one_of_the_root_dirs.json
         *   - [ ] ./relative/is/possible/too.json
         *   - [ ] layout/header
         *   - [ ] *header.html
         */
        [[nodiscard]] stl::optional<path_type> find_file(stl::string_view request) const noexcept {
            namespace fs = stl::filesystem;

            stl::error_code err;

            // an absolute path should
            if (request.starts_with('/')) {
                path_type const file{request};
                if (!fs::is_regular_file(file, err)) {
                    if (err) {
                        logger.error(logging_category,
                                     fmt::format("Cannot check file details for {}", file.string()),
                                     err);
                    }
                    return stl::nullopt;
                }
                return file;
            }

            // a search for all request
            bool const recursive_search = request.starts_with('*');
            if (recursive_search) {
                request.remove_prefix(1);
            }

            // traverse the root directories
            for (path_type dir : view_roots) {
                fs::file_status status = fs::status(dir, err);
                if (err && !fs::status_known(status)) {
                    logger.error(logging_category,
                                 fmt::format("Cannot check directory status of {}", dir.string()),
                                 err);
                    continue;
                }

                // make sure the path is a directory
                if (!fs::is_directory(status)) {
                    continue;
                }

                if (recursive_search) {
                    fs::recursive_directory_iterator const iter(dir, err);
                    if (err) {
                        logger.error(logging_category, fmt::format("Cannot read dir {}", dir.string()), err);
                        continue;
                    }
                    fs::recursive_directory_iterator       it     = fs::begin(iter);
                    fs::recursive_directory_iterator const it_end = fs::end(iter);
                    for (; it != it_end; it.increment(err)) {
                        if (err) {
                            logger.error(logging_category,
                                         fmt::format("Cannot traverse directory {}", dir.string()),
                                         err);
                            continue;
                        }
                        path_type const file      = *it;
                        auto const      file_stem = file.stem();
                        if (file_stem == request) {
                            goto found_it;
                        }
                        // The user may have omitted the extension, we are searching for them
                        for (string_view_type ext : valid_extensions) {
                            auto new_file = file;
                            new_file.concat(ext.begin(), ext.end()); // append the extension

                            if (file_stem == new_file) {
                                goto found_it;
                            }
                        }

                        // did not find it
                        continue;
                    found_it:
                        status = fs::status(file, err);
                        if (fs::is_regular_file(status)) {
                            return file;
                        }
                        if (err && !fs::status_known(status)) {
                            logger.error(logging_category,
                                         fmt::format("Cannot check file type of {}", dir.string()),
                                         err);
                        }
                    }
                } else { // non-recursive normal path appending
                    dir.append(request.begin(), request.end());
                    status = fs::status(dir, err);
                    if (fs::exists(status)) {
                        return dir;
                    }
                    if (err && !fs::status_known(status)) {
                        logger.error(logging_category, fmt::format("Cannot check file type of {}", dir.string()), err);
                        // no need to "continue"
                    }


                    // The user may have omitted the extension, we are searching for them
                    for (string_view_type ext : valid_extensions) {
                        auto file = dir;
                        file.concat(ext.begin(), ext.end()); // append the extension

                        status = fs::status(file, err);
                        if (fs::exists(status)) {
                            return file;
                        }
                        if (err && !fs::status_known(status)) {
                            logger.error(logging_category,
                                         fmt::format("Cannot check file type of {}", dir.string()),
                                         err);
                        }
                    }
                }
            }

            return stl::nullopt;
        }

        [[nodiscard]] decltype(auto) get_allocator() const noexcept {
            return view_roots.get_allocator();
        }

        /**
         * Read the file content
         */
        bool read_file(stl::filesystem::path const& filepath, istl::String auto& out) const {
            bool const res = file::get_to(filepath, out);
            if (!res) {
                logger.error("Response/File", fmt::format("Cannot load the specified file: {}", filepath.string()));
                // return empty string
            }
            return res;
        }

        template <typename VT>
        [[nodiscard]] auto* get_view(path_type const& file) {
            static view_types default_view{stl::in_place_type<VT>, get_allocator()};
            return cached_views.emplace_get_ptr(file, default_view);
        }

        template <typename ViewType, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, path_type const& file, DataType&&... data) {
            using view_type = ViewType;
            auto* cached    = get_view<view_type>(file);
            auto& view      = stl::get<view_type>(*cached);
            if (!view.has_scheme()) {
                string_type file_content{get_allocator()};
                if (!read_file(file, file_content)) {
                    return; // We weren't able to read the file.
                }
                view.scheme(file_content);
                // since we got a pointer, we don't need to save it the cache again
            }

            // Render the view based on the data that passed to us
            view.render(out, stl::forward<DataType>(data)...);
        }

        template <typename ViewType, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, stl::basic_string_view<CharT> file_request, DataType&&... data) {
            auto const file = find_file(file_request);
            if (!file) {
                logger.error(logging_category, fmt::format("We can't find the specified view {}.", file_request));
                return;
            }
            view_to<ViewType>(out, file.value(), stl::forward<DataType>(data)...);
        }


      public:
        /**
         * This is essentially the same as ".view" but it's specialized for a mustache file.
         */
        [[nodiscard]] constexpr auto mustache(string_view_type const file_request, mustache_data_type const& data) {
            string_type out{get_allocator()};
            view_to<mustache_view_type>(out, file_request, data);
            return out;
        }

        template <typename... StrT2, typename... DataType>
        [[nodiscard]] constexpr auto mustache(string_view_type const file_request, stl::pair<StrT2, DataType>... data) {
            return mustache<string_view_type>(file_request,
                                              view::data_view_caster<mustache_data_type>(*this, stl::move(data)...));
        }

        template <typename... DataType>
            requires(
              !(sizeof...(DataType) == 1 && (stl::same_as<stl::remove_cvref_t<DataType>, mustache_data_type> && ...)))
        [[nodiscard]] constexpr auto mustache(string_view_type const file_request, DataType&&... data) {
            return mustache<string_view_type>(
              file_request,
              view::data_view_caster<mustache_data_type>(*this, stl::forward<DataType>(data)...));
        }

        [[nodiscard]] constexpr string_type file(string_view_type const file_request) {
            string_type out{get_allocator()};
            view_to<file_view_type>(out, file_request);
            return out;
        }

        [[nodiscard]] auto view(string_view_type const file_request) {
            return view(file_request, istl::nothing_type{});
        }

        /**
         * Render a view
         */
        template <typename DT>
            requires(PossibleDataTypes<mustache_view_type, stl::remove_cvref_t<DT>> ||
                     PossibleDataTypes<file_view_type, stl::remove_cvref_t<DT>>)
        [[nodiscard]] auto view(string_view_type const file_request, DT&& data) {
            auto const  file = find_file(file_request);
            string_type out{get_allocator()};
            if (!file) {
                logger.error(logging_category, fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            auto const ext = file->extension().string();
            if (ext.size() >= 1) {
                switch (ext[1]) {
                    case 'm': {
                        if (ext == ".mustache") {
                            view_to<mustache_view_type>(out, file.value(), data);
                            return out;
                        }
                        break;
                    }
                    case 'j': {
                        if (ext == ".json") {
                            // auto view = get_view<json_view_type>(file).view;
                            // view.scheme(file_content);
                            // view.render(out, data);
                        }
                        break;
                    }
                    default: break;
                }
            }
            view_to<file_view_type>(out, file.value());
            return out;
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
