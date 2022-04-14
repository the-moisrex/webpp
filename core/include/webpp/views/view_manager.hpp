// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_MANAGER_HPP
#define WEBPP_VIEW_MANAGER_HPP

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
         *   - [ ] *header
         */
        stl::optional<path_type> find_file(string_view_type request) const noexcept {
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
            for (path_type const& dir : view_roots) {

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
                    for (auto const& file : iter) {
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
                    dir /= request;
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
        auto render(StrT&& file) const noexcept {
            namespace fs = stl::filesystem;

            stl::error_code ec;
            fs::path        file_path{istl::to_std_string(file)};
            if (!fs::exists(file_path, ec)) {
                fs::path mustache_file = file_path;
                mustache_file += ".mustache";
                if (fs::exists(mustache_file, ec) && !ec) {
                    return this->mustache(read_file(mustache_file));
                } else if (ec) {
                    ctx.logger.error(VIEW_CAT, "Cannot read the specified view file.", ec);
                    return ctx.error(500); // todo
                }
            } else {
                auto const extension = file_path.extension();
                if (".mustache" == extension) {
                    return this->mustache(this->read_file(file_path));
                } else {
                    return ctx.error(500); // todo
                }
            }
        }
    };

} // namespace webpp::views

#endif // WEBPP_VIEW_MANAGER_HPP
