#include "../../../include/webpp/http/cookies.h"

#include "../../../include/webpp/utils/strings.h"


/*
basic_cookie::basic_cookie(const basic_cookie& c) noexcept
    : attrs{c.attrs}, _name{c._name}, _value{c._value}, _domain{c._domain},
      _path{c._path},
      _expires(c._expires ? std::make_unique<date_t>(*c._expires) : nullptr),
      _max_age{c._max_age}, _same_site{c._same_site}, _secure{c._secure},
      _host_only{c._host_only},
      _encrypted{c._encrypted}, _prefix{c._prefix}, _comment{c._comment} {}

basic_cookie::basic_cookie(basic_cookie&& c) noexcept
    : attrs{std::move(c.attrs)}, _name{std::move(c._name)}, _value{std::move(
                                                                c._value)},
      _domain{std::move(c._domain)}, _path{std::move(c._path)},
      _expires(std::move(c._expires)), _max_age{c._max_age},
      _same_site{c._same_site}, _secure{c._secure}, _host_only{c._host_only},
      _encrypted{c._encrypted}, _prefix{c._prefix}, _comment{std::move(
                                                        c._comment)} {}

basic_cookie::basic_cookie(basic_cookie::name_t __name, basic_cookie::value_t
__value) noexcept {
    // these two will trim the strings
    name(std::move(__name));
    value(std::move(__value));
}

basic_cookie& basic_cookie::operator=(const basic_cookie& c) noexcept {
    attrs = c.attrs;
    _name = c._name;
    _value = c._value;
    _domain = c._domain;
    _path = c._path;
    _expires = std::make_it_unique<date_t>(*c._expires);
    _max_age = c._max_age;
    _same_site = c._same_site;
    _secure = c._secure;
    _host_only = c._host_only;
    _encrypted = c._encrypted;
    _prefix = c._prefix;
    _comment = c._comment;
    return *this;
}

basic_cookie& basic_cookie::operator=(basic_cookie&& c) noexcept {
    attrs = std::move(c.attrs);
    _name = std::move(c._name);
    _value = std::move(c._value);
    _domain = std::move(c._domain);
    _path = std::move(c._path);
    _expires = std::move(c._expires);
    _max_age = c._max_age;
    _same_site = c._same_site;
    _secure = c._secure;
    _host_only = c._host_only;
    _encrypted = c._encrypted;
    _prefix = c._prefix;
    _comment = std::move(c._comment);
    return *this;
}
*/
