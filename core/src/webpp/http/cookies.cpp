#include "../../../include/webpp/http/cookies.h"

#include "../../../include/webpp/utils/strings.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string_view>


using namespace webpp;
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


cookie_jar::const_iterator
cookie_jar::find(basic_cookie::name_t const& name) const noexcept {
    return std::find_if(cbegin(), cend(), [&](auto const& a) {
        return a.name() == name;
    });
}

cookie_jar::const_iterator
cookie_jar::find(const basic_cookie& c) const noexcept {
    return std::find_if(cbegin(), cend(), [&](auto const& a) {
        return a.same_as(c);
    });
}

std::pair<cookie_jar::iterator, bool>
cookie_jar::insert(const value_type& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(value);
}

std::pair<cookie_jar::iterator, bool> cookie_jar::insert(value_type&& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(std::move(value));
}

cookie_jar::iterator cookie_jar::insert(const_iterator    hint,
                                        const value_type& value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(hint, value);
}

cookie_jar::iterator cookie_jar::insert(const_iterator hint,
                                        value_type&&   value) {
    auto found = find(value);
    if (found != cend())
        erase(found);
    return static_cast<super*>(this)->insert(hint, std::move(value));
}

void cookie_jar::insert(std::initializer_list<value_type> ilist) {
    for (const auto& it : ilist) {
        auto found = find(it);
        if (found != cend())
            erase(found);
    }
    return static_cast<super*>(this)->insert(ilist);
}

cookie_jar&
cookie_jar::encrypted(basic_cookie::encrypted_t const& _encrypted) noexcept {
    for (auto& c : *this)
        c._encrypted = _encrypted;
    return *this;
}
cookie_jar&
cookie_jar::encrypted(condition const&                 _condition,
                      basic_cookie::encrypted_t const& _encrypted) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_encrypted = _encrypted;
    });
    return *this;
}
cookie_jar&
cookie_jar::encrypted(basic_cookie::name_t const&      _name,
                      basic_cookie::encrypted_t const& _encrypted) noexcept {
    change_if(_name, [&](auto& it) {
        it->_encrypted = _encrypted;
    });
    return *this;
}
cookie_jar&
cookie_jar::encrypted(const_iterator const&     it,
                      basic_cookie::encrypted_t _encrypted) noexcept {
    it->_encrypted = _encrypted;
    return *this;
}

cookie_jar& cookie_jar::secure(basic_cookie::secure_t const& _secure) noexcept {
    for (auto& c : *this)
        c._secure = _secure;
    return *this;
}
cookie_jar& cookie_jar::secure(condition const&              _condition,
                               basic_cookie::secure_t const& _secure) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_secure = _secure;
    });
    return *this;
}
cookie_jar& cookie_jar::secure(basic_cookie::name_t const&   _name,
                               basic_cookie::secure_t const& _secure) noexcept {
    change_if(_name, [&](auto& it) {
        it->_secure = _secure;
    });
    return *this;
}
cookie_jar& cookie_jar::secure(const_iterator const&  it,
                               basic_cookie::secure_t _secure) noexcept {
    it->_secure = _secure;
    return *this;
}

cookie_jar&
cookie_jar::host_only(basic_cookie::host_only_t const& _host_only) noexcept {
    for (auto& c : *this)
        c._host_only = _host_only;
    return *this;
}

cookie_jar&
cookie_jar::host_only(condition const&                 _condition,
                      basic_cookie::host_only_t const& _host_only) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_host_only = _host_only;
    });
    return *this;
}

cookie_jar&
cookie_jar::host_only(basic_cookie::name_t const&      _name,
                      basic_cookie::host_only_t const& _host_only) noexcept {
    change_if(_name, [&](auto& it) {
        it->_host_only = _host_only;
    });
    return *this;
}

cookie_jar&
cookie_jar::host_only(const_iterator const&     it,
                      basic_cookie::host_only_t _host_only) noexcept {
    it->_host_only = _host_only;
    return *this;
}

cookie_jar& cookie_jar::prefix(basic_cookie::prefix_t const& _prefix) noexcept {
    for (auto& c : *this)
        c._prefix = _prefix;
    return *this;
}

cookie_jar& cookie_jar::prefix(basic_cookie::name_t const&   _name,
                               basic_cookie::prefix_t const& _prefix) noexcept {
    change_if(_name, [&](auto& it) {
        it->_prefix = _prefix;
    });
    return *this;
}

cookie_jar& cookie_jar::prefix(condition const&              _condition,
                               basic_cookie::prefix_t const& _prefix) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_prefix = _prefix;
    });
    return *this;
}

cookie_jar& cookie_jar::prefix(const_iterator const&  it,
                               basic_cookie::prefix_t _prefix) noexcept {
    it->_prefix = _prefix;
    return *this;
}

cookie_jar&
cookie_jar::comment(basic_cookie::comment_t const& _comment) noexcept {
    for (auto& c : *this)
        c._comment = _comment;
    return *this;
}
cookie_jar&
cookie_jar::comment(condition const&               _condition,
                    basic_cookie::comment_t const& _comment) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_comment = _comment;
    });
    return *this;
}
cookie_jar&
cookie_jar::comment(basic_cookie::name_t const&    _name,
                    basic_cookie::comment_t const& _comment) noexcept {
    change_if(_name, [&](auto& it) {
        it->_comment = _comment;
    });
    return *this;
}
cookie_jar& cookie_jar::comment(const_iterator const&     it,
                                basic_cookie::comment_t&& _comment) noexcept {
    it->_comment = std::move(_comment);
    return *this;
}
cookie_jar&
cookie_jar::comment(const_iterator const&          it,
                    basic_cookie::comment_t const& _comment) noexcept {
    return comment(it, basic_cookie::comment_t{_comment});
}

cookie_jar&
cookie_jar::same_site(basic_cookie::same_site_t const& _same_site) noexcept {
    for (auto& c : *this)
        c._same_site = _same_site;
    return *this;
}
cookie_jar&
cookie_jar::same_site(basic_cookie::name_t const&      _name,
                      basic_cookie::same_site_t const& _same_site) noexcept {
    change_if(_name, [&](auto& it) {
        it->_same_site = _same_site;
    });
    return *this;
}
cookie_jar&
cookie_jar::same_site(condition const&                 _condition,
                      basic_cookie::same_site_t const& _same_site) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_same_site = _same_site;
    });
    return *this;
}
cookie_jar&
cookie_jar::same_site(const_iterator const&     it,
                      basic_cookie::same_site_t _same_site) noexcept {
    it->_same_site = _same_site;
    return *this;
}

cookie_jar& cookie_jar::expires(basic_cookie::date_t const& _expires) noexcept {
    for (auto& c : *this)
        c._expires = _expires;
    return *this;
}

cookie_jar& cookie_jar::expires(basic_cookie::name_t const& _name,
                                basic_cookie::date_t const& _expires) noexcept {
    change_if(_name, [&](auto& it) {
        it->_expires = _expires;
    });
    return *this;
}

cookie_jar& cookie_jar::expires(condition const&            _condition,
                                basic_cookie::date_t const& _expires) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_expires = _expires;
    });
    return *this;
}

cookie_jar& cookie_jar::expires(const_iterator const&  it,
                                basic_cookie::date_t&& _expires) noexcept {
    it->_expires = _expires;
    return *this;
}

cookie_jar& cookie_jar::expires(const_iterator const&       it,
                                basic_cookie::date_t const& _expires) noexcept {
    return expires(it, basic_cookie::date_t(_expires));
}

cookie_jar&
cookie_jar::max_age(basic_cookie::max_age_t const& _max_age) noexcept {
    for (auto& c : *this)
        c._max_age = _max_age;
    return *this;
}
cookie_jar&
cookie_jar::max_age(basic_cookie::name_t const&    _name,
                    basic_cookie::max_age_t const& _max_age) noexcept {
    change_if(_name, [&](auto& it) {
        it->_max_age = _max_age;
    });
    return *this;
}
cookie_jar&
cookie_jar::max_age(condition const&               _condition,
                    basic_cookie::max_age_t const& _max_age) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_max_age = _max_age;
    });
    return *this;
}

cookie_jar& cookie_jar::max_age(const_iterator const&   it,
                                basic_cookie::max_age_t _max_age) noexcept {
    it->_max_age = _max_age;
    return *this;
}

cookie_jar& cookie_jar::value(basic_cookie::value_t const& _value) noexcept {
    for (auto& c : *this)
        c._value = _value;
    return *this;
}

cookie_jar& cookie_jar::value(basic_cookie::name_t const&  _name,
                              basic_cookie::value_t const& _value) noexcept {
    change_if(_name, [&](auto& it) {
        it->_value = _value;
    });
    return *this;
}

cookie_jar& cookie_jar::value(const_iterator const&        it,
                              basic_cookie::value_t const& _value) noexcept {
    return value(it, basic_cookie::value_t(_value));
}

cookie_jar& cookie_jar::value(const_iterator const&   it,
                              basic_cookie::value_t&& _value) noexcept {
    it->_value = std::move(_value);
    return *this;
}

cookie_jar& cookie_jar::value(condition const&             _condition,
                              basic_cookie::value_t const& _value) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_value = _value;
    });
    return *this;
}

cookie_jar& cookie_jar::path(basic_cookie::path_t const& _path) noexcept {
    change_all([&](auto& it) {
        it->_path = _path;
        make_it_unique(it, [&](auto const& c) {
            return c._path == _path;
        });
    });
    return *this;
}

cookie_jar& cookie_jar::path(basic_cookie::name_t const& _name,
                             basic_cookie::path_t const& _path) noexcept {
    change_if(_name, [&](auto& it) {
        it->_path = _path;
        make_it_unique(it, [&](auto const& c) {
            return c._path == _path;
        });
    });
    return *this;
}

cookie_jar& cookie_jar::path(condition const&            _condition,
                             basic_cookie::path_t const& _path) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_path = _path;
        make_it_unique(it, [&](auto const& c) {
            return c._path == _path;
        });
    });
    return *this;
}

cookie_jar& cookie_jar::path(const_iterator const&  it,
                             basic_cookie::path_t&& _path) noexcept {
    it->_path = _path;
    make_it_unique(it, [&](auto const& c) {
        return c._path == it->_path;
    });
    return *this;
}

cookie_jar& cookie_jar::path(const_iterator const&       it,
                             basic_cookie::path_t const& _path) noexcept {
    return path(it, basic_cookie::path_t(_path));
}

cookie_jar& cookie_jar::domain(basic_cookie::domain_t const& _domain) noexcept {
    change_all([&](auto& it) {
        it->_domain = _domain;
        make_it_unique(it, [&](auto const& c) {
            return c._domain == _domain;
        });
    });
    return *this;
}

cookie_jar&
cookie_jar::domain(basic_cookie::name_t const&   _name,
                   basic_cookie::domain_t const& new_domain) noexcept {
    change_if(_name, [&](auto& it) {
        it->_domain = new_domain;
        make_it_unique(it, [&](auto const& c) {
            return c._domain == new_domain;
        });
    });
    return *this;
}

cookie_jar& cookie_jar::domain(const_iterator const&    it,
                               basic_cookie::domain_t&& new_domain) noexcept {
    it->_domain = std::move(new_domain);
    make_it_unique(it, [&](auto const& c) {
        return c._domain == it->_domain;
    });
    return *this;
}

cookie_jar&
cookie_jar::domain(const_iterator const&         it,
                   basic_cookie::domain_t const& new_domain) noexcept {
    return domain(it, basic_cookie::domain_t(new_domain));
}

cookie_jar&
cookie_jar::domain(condition const&              _condition,
                   basic_cookie::domain_t const& new_domain) noexcept {
    change_if(_condition, [&](auto& it) {
        it->_domain = new_domain;
    });
    return *this;
}

cookie_jar& cookie_jar::name(basic_cookie::name_t const& __name) noexcept {
    if (auto first = begin(); first != end()) {
        first->_name = __name;
        erase(std::next(first), end()); // remove the rest
    }
    return *this;
}

cookie_jar& cookie_jar::name(basic_cookie::name_t const& old_name,
                             basic_cookie::name_t const& new_name) noexcept {
    erase(find(new_name));
    if (auto found = find(old_name); found != end())
        found->_name = new_name;
    return *this;
}

cookie_jar& cookie_jar::name(const_iterator const&  it,
                             basic_cookie::name_t&& new_name) noexcept {
    erase(find(new_name));
    it->_name = std::move(new_name);
    return *this;
}

cookie_jar& cookie_jar::name(const_iterator const&       it,
                             basic_cookie::name_t const& new_name) noexcept {
    return name(it, basic_cookie::name_t(new_name));
}

cookie_jar& cookie_jar::name(condition const&            _condition,
                             basic_cookie::name_t const& new_name) noexcept {
    erase(find(new_name));
    if (auto found = std::find_if(begin(), end(), _condition); found != end()) {
        found->_name = new_name;
    }
    return *this;
}
