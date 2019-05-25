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
    using namespace std::chrono;
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
            std::time_t expires_c = system_clock::to_time_t(_expires);
            std::tm expires_tm = *std::localtime(&expires_c);
            char buff[30];
            // FIXME: check time zone and see if it's ok
            //            setlocale(LC_ALL, "en_US.UTF-8");
            if (strftime(buff, sizeof buff, "%a, %d %b %Y %H:%M:%S GMT",
                         &expires_tm))
                out << "; Expires=" << buff;
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
