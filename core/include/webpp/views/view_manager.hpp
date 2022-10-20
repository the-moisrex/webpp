// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_MANAGER_HPP
#define WEBPP_VIEW_MANAGER_HPP

#include "../http/http_concepts.hpp"
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
#include <type_traits>
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
        using view_roots_type  = stl::vector<path_type, traits::general_allocator<traits_type, path_type>>;
        using char_type        = traits::char_type<traits_type>;
        using ifstream_type    = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;


        using mustache_view_type = mustache_view<traits_type>;
        using json_view_type     = json_view<traits_type>;
        using file_view_type     = file_view<traits_type>;

      private:
        // fixme: variant is not the best strategy, a file doesn't require as much storage as a mustache view
        using view_types = stl::variant<mustache_view_type, /*json_view_type,*/ file_view_type>;


        /**
         * This is what gets stored as a value
         */
        struct cache_value_type {
            view_types  view;         // the view, the view should not own the data
            string_type file_content; // content file; the view is a span/view/string_view... of the data
        };

        using mustache_data_type = typename mustache_view_type::data_type;
        // using json_data_type = typename json_view_type::data_type;
        using file_data_type = typename file_view_type::data_type;


        using cache_type = lru_cache<traits_type, path_type, cache_value_type, memory_gate<null_gate>>;

        static constexpr auto VIEW_CAT = "View";

        static constexpr stl::array<string_view_type, 1> valid_extensions{".mustache"};



        cache_type cached_views;


      public:
        view_roots_type view_roots; // the root directories where we can find the views


        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, view_manager>)
        constexpr view_manager( // NOLINT(bugprone-forwarding-reference-overload)
          ET&&        et,
          stl::size_t cache_limit = 100) noexcept
          : etraits{et},
            cached_views{et, cache_limit},
            view_roots{alloc::allocator_for<view_roots_type>(*this)} {}


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
                if (ec && !fs::status_known(status)) {
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
                    fs::recursive_directory_iterator const iter(dir, ec);
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
                        const path_type file      = *it;
                        const auto      file_stem = file.stem();
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
                            this->logger.error(VIEW_CAT,
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
                        this->logger.error(VIEW_CAT,
                                           fmt::format("Cannot check file type of {}", dir.string()),
                                           ec);
                    }


                    // The user may have omitted the extension, we are searching for them
                    for (string_view_type ext : valid_extensions) {
                        auto file = dir;
                        file.concat(ext.begin(), ext.end()); // append the extension

                        status = fs::status(file, ec);
                        if (fs::exists(status)) {
                            return file;
                        } else if (ec && !fs::status_known(status)) {
                            this->logger.error(VIEW_CAT,
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
        [[nodiscard]] string_type read_file(stl::filesystem::path const& file) const {
#ifdef WEBPP_EMBEDDED_FILES
            if (auto content = ::get_static_file(filepath); !content.empty()) {
                return string_type{this->content, alloc};
            }
#endif
            auto result = object::make_general<string_type>(*this);
            if (auto in = ifstream_type(file.c_str(), stl::ios::binary | stl::ios::ate); in.is_open()) {
                // details on this matter:
                // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
                // stl::unique_ptr<char[]> buffer{new char[buffer_size]};
                // stl::unique_ptr<char_type[]> result(static_cast<char_type*>(
                //  this->alloc_pack.template local_allocator<char_type[]>().allocate(size)));
                in.seekg(0, in.end);
                const auto size = in.tellg();
                // todo: don't need to zero it out; https://stackoverflow.com/a/29348072
                result.resize(static_cast<stl::size_t>(size));
                in.seekg(0);
                in.read(result.data(), size);
            } else {
                this->logger.error("Response/File",
                                   fmt::format("Cannot load the specified file: {}", file.string()));
                // return empty string
            }
            return result;
        }


        template <typename VT>
        [[nodiscard]] auto get_view(path_type const& file) noexcept {
            if (auto val = cached_views[file]; val) {
                return val;
            }
            cached_views.set(
              file,
              cache_value_type{.view         = view_types{stl::in_place_type<VT>, *this},
                               .file_content = string_type{alloc::general_alloc_for<string_type>(*this)}});
            return cached_views[file];
        }


        template <typename ViewType, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, path_type const& file, DataType&&... data) noexcept {
            using view_type = ViewType;
            auto cached     = get_view<view_type>(file);
            if (cached->file_content.empty()) {

                // get and set the code:
                cached->file_content = read_file(file);
                if (cached->file_content.empty()) {
                    return; // empty string is returned.
                }
                stl::get<view_type>(cached->view).scheme(cached->file_content);
                cached.save();
            }

            // at this point we don't care about the extension of the file; the user explicitly wants us to
            // parse it as a mustache file
            stl::get<view_type>(cached->view).render(out, stl::forward<DataType>(data)...);
        }


        template <typename ViewType, istl::StringViewifiable StrT, typename OutT, typename... DataType>
        constexpr void view_to(OutT& out, StrT&& file_request, DataType&&... data) noexcept {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            if (!file) {
                this->logger.error(VIEW_CAT,
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
        [[nodiscard]] constexpr auto mustache(StrT&& file_request, mustache_data_type const& data) noexcept {
            auto out = object::make_general<string_type>(this->alloc_pack);
            view_to<mustache_view_type>(out, stl::forward<StrT>(file_request), data);
            return out;
        }


        template <istl::StringViewifiable StrT, typename... StrT2, typename... DataType>
        [[nodiscard]] constexpr auto mustache(StrT&& file_request,
                                              stl::pair<StrT2, DataType>... data) noexcept {
            auto m_data = object::make_general<mustache_data_type>(*this);
            (m_data.emplace_back(*this, stl::move(data)), ...);
            return mustache<StrT>(stl::forward<StrT>(file_request), m_data);
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr string_view_type file(StrT&& file_request) noexcept {
            string_view_type out;
            view_to<file_view_type>(out, stl::forward<StrT>(file_request));
            return out;
        }

        template <istl::StringViewifiable StrT = string_view_type>
        [[nodiscard]] auto view(StrT&& file_request) noexcept {
            return view(stl::forward<StrT>(file_request), istl::nothing_type{});
        }

        /**
         * Get Render a view
         */
        template <istl::StringViewifiable StrT, typename DT>
            requires(PossibleDataTypes<mustache_view_type, stl::remove_cvref_t<DT>> ||
                     PossibleDataTypes<file_view_type, stl::remove_cvref_t<DT>>)
        [[nodiscard]] auto view(StrT&& file_request, DT&& data) noexcept {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out  = object::make_general<string_type>(this->alloc_pack);
            if (!file) {
                this->logger.error(VIEW_CAT,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            const auto ext = file->extension().string();
            if (ext.size() >= 1) {
                switch (ext[1]) {
                    case 'm': {
                        if (ext == ".mustache") {
                            view_to<mustache_view_type>(out, file.value(), data);
                        }
                        break;
                    }
                    case 'j': {
                        if (ext == ".json") {
                            // auto view = get_view<json_view_type>(file).view;
                            // view.scheme(file_content);
                            // view.render(out, data);
                        }
                    }
                    default: {
                        goto file_view;
                    }
                }
                return out;
            }
        file_view:
            view_to<file_view_type>(out, file.value());
            // cached_views.set(*file, view_types{stl::in_place_type<file_view_type>, stl::move(view)});

            return out;
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
