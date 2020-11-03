// Created by moisrex on 10/30/20.

#ifndef WEBPP_HOST_HPP
#define WEBPP_HOST_HPP

#include <variant>
#include "../std/optional.hpp"
#include "./parser.hpp"
#include "../utils/ipv6.hpp"

namespace webpp {

    template <typename ...T>
    struct host_type : public uri_parser<T...> {
        using super = uri_parser<T...>;
        using string_view_type = typename super::string_view_type;
        using string_type = typename super::string_type;

        /**
         * @brief return host as an string_view
         * @return string_view
         */
        [[nodiscard]] string_view_type raw() const noexcept {
            this->parse_host();
            if (this->authority_start == this->data.size()) {
                // there will not be a host without the authority_start
                return {};
            }

            stl::size_t start, len;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (this->user_info_end == this->data.size()) {
                // there's no user info
                start = this->authority_start;
            } else {
                // there's a user info
                start = this->user_info_end;
            }

            if (this->port_start != this->data.size()) {
                // but there's a port
                len = this->port_start - start;
            } else {
                // there's no port either
                if (this->authority_end != this->data.size()) {
                    // there's a path
                    len = this->authority_end - start;
                } else {
                    // there's no path either
                    len = this->data.size() - 1; // till the end
                }
            }

            return this->substr(start, len);
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid syntax.
         * @return
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, string_view_type>
        host_structured() const noexcept {
            auto _host = raw();
            if (is::ipv4(_host))
                return ipv4<traits_type>(_host);
            if (is::ipv6(_host))
                return ipv6<traits_type>(_host);
            return _host;
        }

        /**
         * @brief get the decoded version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        [[nodiscard]] stl::optional<string_type> decoded() const noexcept {
            string_type d_host{this->get_allocator()};
            if (decode_uri_component(raw(), d_host, super::REG_NAME_NOT_PCT_ENCODED))
                return d_host;
            return stl::nullopt;
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or not.
         * @return true if it can't find a hostname/ip in the uri or it's empty
         */
        [[nodiscard]] bool empty() const noexcept {
            return raw().empty();
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        auto& set(string_view_type const& new_host) noexcept {
            this->parse_host();

            // todo: are you sure it can handle punycode as well?
            string_type encoded_host{this->get_allocator()};
            encode_uri_component(new_host, encoded_host, super::REG_NAME_NOT_PCT_ENCODED);
            if ((!ascii::starts_with(new_host, '[') || !ascii::ends_with(new_host, ']')) &&
                is::ipv6(new_host)) {
                encoded_host = '[' + encoded_host + ']';
            }

            if (this->authority_start == this->data.size()) {
                // there's no authority start

                if (encoded_host.empty())
                    return *this; // there's nothing to do here. It's already
                // what the user wants

                if (this->scheme_end == this->data.size()) {
                    // there's no scheme either, so we just have to add to the
                    // beginning of the string
                    this->replace_value(0, 0, string_type("//") + encoded_host);
                    return *this;
                } else {
                    // there's a scheme
                    this->replace_value(this->scheme_end, 0, string_type("//") + encoded_host);
                    return *this;
                }
            }

            stl::size_t start, finish;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (this->user_info_end == this->data.size()) {
                // there's no user info
                if (this->scheme_end == this->data.size()) {
                    start = 0;
                    if (!new_host.empty() && !ascii::starts_with(string_view_type{encoded_host}, "//")) {
                        encoded_host = "//" + encoded_host;
                    }
                } else {
                    start = this->authority_start;
                }
            } else {
                // there's a user info
                start = this->user_info_end;
            }

            if (this->port_start != this->data.size()) {
                // but there's a port
                finish = this->port_start;
            } else {
                // there's no port either
                if (this->authority_end != this->data.size()) {
                    // there's a path
                    finish = this->authority_end;
                } else {
                    // there's no path either
                    finish = this->data.size() - 1; // till the end
                }
            }

            this->replace_value(start, finish - start, encoded_host);

            return *this;
        }

        /**
         * @brief get the hostname/ipv4/ipv6 from the URI. if the URI doesn't
         * include a hostname/ip or its hostname/ip is not in a valid shape, it
         * will return an empty string
         * @return string/ipv4/ipv6
         * @default empty string
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, string_type>
        host_structured_decoded() const noexcept {
            if (auto _host_structured = host_structured();
              stl::holds_alternative<string_view_type>(_host_structured))
                return decode_uri_component<traits_type>(stl::get<string_view_type>(_host_structured),
                                                         super::REG_NAME_NOT_PCT_ENCODED);
            else
                return _host_structured;
        }

        /**
         * @brief clear host part from URI
         * @return
         */
        auto& clear() noexcept {
            return set("");
        }

        /**
         * Check if the specified host is an IP address or not
         * This method check if the specified host is a IPv4 Address or has a
         * valid syntax for either IPv6 or Future version of IP addresses which
         * also starts with "[" and ends with "]" with an unknown syntax (as of
         * writing this code obv)
         * @return an indication of weather or not the specified Hostname is a
         * valid IP address or not
         */
        [[nodiscard]] bool is_ip() const noexcept {
            auto _host = raw();
            return is::ipv4(_host) || (ascii::starts_with(_host, '[') && ascii::ends_with(_host, ']'));
        }

        /**
         * Get the host and split it by dot separator. TLD (Top Level Domain)
         * will be the last one and Second Level Domain will be the one before
         * that and the rest will be subdomains.
         */
        [[nodiscard]] istl::vector<traits_type, string_type> domains() const noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return {};
            istl::vector<traits_type, string_type> subs;
            for (;;) {
                auto dot = _host.find('.');
                auto sub = _host.substr(0, dot);
                if (sub.empty())
                    break;
                subs.emplace_back(stl::move(sub));
                if (dot == string_view_type::npos)
                    break;
                _host.remove_prefix(dot + 1);
            }
            return subs;
        }

        /**
         * Get the TLD (top level domain) or sometimes called extension
         */
        [[nodiscard]] string_view_type top_level_domain() const noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return {};
            auto dot = _host.find_last_of('.');
            return _host.substr(dot != string_view_type::npos ? dot + 1 : 0);
        }

        /**
         * Set the TLD (Top Level Domain) in the uri
         * @param tld
         * @return
         */
        auto& top_level_domain(string_view_type const& tld) noexcept {
            auto _host = raw();
            if (_host.empty()) {
                // I've already written that code. Yay, I'm so happy
                set(tld);
            } else if (!is_ip() && !is::ip(tld)) {
                // cannot put an ip address as a tld, user should use set host
                // instead of this method.
                auto dot   = _host.find_last_of('.');
                auto start = dot != string_view_type::npos ? dot + 1 : 0;
                set(string_type(_host.substr(0, start)) + string_type(tld));
            }
            return *this;
        }

        /**
         * Check if the specified uri has a top level domain (TLD) or not
         * @return an indication of weather or not the URI has TLD or not
         */
        [[nodiscard]] bool has_top_level_domain() const noexcept {
            return !top_level_domain().empty();
        }

        /**
         * Get the second level domain out of the host
         */
        [[nodiscard]] string_view_type second_level_domain() const noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            auto start        = bef_last_dot == string_view_type::npos ? 0 : bef_last_dot + 1;
            auto sld          = _host.substr(start, last_dot - start);
            return sld;
        }

        /**
         * Set the second level domain to the specified string.
         * Attention: this method will only work if Top Level Domain already
         * exists
         * @param sld
         */
        auto& second_level_domain(string_view_type sld) noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return *this;

            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos) {
                // we have to insert it at the beginning of the host string

                // there's nothing to do it's empty
                if (!sld.empty()) {
                    static_cast<void>(set(string_type(sld) + '.' + string_type(_host)));
                }
            } else {
                auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
                auto start        = bef_last_dot == string_view_type::npos ? 0 : bef_last_dot + 1;
                if (!sld.empty())
                    static_cast<void>(
                      set(string_type(_host.substr(0, start)) + string_type(sld) + string_type(_host.substr(last_dot))));
                else
                    static_cast<void>(set(string_type(_host.substr(last_dot + 1))));
            }
            return *this;
        }

        /**
         * This method will remove the Second Level Domain and also any
         * Sub-Domains if there are any.
         */
        auto& clear_second_level_domain() noexcept {
            return second_level_domain({});
        }

        /**
         * Check if the specified uri has a second level domain or not.
         * @return An indication of weather or not the URI has SLD or not
         */
        [[nodiscard]] bool has_second_level_domain() const noexcept {
            return !second_level_domain().empty();
        }

        /**
         * Get the sub-domain (with sub-sub-...-sub-domain)
         * @return
         */
        [[nodiscard]] string_view_type subdomains() const noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == string_view_type::npos)
                return {};
            return _host.substr(0, bef_last_dot);
        }

        /**
         * Set the sub-domain part of the host name
         * Attention: this method will only work if Top Level Domain and Second
         * Level Domain already exists
         * @param sds
         */
        auto& subdomains(string_view_type sds) noexcept {
            auto _host = raw();
            if (_host.empty() || is_ip())
                return *this;
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return *this;
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == string_view_type::npos)
                return *this;
            if (sds.empty()) // special check for when we want to remove the SDS
                bef_last_dot++;
            static_cast<void>(set(string_type(sds) + string_type(_host.substr(bef_last_dot))));
            return *this;
        }

        /**
         * Remove the sub-domains if exists. This method should not have
         * side-effects if there's no sub-domain
         */
        auto& clear_subdomains() noexcept {
            return subdomains({});
        }

        /**
         * Check if the specified uri has at least one sub-domain or not
         * @return an indication of weather or not the URI has subdomains or not
         * TODO: we could be smarter here
         */
        [[nodiscard]] bool has_subdomains() const noexcept {
            return !subdomains().empty();
        }
    };

}

#endif // WEBPP_HOST_HPP
