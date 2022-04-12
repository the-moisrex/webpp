// Created by moisrex on 11/04/22.

#ifndef WEBPP_VIEW_HPP
#define WEBPP_VIEW_HPP

#include "../std/map.hpp"
#include "../std/string.hpp"
#include "../traits/traits.hpp"

#include <filesystem>

namespace webpp::views {


    /**
     * View
     *
     * Todo:
     *   - [ ] Extension support
     */
    template <Traits TraitsType, typename CtxT>
    struct view {
        using traits_type    = TraitsType;
        using context_type   = CtxT;
        using string_type    = traits::general_string<traits_type>;
        using name_type      = string_type;
        using value_type     = string_type; // convert to std::variant or something
        using arguments_type = traits::generalify_allocators<traits_type, stl::map<name_type, value_type>>;

        static constexpr auto VIEW_CAT = "View";

      private:
        context_type&  ctx;
        arguments_type args;
        string_type    file_content;

      public:
        constexpr view(context_type& input_ctx) noexcept : ctx{input_ctx} {}

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

} // namespace webpp

#endif // WEBPP_VIEW_HPP
