#include "../../../include/webpp/http/header.h"

#include <sstream>
using namespace webpp;



std::string headers::str() const noexcept {
    std::stringstream res;
    // TODO: add support for other HTTP versions
    // res << "HTTP/1.1" << " " << status_code() << " " <<
    // status_reason_phrase(status_code()) << "\r\n";
    for (auto const& [attr, val] : *this) {
        res << attr << ": ";
        res << val; // TODO: make sure it's secure
        res << "\r\n";
    }
    return res.str();
}

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
