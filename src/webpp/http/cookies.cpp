#include "cookies.h"
#include <boost/algorithm/string/trim.hpp>
#include <sstream>

webpp::cookie& webpp::cookie::name(std::string __name) noexcept {
    boost::algorithm::trim(__name);
    _name = std::move(__name);
    return *this;
}

webpp::cookie& webpp::cookie::value(std::string __value) noexcept {
    boost::algorithm::trim(__value);
    _value = std::move(__value);
    return *this;
}

std::ostream& webpp::cookie::operator<<(std::ostream& out) const noexcept {
    if (_prefix) {
        if (_secure)
            out << "__Secure-";
        else if (_host_only)
            out << "__Host-";
    }
    if (!_name.empty()) {

        // FIXME: encode/... name and value here. Programmers are dumb!
        out << _name << "=" << _value;

        if (!_comment.empty())
            out << "; Comment=" << _comment;

        if (!_domain.empty())
            out << "; Domain=" << _domain;

        if (!_path.empty())
            out << "; Path=" << _path;

        if (_expires == std::chrono::system_clock::now()) {
            // FIXME: you know what's wrong with it. just fix it
            out << "; Expires=" << _expires;
        }

        if (_secure)
            out << "; Secure";

        if (_host_only)
            out << "; HttpOnly";

        if (_max_age)
            out << "; Max-Age=" << _max_age;

        if (_same_site != same_site_value::NONE)
            out << "; SameSite="
                << (_same_site == same_site_value::STRICT ? "Strict" : "Lax");

        // TODO: encode value and check the key here:
        if (!attrs.empty())
            for (auto const& attr : attrs)
                out << "; " << attr.first << "=" << attr.second;
    }
    return out;
}

std::string webpp::cookie::render() const noexcept {
    std::ostringstream os;
    this->operator<<(os);
    return os.str();
}
