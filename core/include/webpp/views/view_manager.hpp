// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_MANAGER_HPP
#define WEBPP_VIEW_MANAGER_HPP

#include "../http/response_concepts.hpp"
#include "../std/format.hpp"
#include "../std/map.hpp"
#include "../std/string.hpp"
#include "../traits/traits.hpp"

#include <filesystem>
#include <system_error>

namespace webpp::views {


    /**
     * View
     *
     */
    template <Traits TraitsType, typename CtxT>
    struct view_manager {
        using traits_type      = TraitsType;
        using context_type     = CtxT;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using name_type        = string_type;
        using value_type       = string_type; // convert to std::variant or something
        using arguments_type   = traits::generalify_allocators<traits_type, stl::map<name_type, value_type>>;
        using path_type        = stl::filesystem::path;
        using view_roots_type  = traits::generalify_allocators<traits_type, stl::vector<path_type>>;

        static constexpr auto VIEW_CAT = "View";


        view_roots_type view_roots; // the root directories where we can find the views

      private:
        context_type&  ctx;
        arguments_type args;
        string_type    file_content;

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
                        ctx.logger.error(VIEW_CAT,
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
                    ctx.logger.error(VIEW_CAT,
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
                        ctx.logger.error(VIEW_CAT, fmt::format("Cannot read dir {}", dir.string()), ec);
                    }
                    fs::recursive_directory_iterator       it     = fs::begin(iter);
                    const fs::recursive_directory_iterator it_end = fs::end(iter);
                    for (; it != it_end; it.increment(ec)) {
                        if (ec) {
                            ctx.logger.error(VIEW_CAT,
                                             fmt::format("Cannot traverse directory {}", dir.string()),
                                             ec);
                        }
                        const path_type file = *it;
                        if (file.stem() != request) {
                            continue;
                        }

                        status = fs::status(file, ec);
                        if (ec) {
                            ctx.logger.error(VIEW_CAT,
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
                        ctx.logger.error(VIEW_CAT,
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

      public:
        constexpr view_manager(context_type& input_ctx) noexcept : ctx{input_ctx} {}

        auto mustache() const {}

        string_type read_file(stl::filesystem::path const& file) const {
            return "";
        }

        template <istl::StringViewifiable StrT>
        constexpr http::HTTPResponse auto render(StrT&& file_request) const noexcept {
            namespace fs = stl::filesystem;

            auto const file = find_file(istl::to_std_string_view(stl::forward<StrT>(file_request)));
            if (!file) {
                return ctx.error(404);
            }

            // todo: read the file.
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
