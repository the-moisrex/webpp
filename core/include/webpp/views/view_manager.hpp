// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_MANAGER_HPP
#define WEBPP_VIEW_MANAGER_HPP

#include "../http/response_concepts.hpp"
#include "../std/format.hpp"
#include "../std/map.hpp"
#include "../std/string.hpp"
#include "../storage/lru_cache.hpp"
#include "../storage/memory_gate.hpp"
#include "../storage/null_gate.hpp"
#include "../traits/traits.hpp"
#include "file_view.hpp"
#include "json_view.hpp"
#include "mustache_view.hpp"

#include <filesystem>
#include <fstream>
#include <system_error>
#include <variant>

#ifdef WEBPP_EMBEDDED_FILES
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    else
extern std::string_view get_static_file(std::string_view const&) noexcept;
#    endif
#endif

namespace webpp::views {


    /**
     * View Manager
     */
    template <Traits TraitsType>
    struct view_manager : enable_traits<TraitsType> {
        using etraits          = enable_traits<TraitsType>;
        using traits_type      = TraitsType;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using path_type        = stl::filesystem::path;
        using view_roots_type  = traits::generalify_allocators<traits_type, stl::vector<path_type>>;
        using char_type        = traits::char_type<traits_type>;
        using ifstream_type    = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;


        using mustache_view_type = mustache_view<traits_type>;
        using json_view_type     = json_view<traits_type>;
        using file_view_type     = file_view<traits_type>;
        using view_types         = stl::variant<mustache_view_type, json_view_type, file_view_type>;

        // using mustache_data_type = typename mustache_view_type::data_type;
        // using json_data_type = typename json_view_type::data_type;
        // using file_data_type = typename file_view_type::data_type;

        // using mustache_data_view_type = typename mustache_view_type::data_view_type;
        // using json_data_view_type = typename json_view_type::data_view_type;
        // using file_data_view_type = typename file_view_type::data_view_type;

        struct cached_view_type {
            path_type  file;
            view_types view;
        };

        using cache_type = lru_cache<traits_type, string_type, cached_view_type, memory_gate<null_gate>>;

        static constexpr auto VIEW_CAT = "View";


        view_roots_type view_roots; // the root directories where we can find the views

      private:
        cache_type cached_views;


      public:
        constexpr view_manager() noexcept = default;

        template <EnabledTraits ET>
        constexpr view_manager(ET const& et) noexcept : etraits{et} {}


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
                const path_type file{request};
                if (!fs::is_regular_file(file, ec)) {
                    if (ec) {
                        this->logger.error(VIEW_CAT,
                                           fmt::format("Cannot check file details for {}", file.string()),
                                           ec);
                    }
                    return stl::nullopt;
                }
                return file;
            }

            // a search for all request
            const bool recursive_search = request.starts_with('*');
            if (recursive_search) {
                request.remove_prefix(1);
            }

            // traverse the root directories
            for (path_type dir : view_roots) {

                fs::file_status status = fs::status(dir, ec);
                if (ec) {
                    this->logger.error(VIEW_CAT,
                                       fmt::format("Cannot check directory status of {}", dir.string()),
                                       ec);
                    continue;
                }

                // make sure the path is a directory
                if (!fs::is_directory(status)) {
                    continue;
                }

                if (recursive_search) {
                    fs::recursive_directory_iterator iter(dir, ec);
                    if (ec) {
                        this->logger.error(VIEW_CAT, fmt::format("Cannot read dir {}", dir.string()), ec);
                    }
                    fs::recursive_directory_iterator       it     = fs::begin(iter);
                    const fs::recursive_directory_iterator it_end = fs::end(iter);
                    for (; it != it_end; it.increment(ec)) {
                        if (ec) {
                            this->logger.error(VIEW_CAT,
                                               fmt::format("Cannot traverse directory {}", dir.string()),
                                               ec);
                        }
                        const path_type file = *it;
                        if (file.stem() != request) {
                            continue;
                        }

                        status = fs::status(file, ec);
                        if (ec) {
                            this->logger.error(VIEW_CAT,
                                               fmt::format("Cannot check file type of {}", dir.string()),
                                               ec);
                        }
                        if (fs::is_character_file(status)) {
                            return file;
                        }
                    }
                } else { // non-recursive normal path appending
                    dir.append(request.begin(), request.end());
                    status = fs::status(dir, ec);
                    if (ec) {
                        this->logger.error(VIEW_CAT,
                                           fmt::format("Cannot check file type of {}", dir.string()),
                                           ec);
                    }
                    if (fs::exists(status) && fs::is_character_file(status)) {
                        return dir;
                    }
                }
            }

            return stl::nullopt;
        }


        /**
         * Read the file content
         */
        string_type read_file(stl::filesystem::path const& file) const {
#ifdef WEBPP_EMBEDDED_FILES
            if (auto content = ::get_static_file(filepath); !content.empty()) {
                return string_type{this->content, alloc};
            }
#endif
            if (auto in = ifstream_type(file.c_str(), stl::ios::binary | stl::ios::ate); in.is_open()) {
                // details on this matter:
                // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                // stl::unique_ptr<char_type[]> result(static_cast<char_type*>(
                //  this->alloc_pack.template local_allocator<char_type[]>().allocate(size)));
                auto result = object::make_general<string_type>(*this);
                in.seekg(0, in.end);
                const auto size = in.tellg();
                // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                result.resize(static_cast<stl::size_t>(size));
                in.seekg(0);
                in.read(result.data(), size);
                return result;
            } else {
                this->logger.error("Response/File",
                                   fmt::format("Cannot load the specified file: {}", file.string()));
                return "";
            }
        }


        view_types& get_view(path_type const& file) noexcept {
            return cached_views.emplace_get(file, *this);
        }


        /**
         * This is essentially the same as ".view" but it's specialized for a mustache file.
         */
        template <istl::StringViewifiable StrT>
        constexpr auto mustache(StrT&& file_request, mustache_data_type data) noexcept {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out  = object::make_general<string_type>(this->alloc_pack);
            if (!file) {
                this->logger.error(VIEW_CAT,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            const auto file_content = read_file(file.value());

            // at this point we don't care about the extension of the file; the user explicitly wants us to
            // parse it as a mustache file
            mustache_view_type& view = stl::get<mustache_view_type>(get_view(file));
            view.scheme(file_content);
            view.render(data, out);
            return out;
        }


        /**
         * Get Render a view
         */
        template <istl::StringViewifiable StrT, PossibleDataViews DV = stl::tuple<>>
        auto view(StrT&& file_request, DV&& data = stl::tuple<>{}) noexcept {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out  = object::make_general<string_type>(this->alloc_pack);
            if (!file) {
                this->logger.error(VIEW_CAT,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            const auto file_content = read_file(file.value());
            const auto ext          = file->extension().string();
            if (ext.size() >= 1) {
                switch (ext[1]) {
                    case 'm': {
                        if (ext == ".mustache") {
                            mustache_view_type& view = stl::get<mustache_view_type>(get_view(*file));
                            view.scheme(file_content);
                            view.render(data, out);
                        }
                        break;
                    }
                    case 'j': {
                        if (ext == ".json") {
                            // json_view_type& view = stl::get<json_view_type>(get_view(file));
                            // view.scheme(file_content);
                            // view.render(data, out);
                        }
                    }
                    default: {
                        goto file_view;
                    }
                }
                return out;
            }
        file_view:
            file_view_type view;
            view.scheme(file_content);
            view.render(data, out);

            return out;
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
