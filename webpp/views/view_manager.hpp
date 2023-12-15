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
#include "../traits/traits.hpp"
#include "data_view_caster.hpp"
#include "file_view.hpp"
#include "json_view.hpp"
#include "mustache_view.hpp"

#include <filesystem>
#include <fstream>
#include <system_error>
#include <type_traits>
#include <variant>

namespace webpp::views {


    /**
     * View Manager
     */
    template <Traits TraitsType>
    struct view_manager : enable_traits<TraitsType> {
        using etraits          = enable_traits<TraitsType>;
        using traits_type      = TraitsType;
        using string_type      = traits::string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using path_type        = stl::filesystem::path;
        using view_roots_type  = stl::vector<path_type, traits::allocator_type_of<traits_type, path_type>>;
        using char_type        = traits::char_type<traits_type>;
        using ifstream_type    = stl::basic_ifstream<char_type>;

        using mustache_view_type = mustache_view<traits_type>;
        using json_view_type     = json_view<traits_type>;
        using file_view_type     = file_view<traits_type>;

        static constexpr stl::size_t default_cache_limit = 100u;
        static constexpr auto        logging_category    = "ViewMan";

      private:
        // fixme: variant is not the best strategy, a file doesn't require as much storage as a mustache view
        using view_types = stl::variant<mustache_view_type, /*json_view_type,*/ file_view_type>;


        using mustache_data_type = typename mustache_view_type::data_type;
        // using json_data_type = typename json_view_type::data_type;
        using file_data_type     = typename file_view_type::data_type;
        using cache_type         = lru_cache<traits_type, path_type, view_types, memory_gate<null_gate>>;

        static constexpr stl::array<string_view_type, 1> valid_extensions{".mustache"};


        cache_type cached_views;


      public:
        // the root directories where we can find the views
        view_roots_type view_roots; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, view_manager>)
        explicit constexpr view_manager( // NOLINT(bugprone-forwarding-reference-overload)
          ET&&        et,
          stl::size_t cache_limit = default_cache_limit) noexcept
          : etraits{et},
            cached_views{et, cache_limit},
            view_roots{get_alloc_for<view_roots_type>(*this)} {}


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

            stl::error_code ec;

            // an absolute path should
            if (request.starts_with('/')) {
                path_type const file{request};
                if (!fs::is_regular_file(file, ec)) {
                    if (ec) {
                        this->logger.error(logging_category,
                                           fmt::format("Cannot check file details for {}", file.string()),
                                           ec);
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
                fs::file_status status = fs::status(dir, ec);
                if (ec && !fs::status_known(status)) {
                    this->logger.error(logging_category,
                                       fmt::format("Cannot check directory status of {}", dir.string()),
                                       ec);
                    continue;
                }

                // make sure the path is a directory
                if (!fs::is_directory(status)) {
                    continue;
                }

                if (recursive_search) {
                    fs::recursive_directory_iterator const iter(dir, ec);
                    if (ec) {
                        this->logger.error(logging_category,
                                           fmt::format("Cannot read dir {}", dir.string()),
                                           ec);
                        continue;
                    }
                    fs::recursive_directory_iterator       it     = fs::begin(iter);
                    fs::recursive_directory_iterator const it_end = fs::end(iter);
                    for (; it != it_end; it.increment(ec)) {
                        if (ec) {
                            this->logger.error(logging_category,
                                               fmt::format("Cannot traverse directory {}", dir.string()),
                                               ec);
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
                        status = fs::status(file, ec);
                        if (fs::is_regular_file(status)) {
                            return file;
                        } else if (ec && !fs::status_known(status)) {
                            this->logger.error(logging_category,
                                               fmt::format("Cannot check file type of {}", dir.string()),
                                               ec);
                        }
                    }
                } else { // non-recursive normal path appending
                    dir.append(request.begin(), request.end());
                    status = fs::status(dir, ec);
                    if (fs::exists(status)) {
                        return dir;
                    } else if (ec && !fs::status_known(status)) {
                        this->logger.error(logging_category,
                                           fmt::format("Cannot check file type of {}", dir.string()),
                                           ec);
                        // no need to "continue"
                    }


                    // The user may have omitted the extension, we are searching for them
                    for (string_view_type ext : valid_extensions) {
                        auto file = dir;
                        file.concat(ext.begin(), ext.end()); // append the extension

                        status = fs::status(file, ec);
                        if (fs::exists(status)) {
                            return file;
                        }
                        if (ec && !fs::status_known(status)) {
                            this->logger.error(logging_category,
                                               fmt::format("Cannot check file type of {}", dir.string()),
                                               ec);
                        }
                    }
                }
            }

            return stl::nullopt;
        }

        /**
         * Read the file content
         */
        bool read_file(stl::filesystem::path const& filepath, istl::String auto& out) const {
            bool const res = file::get_to(filepath, out);
            if (!res) {
                this->logger.error("Response/File",
                                   fmt::format("Cannot load the specified file: {}", filepath.string()));
                // return empty string
            }
            return res;
        }

        template <typename VT>
        [[nodiscard]] auto* get_view(path_type const& file) {
            static view_types default_view{stl::in_place_type<VT>, *this};
            return cached_views.emplace_get_ptr(file, default_view);
        }

        template <typename ViewType, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, path_type const& file, DataType&&... data) {
            using view_type = ViewType;
            auto* cached    = get_view<view_type>(file);
            auto& view      = stl::get<view_type>(*cached);
            if (!view.has_scheme()) {
                auto file_content = object::make_object<string_type>(*this);
                if (!read_file(file, file_content)) {
                    return; // We weren't able to read the file.
                }
                view.scheme(file_content);
                // since we got a pointer, we don't need to save it the cache again
            }

            // Render the view based on the data that passed to us
            view.render(out, stl::forward<DataType>(data)...);
        }

        template <typename ViewType, istl::StringViewifiable StrT, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, StrT&& file_request, DataType&&... data) {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            if (!file) {
                this->logger.error(logging_category,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return;
            }
            view_to<ViewType>(out, file.value(), stl::forward<DataType>(data)...);
        }


      public:
        /**
         * This is essentially the same as ".view" but it's specialized for a mustache file.
         */
        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr auto mustache(StrT&& file_request, mustache_data_type const& data) {
            auto out = object::make_object<string_type>(*this);
            view_to<mustache_view_type>(out, stl::forward<StrT>(file_request), data);
            return out;
        }

        template <istl::StringViewifiable StrT, typename... StrT2, typename... DataType>
        [[nodiscard]] constexpr auto mustache(StrT&& file_request, stl::pair<StrT2, DataType>... data) {
            return mustache<StrT>(stl::forward<StrT>(file_request),
                                  view::data_view_caster<mustache_data_type>(*this, stl::move(data)...));
        }

        template <istl::StringViewifiable StrT, typename... DataType>
            requires(!(sizeof...(DataType) == 1 &&
                       (stl::same_as<stl::remove_cvref_t<DataType>, mustache_data_type> && ...)))
        [[nodiscard]] constexpr auto mustache(StrT&& file_request, DataType&&... data) {
            return mustache<StrT>(
              stl::forward<StrT>(file_request),
              view::data_view_caster<mustache_data_type>(*this, stl::forward<DataType>(data)...));
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr string_type file(StrT&& file_request) {
            string_type out{get_alloc_for<string_type>(*this)};
            view_to<file_view_type>(out, stl::forward<StrT>(file_request));
            return out;
        }

        template <istl::StringViewifiable StrT = string_view_type>
        [[nodiscard]] auto view(StrT&& file_request) {
            return view(stl::forward<StrT>(file_request), istl::nothing_type{});
        }

        /**
         * Render a view
         */
        template <istl::StringViewifiable StrT, typename DT>
            requires(PossibleDataTypes<mustache_view_type, stl::remove_cvref_t<DT>> ||
                     PossibleDataTypes<file_view_type, stl::remove_cvref_t<DT>>)
        [[nodiscard]] auto view(StrT&& file_request, DT&& data) {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out  = object::make_object<string_type>(*this);
            if (!file) {
                this->logger.error(logging_category,
                                   fmt::format("We can't find the specified view {}.", file_request));
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
