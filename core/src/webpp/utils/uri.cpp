
#include <webpp/valves/uri.h>

#include "../../../include/webpp/utils/uri.h"

[[nodiscard]] bool webpp::is::scheme(std::string_view const& _scheme) noexcept {
    return ALPHA.contains(_scheme);
}

bool webpp::operator==(uri const& one, const_uri const& two) noexcept {
    return one.operator==(two.str());
}

bool webpp::operator==(const_uri const& one, uri const& two) noexcept {
    return one.operator==(two.str());
}
