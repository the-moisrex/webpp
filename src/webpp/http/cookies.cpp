#include "cookies.h"
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/container_hash/hash.hpp>
#include <functional>
#include <sstream>

webpp::cookie::cookie(const webpp::cookie& c) noexcept
    : attrs{c.attrs}, _name{c._name}, _value{c._value}, _domain{c._domain},
      _path{c._path},
      _expires(c._expires ? std::make_unique<date_t>(*c._expires) : nullptr),
      _max_age{c._max_age}, _same_site{c._same_site}, _secure{c._secure},
      _host_only{c._host_only},
      _encrypted{c._encrypted}, _prefix{c._prefix}, _comment{c._comment} {}

webpp::cookie::cookie(webpp::cookie&& c) noexcept
    : attrs{std::move(c.attrs)}, _name{std::move(c._name)}, _value{std::move(
                                                                c._value)},
      _domain{std::move(c._domain)}, _path{std::move(c._path)},
      _expires(std::move(c._expires)), _max_age{std::move(c._max_age)},
      _same_site{std::move(c._same_site)}, _secure{std::move(c._secure)},
      _host_only{std::move(c._host_only)}, _encrypted{std::move(c._encrypted)},
      _prefix{std::move(c._prefix)}, _comment{std::move(c._comment)} {}

webpp::cookie::cookie(std::string __name, std::string __value) noexcept {
    name(std::move(__name));
    value(std::move(__value));
}

webpp::cookie& webpp::cookie::operator=(const webpp::cookie& c) noexcept {
    attrs = c.attrs;
    _name = c._name;
    _value = c._value;
    _domain = c._domain;
    _path = c._path;
    _expires.reset(new date_t{*c._expires});
    _max_age = c._max_age;
    _same_site = c._same_site;
    _secure = c._secure;
    _host_only = c._host_only;
    _encrypted = c._encrypted;
    _prefix = c._prefix;
    _comment = c._comment;
    return *this;
}

webpp::cookie& webpp::cookie::operator=(webpp::cookie&& c) noexcept {
    attrs = std::move(c.attrs);
    _name = std::move(c._name);
    _value = std::move(c._value);
    _domain = std::move(c._domain);
    _path = std::move(c._path);
    _expires = std::move(c._expires);
    _max_age = std::move(c._max_age);
    _same_site = std::move(c._same_site);
    _secure = std::move(c._secure);
    _host_only = std::move(c._host_only);
    _encrypted = std::move(c._encrypted);
    _prefix = std::move(c._prefix);
    _comment = std::move(c._comment);
    return *this;
}

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

decltype(webpp::cookie::_value) webpp::cookie::encrypted_value() const
    noexcept {}

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

        if (_expires) {
            std::time_t expires_c = system_clock::to_time_t(*_expires);
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

bool webpp::cookie::operator==(const webpp::cookie& c) const noexcept {
    return _name == c._name && _value == c._value && _prefix == c._prefix &&
           _encrypted == c._encrypted && _secure == c._secure &&
           _host_only == c._host_only && _same_site == c._same_site &&
           _comment == c._comment && _expires == c._expires &&
           _path == c._path && _domain == c._domain && attrs == c.attrs;
}

bool webpp::cookie::operator<(const webpp::cookie& c) const noexcept {
    return _expires < c._expires;
}

bool webpp::cookie::operator>(const webpp::cookie& c) const noexcept {
    return _expires > c._expires;
}

bool webpp::cookie::operator<=(const webpp::cookie& c) const noexcept {
    return _expires <= c._expires;
}

bool webpp::cookie::operator>=(const webpp::cookie& c) const noexcept {
    return _expires >= c._expires;
}

std::string webpp::cookie::render() const noexcept {
    std::ostringstream os;
    this->operator<<(os);
    return os.str();
}

webpp::cookie_hash::result_type webpp::cookie_hash::
operator()(const webpp::cookie_hash::argument_type& c) const noexcept {
    webpp::cookie_hash::result_type seed = 0;
    boost::hash_combine(seed, c._name);
    boost::hash_combine(seed, c._domain);
    boost::hash_combine(seed, c._path);
    //    boost::hash_combine(seed, c._value);
    //    boost::hash_combine(seed, c._prefix);
    //    boost::hash_combine(seed, c._secure);
    //    if (c._expires)
    //        boost::hash_combine(seed, c._expires->time_since_epoch().count());
    //    boost::hash_combine(seed, c._max_age);
    //    boost::hash_combine(seed, c._same_site);
    //    boost::hash_combine(seed, c._comment);
    //    boost::hash_combine(seed, c._host_only);
    //    boost::hash_combine(seed, c._encrypted);
    return seed;
}

bool webpp::cookie_equals::operator()(const webpp::cookie& lhs,
                                      const webpp::cookie& rhs) const noexcept {
    return lhs.name() == rhs.name() && lhs.domain() == rhs.domain() &&
           lhs.path() == rhs.path();
}

webpp::cookies::const_iterator
webpp::cookies::find(std::string const& name) const noexcept {
    return std::find_if(this->cbegin(), this->cend(),
                        [&](auto const& a) { return a.name() == name; });
}
