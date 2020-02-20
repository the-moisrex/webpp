#include "webpp/router.h"
#include "webpp/utils/uri.h"

std::map<std::string_view, std::string_view>
webpp::parse_vars(std::string_view const& _templ,
           std::string_view const& _path) noexcept {

    using namespace webpp;

    std::map<std::string_view, std::string_view> res;
    auto parsed_path = const_uri{_path}.path_structured_decoded();
    auto parsed_templ = const_uri{_templ}.path_structured_decoded();
    auto it = parsed_path.cbegin();
    for (auto const& templ_segment : parsed_templ) {

        if (templ_segment != *it) {

            if (templ_segment.starts_with('{') &&
                templ_segment.ends_with('}')) {

                // remove the curly-braces
                auto var = templ_segment.substr(1, templ_segment.size() - 3);
                res[var] = *it;
            }

            break; // it's faulty now
        }

        it++;
        if (it == parsed_path.cend())
            break;
    }

    return res;
}
