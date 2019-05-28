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

bool webpp::cookie::same_as(const webpp::cookie& c) const noexcept {
    return _name == c._name && _path == c._path && c._domain == _domain;
}

webpp::cookie_hash::result_type webpp::cookie_hash::
operator()(const webpp::cookie_hash::argument_type& c) const noexcept {
    // change the "same_as" method too if you ever touch this function
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
    return std::find_if(cbegin(), cend(),
                        [&](auto const& a) { return a.name() == name; });
}

webpp::cookies::const_iterator
webpp::cookies::find(const webpp::cookie& c) const noexcept {
    return std::find_if(cbegin(), cend(),
                        [&](auto const& a) { return a.same_as(c); });
}

void webpp::cookies::make_unique(
    const webpp::cookies::const_iterator& dont_touch,
    std::function<bool(cookie const&)> const& check) noexcept {
    for (auto it = begin(); it != end(); it++)
        if (check(*it) && dont_touch != it && dont_touch->same_as(*it)) {
            erase(it);
            break;
        }
}

void webpp::cookies::change_if(
    webpp::cookies::condition const& if_statement,
    std::function<void(iterator&)> const& change) noexcept {
    for (auto it = begin(); it != end(); it++)
        if (if_statement(*it))
            change(it);
}

void webpp::cookies::change_if(
    cookie::name_t const& _name,
    std::function<void(iterator&)> const& change) noexcept {
    for (auto it = begin(); it != end(); it++)
        if (it->_name == _name)
            change(it);
}

void webpp::cookies::change_all(
    std::function<void(iterator&)> const& change) noexcept {
    for (auto it = begin(); it != end(); it++)
        change(it);
}

std::pair<webpp::cookies::iterator, bool>
webpp::cookies::insert(const value_type& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(value);
}

std::pair<webpp::cookies::iterator, bool>
webpp::cookies::insert(value_type&& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(std::move(value));
}

webpp::cookies::iterator webpp::cookies::insert(const_iterator hint,
                                                const value_type& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(hint, value);
}

webpp::cookies::iterator webpp::cookies::insert(const_iterator hint,
                                                value_type&& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(hint, std::move(value));
}

void webpp::cookies::insert(std::initializer_list<value_type> ilist) {
    for (auto it = ilist.begin(); it != ilist.end(); it++) {
        auto found = find(*it);
        if (found != cend())
            erase(found);
    }
    return static_cast<super*>(this)->insert(ilist);
}

webpp::cookies&
webpp::cookies::encrypted(cookie::encrypted_t const& _encrypted) noexcept {
    for (auto& c : *this)
        c._encrypted = _encrypted;
    return *this;
}
webpp::cookies&
webpp::cookies::encrypted(condition const& _condition,
                          cookie::encrypted_t const& _encrypted) noexcept {
    change_if(_condition, [&](auto& it) { it->_encrypted = _encrypted; });
    return *this;
}
webpp::cookies&
webpp::cookies::encrypted(cookie::name_t const& _name,
                          cookie::encrypted_t const& _encrypted) noexcept {
    change_if(_name, [&](auto& it) { it->_encrypted = _encrypted; });
    return *this;
}
webpp::cookies&
webpp::cookies::encrypted(const_iterator const& it,
                          cookie::encrypted_t _encrypted) noexcept {
    it->_encrypted = std::move(_encrypted);
    return *this;
}

webpp::cookies&
webpp::cookies::secure(cookie::secure_t const& _secure) noexcept {
    for (auto& c : *this)
        c._secure = _secure;
    return *this;
}
webpp::cookies&
webpp::cookies::secure(condition const& _condition,
                       cookie::secure_t const& _secure) noexcept {
    change_if(_condition, [&](auto& it) { it->_secure = _secure; });
    return *this;
}
webpp::cookies&
webpp::cookies::secure(cookie::name_t const& _name,
                       cookie::secure_t const& _secure) noexcept {
    change_if(_name, [&](auto& it) { it->_secure = _secure; });
    return *this;
}
webpp::cookies& webpp::cookies::secure(const_iterator const& it,
                                       cookie::secure_t _secure) noexcept {
    it->_secure = std::move(_secure);
    return *this;
}

webpp::cookies&
webpp::cookies::host_only(cookie::host_only_t const& _host_only) noexcept {
    for (auto& c : *this)
        c._host_only = _host_only;
    return *this;
}

webpp::cookies&
webpp::cookies::host_only(condition const& _condition,
                          cookie::host_only_t const& _host_only) noexcept {
    change_if(_condition, [&](auto& it) { it->_host_only = _host_only; });
    return *this;
}

webpp::cookies&
webpp::cookies::host_only(cookie::name_t const& _name,
                          cookie::host_only_t const& _host_only) noexcept {
    change_if(_name, [&](auto& it) { it->_host_only = _host_only; });
    return *this;
}

webpp::cookies&
webpp::cookies::host_only(const_iterator const& it,
                          cookie::host_only_t _host_only) noexcept {
    it->_host_only = std::move(_host_only);
    return *this;
}

webpp::cookies&
webpp::cookies::prefix(cookie::prefix_t const& _prefix) noexcept {
    for (auto& c : *this)
        c._prefix = _prefix;
    return *this;
}

webpp::cookies&
webpp::cookies::prefix(cookie::name_t const& _name,
                       cookie::prefix_t const& _prefix) noexcept {
    change_if(_name, [&](auto& it) { it->_prefix = _prefix; });
    return *this;
}

webpp::cookies&
webpp::cookies::prefix(condition const& _condition,
                       cookie::prefix_t const& _prefix) noexcept {
    change_if(_condition, [&](auto& it) { it->_prefix = _prefix; });
    return *this;
}

webpp::cookies& webpp::cookies::prefix(const_iterator const& it,
                                       cookie::prefix_t _prefix) noexcept {
    it->_prefix = std::move(_prefix);
    return *this;
}

webpp::cookies&
webpp::cookies::comment(cookie::comment_t const& _comment) noexcept {
    for (auto& c : *this)
        c._comment = _comment;
    return *this;
}
webpp::cookies&
webpp::cookies::comment(condition const& _condition,
                        cookie::comment_t const& _comment) noexcept {
    change_if(_condition, [&](auto& it) { it->_comment = _comment; });
    return *this;
}
webpp::cookies&
webpp::cookies::comment(cookie::name_t const& _name,
                        cookie::comment_t const& _comment) noexcept {
    change_if(_name, [&](auto& it) { it->_comment = _comment; });
    return *this;
}
webpp::cookies& webpp::cookies::comment(const_iterator const& it,
                                        cookie::comment_t&& _comment) noexcept {
    it->_comment = std::move(_comment);
    return *this;
}
webpp::cookies&
webpp::cookies::comment(const_iterator const& it,
                        cookie::comment_t const& _comment) noexcept {
    return comment(it, cookie::comment_t{_comment});
}

webpp::cookies&
webpp::cookies::same_site(cookie::same_site_t const& _same_site) noexcept {
    for (auto& c : *this)
        c._same_site = _same_site;
    return *this;
}
webpp::cookies&
webpp::cookies::same_site(cookie::name_t const& _name,
                          cookie::same_site_t const& _same_site) noexcept {
    change_if(_name, [&](auto& it) { it->_same_site = _same_site; });
    return *this;
}
webpp::cookies&
webpp::cookies::same_site(condition const& _condition,
                          cookie::same_site_t const& _same_site) noexcept {
    change_if(_condition, [&](auto& it) { it->_same_site = _same_site; });
    return *this;
}
webpp::cookies&
webpp::cookies::same_site(const_iterator const& it,
                          cookie::same_site_t _same_site) noexcept {
    it->_same_site = std::move(_same_site);
    return *this;
}

webpp::cookies&
webpp::cookies::expires(cookie::date_t const& _expires) noexcept {
    for (auto& c : *this)
        c._expires.reset(new cookie::date_t{_expires});
    return *this;
}

webpp::cookies&
webpp::cookies::expires(cookie::name_t const& _name,
                        cookie::date_t const& _expires) noexcept {
    change_if(_name, [&](auto& it) {
        it->_expires.reset(new cookie::date_t{_expires});
    });
    return *this;
}

webpp::cookies&
webpp::cookies::expires(condition const& _condition,
                        cookie::date_t const& _expires) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_expires.reset(new cookie::date_t{_expires});
    });
    return *this;
}

webpp::cookies& webpp::cookies::expires(const_iterator const& it,
                                        cookie::date_t&& _expires) noexcept {
    it->_expires.reset(new cookie::date_t{_expires});
    return *this;
}

webpp::cookies&
webpp::cookies::expires(const_iterator const& it,
                        cookie::date_t const& _expires) noexcept {
    return expires(it, cookie::date_t(_expires));
}

webpp::cookies&
webpp::cookies::max_age(cookie::max_age_t const& _max_age) noexcept {
    for (auto& c : *this)
        c._max_age = _max_age;
    return *this;
}
webpp::cookies&
webpp::cookies::max_age(cookie::name_t const& _name,
                        cookie::max_age_t const& _max_age) noexcept {
    change_if(_name, [&](auto& it) { it->_max_age = _max_age; });
    return *this;
}
webpp::cookies&
webpp::cookies::max_age(condition const& _condition,
                        cookie::max_age_t const& _max_age) noexcept {
    change_if(_condition, [&](auto& it) { it->_max_age = _max_age; });
    return *this;
}

webpp::cookies& webpp::cookies::max_age(const_iterator const& it,
                                        cookie::max_age_t&& _max_age) noexcept {
    it->_max_age = std::move(_max_age);
    return *this;
}

webpp::cookies&
webpp::cookies::max_age(const_iterator const& it,
                        cookie::max_age_t const& _max_age) noexcept {
    return max_age(it, cookie::max_age_t(_max_age));
}

webpp::cookies& webpp::cookies::value(cookie::value_t const& _value) noexcept {
    for (auto& c : *this)
        c._value = _value;
    return *this;
}

webpp::cookies& webpp::cookies::value(cookie::name_t const& _name,
                                      cookie::value_t const& _value) noexcept {
    change_if(_name, [&](auto& it) { it->_value = _value; });
    return *this;
}

webpp::cookies& webpp::cookies::value(const_iterator const& it,
                                      cookie::value_t const& _value) noexcept {
    return value(it, cookie::value_t(_value));
}

webpp::cookies& webpp::cookies::value(const_iterator const& it,
                                      cookie::value_t&& _value) noexcept {
    it->_value = std::move(_value);
    return *this;
}

webpp::cookies& webpp::cookies::value(condition const& _condition,
                                      cookie::value_t const& _value) noexcept {
    change_if(_condition, [&](auto& it) { it->_value = _value; });
    return *this;
}

webpp::cookies& webpp::cookies::path(cookie::path_t const& _path) noexcept {
    change_all([&](auto& it) {
        it->_path = _path;
        make_unique(it, [&](auto const& c) { return c._path == _path; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::path(cookie::name_t const& _name,
                                     cookie::path_t const& _path) noexcept {
    change_if(_name, [&](auto& it) {
        it->_path = _path;
        make_unique(it, [&](auto const& c) { return c._path == _path; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::path(condition const& _condition,
                                     cookie::path_t const& _path) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_path = _path;
        make_unique(it, [&](auto const& c) { return c._path == _path; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::path(const_iterator const& it,
                                     cookie::path_t&& _path) noexcept {
    it->_path = _path;
    make_unique(it, [&](auto const& c) { return c._path == it->_path; });
    return *this;
}

webpp::cookies& webpp::cookies::path(const_iterator const& it,
                                     cookie::path_t const& _path) noexcept {
    return path(it, cookie::path_t(_path));
}

webpp::cookies&
webpp::cookies::domain(cookie::domain_t const& _domain) noexcept {
    change_all([&](auto& it) {
        it->_domain = _domain;
        make_unique(it, [&](auto const& c) { return c._domain == _domain; });
    });
    return *this;
}

webpp::cookies&
webpp::cookies::domain(cookie::name_t const& _name,
                       cookie::domain_t const& new_domain) noexcept {
    change_if(_name, [&](auto& it) {
        it->_domain = new_domain;
        make_unique(it, [&](auto const& c) { return c._domain == new_domain; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::domain(const_iterator const& it,
                                       cookie::domain_t&& new_domain) noexcept {
    it->_domain = std::move(new_domain);
    make_unique(it, [&](auto const& c) { return c._domain == it->_domain; });
    return *this;
}

webpp::cookies&
webpp::cookies::domain(const_iterator const& it,
                       cookie::domain_t const& new_domain) noexcept {
    return domain(it, cookie::domain_t(new_domain));
}

webpp::cookies&
webpp::cookies::domain(condition const& _condition,
                       cookie::domain_t const& new_domain) noexcept {
    change_if(_condition, [&](auto& it) { it->_domain = new_domain; });
    return *this;
}

webpp::cookies& webpp::cookies::name(cookie::name_t const& _name) noexcept {
    change_all([&](auto& it) {
        it->_name = _name;
        make_unique(it, [&](auto const& c) { return c._name == _name; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::name(cookie::name_t const& old_name,
                                     cookie::name_t const& new_name) noexcept {
    change_if(old_name, [&](auto& it) {
        it->_name = new_name;
        make_unique(it, [&](auto const& c) { return c._name == new_name; });
    });
    return *this;
}

webpp::cookies& webpp::cookies::name(const_iterator const& it,
                                     cookie::name_t&& new_name) noexcept {
    it->_name = std::move(new_name);
    make_unique(it, [&](auto const& c) { return c._name == it->_name; });
    return *this;
}

webpp::cookies& webpp::cookies::name(const_iterator const& it,
                                     cookie::name_t const& new_name) noexcept {
    return name(it, cookie::name_t(new_name));
}

webpp::cookies& webpp::cookies::name(condition const& _condition,
                                     cookie::name_t const& new_name) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_name = new_name;
        make_unique(it, [&](auto const& c) { return c._name == new_name; });
    });
    return *this;
}
