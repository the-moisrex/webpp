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
        using view_types         = stl::variant<mustache_view_type, /*json_view_type,*/ file_view_type>;

        using mustache_data_type = typename mustache_view_type::data_type;
        // using json_data_type = typename json_view_type::data_type;
        using file_data_type = typename file_view_type::data_type;


        using cache_type = lru_cache<traits_type, path_type, view_types, memory_gate<null_gate>>;

        static constexpr auto VIEW_CAT = "View";

        static constexpr stl::array<string_view_type, 1> valid_extensions{".mustache"};


        view_roots_type view_roots; // the root directories where we can find the views

      private:
        cache_type cached_views;


      public:
        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, view_manager>)
        constexpr view_manager( // NOLINT(bugprone-forwarding-reference-overload)
          ET&&        et,
          stl::size_t cache_limit = 100) noexcept
          : etraits{et},
            view_roots{alloc::allocator_for<view_roots_type>(*this)},
            cached_views{et, cache_limit} {}


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
        [[nodiscard]] stl::optional<string_type> read_file(stl::filesystem::path const& file) const {
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
                return stl::nullopt;
            }
        }


        template <typename VT>
        [[nodiscard]] VT get_view(path_type const& file) noexcept {
            return stl::get<VT>(cached_views.emplace_get(file, VT{*this}));
        }


        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr auto file(StrT&& file_request) noexcept {
            auto const the_file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out      = object::make_general<string_type>(this->alloc_pack);
            if (!the_file) {
                this->logger.error(VIEW_CAT,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            auto file_content = read_file(the_file.value());
            if (!file_content) {
                return out; // empty string is returned.
            }

            // at this point we don't care about the extension of the file; the user explicitly wants us to
            // parse it as a mustache file
            file_view_type view = get_view<file_view_type>(*the_file);
            view.scheme(stl::move(file_content.value()));
            view.render(out);
            cached_views.set(*the_file, view_types{stl::in_place_type<file_view_type>, stl::move(view)});
            return out;
        }


        /**
         * This is essentially the same as ".view" but it's specialized for a mustache file.
         */
        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr auto mustache(StrT&& file_request, mustache_data_type const& data) noexcept {
            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            auto       out  = object::make_general<string_type>(this->alloc_pack);
            if (!file) {
                this->logger.error(VIEW_CAT,
                                   fmt::format("We can't find the specified view {}.", file_request));
                return out;
            }
            auto file_content = read_file(file.value());
            if (!file_content) {
                return out; // empty string is returned.
            }

            // at this point we don't care about the extension of the file; the user explicitly wants us to
            // parse it as a mustache file
            mustache_view_type view = get_view<mustache_view_type>(*file);
            view.scheme(stl::move(file_content.value()));
            view.render(out, data);
            cached_views.set(*file, view_types{stl::in_place_type<mustache_view_type>, stl::move(view)});
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
            auto file_content = read_file(file.value());
            if (!file_content) {
                return out; // empty string is returned.
            }
            const auto ext = file->extension().string();
            if (ext.size() >= 1) {
                switch (ext[1]) {
                    case 'm': {
                        if (ext == ".mustache") {
                            mustache_view_type view = get_view<mustache_view_type>(*file);
                            view.scheme(file_content.value());
                            view.render(out, data);
                            cached_views.set(
                              *file,
                              view_types{stl::in_place_type<mustache_view_type>, stl::move(view)});
                        }
                        break;
                    }
                    case 'j': {
                        if (ext == ".json") {
                            // json_view_type& view = get_view<json_view_type>(file);
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
            file_view_type view = get_view<file_view_type>(*file);
            view.scheme(stl::move(file_content.value()));
            view.render(out);
            cached_views.set(*file, view_types{stl::in_place_type<file_view_type>, stl::move(view)});

            return out;
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
