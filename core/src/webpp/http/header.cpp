#include "../../../include/webpp/http/header.h"

using namespace webpp;

void headers::remove_cookies() noexcept {
    _cookies.clear();
    for (auto it = begin(); it != end();) {
        if (to_lower_copy(it->first) == "set-cookie")
            it = erase(it);
        else
            ++it;
    }
}

void headers::reload_cookies() const noexcept {
    _cookies.clear();
    for (auto const& [attr, value] : *this) {
        if ("set-cookie" == attr) {
            _cookies.emplace(value);
        }
    }
}
