
#include "../../../include/webpp/utils/uri.h"

#include <webpp/valves/uri.h>

bool webpp::operator==(uri const& one, const_uri const& two) noexcept {
    return one.operator==(two.str());
}

bool webpp::operator==(const_uri const& one, uri const& two) noexcept {
    return one.operator==(two.str());
}
