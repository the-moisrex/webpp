#include "webpp/router.h"
#include "webpp/utils/uri.h"

/**
 * TODO: add types to the "{user_id}" to be able to use it as "{int:user_id}"
 */
std::map<std::string_view, std::string_view>
webpp::parse_vars(std::string_view const& _templ,
                  std::string_view const& _path) noexcept {

    using namespace webpp;

    std::map<std::string_view, std::string_view> res;

    using ptr_t = decltype(_templ.data());

    ptr_t templ_start = _templ.data();
    ptr_t templ_end = _templ.data() + _templ.size();
    ptr_t curly_start = templ_end;
    ptr_t colon = templ_end;
    ptr_t path_char = _path.data();

    for (ptr_t c = templ_start; *c; c++) {

        // breaks out of the loop when the template and the paths don't match
        if (curly_start == templ_end && *c != '{' && *c != *path_char++)
            break;

        switch (*c) {
        case '{':
            if (curly_start == templ_end) {
                colon = curly_start = c;
            }
            break;
        case ':':
            if (curly_start != templ_end) {
                colon = c;
            }
            break;
        case '}':
            // check if we are in a curly braces
            if (curly_start != templ_end) {
                std::string_view key{
                    colon + 1,
                    static_cast<std::string_view::size_type>(c - colon - 1)};

                // find _ in "{var}_" inside path
                // this doesn't take the / in account
                auto next_char_path =
                    std::string_view{path_char}.find(*(c + 1));

                std::string_view value{
                    path_char,
                    std::min(next_char_path,
                             _path.size() - (path_char - _path.data()))};
                /*
                std::string_view type{curly_start + 1,
                                      static_cast<std::string_view::size_type>(
                                          colon - curly_start)};
                                          */
                res[key] = value;
                curly_start = colon = templ_end; // reset
            }
            break;
        }
    }

    return res;
}
