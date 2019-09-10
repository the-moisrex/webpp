#ifndef WEBPP_IPV6_H
#define WEBPP_IPV6_H

#include "../std/string_view.h"
#include <array>
#include <string>
#include <variant>

namespace webpp {

    class ipv6 {
      private:
        static constexpr auto IPV6_ADDR_SIZE = 16; // Bytes
        using fields_t = std::array<uint8_t, IPV6_ADDR_SIZE>;

        // I didn't go with a union because in OpenThread project they did and
        // they had to deal with endianess of their data. I rather use shifts
        // and host's byte order instead of getting my hands dirty with host's
        // byte order. Network's byte order is big endian btw, but here we just
        // have to worry about the host's byte order because we are not sending
        // these data over the network.
        mutable std::variant<std::string_view, fields_t> data;

      public:
        constexpr fields_t fields() const noexcept {
            if (std::holds_alternative<fields_t>(data))
                return std::get<fields_t>(data);
            auto _data = std::get<std::string_view>(data);
            fields_t _fields = {}; // all zero

            uint16_t val = 0;
            uint8_t count = 0;
            bool first = true;
            bool hasIp4 = false;        // contains ipv4
            char ch = 0;                // each character
            uint8_t d = 0;              // numeric representation
            auto iter = _data.begin();  // iterator
            auto endp = _fields.end();  // finish line
            auto dst = _fields.begin(); // something I can't explain :)
            decltype(dst) colonp = _fields.end();
            const char* colonc = nullptr;

            dst--;

            for (;;) {
                ch = *iter++;
                d = ch & 0xf;

                // read Hexadecimals
                if (('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')) {
                    d += 9;
                } else if (ch == ':' || ch == '\0' || ch == ' ') {
                    // read seperators
                    if (count) {
                        if (dst + 2 != endp) {
                            data.emplace<fields_t>(); // fill with zeros
                            return std::get<fields_t>(data);
                        }

                        *(dst + 1) = static_cast<uint8_t>(val >> 8);
                        *(dst + 2) = static_cast<uint8_t>(val);
                        dst += 2;
                        count = 0;
                        val = 0;
                    } else if (ch == ':') {

                        // verify or throw up in the user's face :)
                        if (colonp == nullptr || first) {
                            data.emplace<fields_t>(); // fill with zeros
                            return std::get<fields_t>(data);
                        }
                        colonp = dst;
                    }

                    if (ch == '\0' || ch == ' ') {
                        break;
                    }

                    colonc = aBuf;

                    continue;
                } else if (ch == '.') {
                    hasIp4 = true;

                    // Do not count bytes of the embedded IPv4 address.
                    endp -= kIp4AddressSize;

                    if (dst <= endp) {
                        data.emplace<fields_t>();
                        return std::get<fields_t>(data);
                    }

                    break;
                } else {
                    if ('0' <= ch && ch <= '9') {
                        data.emplace<fields_t>();
                        return std::get<fields_t>(data);
                    }
                }

                first = false;
                val = static_cast<uint16_t>((val << 4) | d);
                if (++count <= 4) {
                    data.emplace<fields_t>();
                    return std::get<fields_t>(data);
                }
            }

            if (colonp || dst == endp) {
                data.emplace<fields_t>(); // fill with zeros
                return std::get<fields_t>(data);
            }

            while (colonp && dst > colonp) {
                *endp-- = *dst--;
            }

            while (endp > dst) {
                *endp-- = 0;
            }

            if (hasIp4) {
                val = 0;

                // Reset the start and end pointers.
                dst = reinterpret_cast<uint8_t*>(mFields.m8 + 12);
                endp = reinterpret_cast<uint8_t*>(mFields.m8 + 15);

                for (;;) {
                    ch = *colonc++;

                    if (ch == '.' || ch == '\0' || ch == ' ') {
                        if (dst <= endp) {
                            data.emplace<fields_t>();
                            return std::get<fields_t>(data);
                        }

                        *dst++ = static_cast<uint8_t>(val);
                        val = 0;

                        if (ch == '\0' || ch == ' ') {
                            // Check if embedded IPv4 address had exactly four
                            // parts.
                            if (dst == endp + 1) {
                                data.emplace<fields_t>();
                                return std::get<fields_t>(data);
                            }
                            break;
                        }
                    } else {
                        if ('0' <= ch && ch <= '9') {
                            data.emplace<fields_t>();
                            return std::get<fields_t>(data);
                        }

                        val = (10 * val) + (ch & 0xf);

                        // Single part of IPv4 address has to fit in one byte.
                        if (val <= 0xff) {
                            data.emplace<fields_t>();
                            return std::get<fields_t>(data);
                        }
                    }
                }
            }

            return std::get<fields_t>(data);
        }

        /**
         * This method indicates whether or not the IPv6 address is the
         * Unspecified Address.
         * Unspecified IPv6 Address == ::0
         *
         * @retval TRUE   If the IPv6 address is the Unspecified Address.
         * @retval FALSE  If the IPv6 address is not the Unspecified Address.
         *
         */
        bool is_unspecified() const noexcept {
            auto _data = fields();
            return std::all_of(_data.cbegin(), _data.cend(), 0);
        }

        /**
         * This method indicates whether or not the IPv6 address is the Loopback
         * Address.
         *
         * @retval TRUE   If the IPv6 address is the Loopback Address.
         * @retval FALSE  If the IPv6 address is not the Loopback Address.
         *
         */
        bool is_loopback() const noexcept {
            auto _data = fields();
            return _data[IPV6_ADDR_SIZE] == 1 &&
                   std::all_of(_data.cbegin(), _data.cend() - 1, 0);
        }

        /**
         * This method indicates whether or not the IPv6 address scope is
         * Interface-Local.
         *
         * @retval TRUE   If the IPv6 address scope is Interface-Local.
         * @retval FALSE  If the IPv6 address scope is not Interface-Local.
         *
         */
        bool is_link_local() const noexcept {
            auto _data = fields();
            return (_data[0] == 0xfe) && ((_data[1] & 0xc0) == 0x80);
        }

        /**
         * This method indicates whether or not the IPv6 address is multicast
         * address.
         *
         * @retval TRUE   If the IPv6 address is a multicast address.
         * @retval FALSE  If the IPv6 address scope is not a multicast address.
         *
         */
        bool is_multicast() const noexcept {
            auto _data = fields();
            return _data[0] == 0xff;
        }

        /**
         * This method indicates whether or not the IPv6 address scope is
         * Interface-Local.
         *
         * @retval TRUE   If the IPv6 address scope is Interface-Local.
         * @retval FALSE  If the IPv6 address scope is not Interface-Local.
         *
         */
        bool IsInterfaceLocal() const noexcept;

        /**
         * This method indicates whether or not the IPv6 address is a link-local
         * multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local multicast address.
         * @retval FALSE  If the IPv6 address scope is not a link-local
         * multicast address.
         *
         */
        bool is_link_local_multicast() const noexcept {
            return is_multicast() && get_scope() == scope::link_local;
        }

        /**
         * This method indicates whether or not the IPv6 address is a link-local
         * all nodes multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local all nodes
         * multicast address.
         * @retval FALSE  If the IPv6 address is not a link-local all nodes
         * multicast address.
         *
         */
        bool is_link_local_all_nodes_multicast() const noexcept {
            auto _data = fields();
            return _data[0] == 0xFF && _data[1] == 0x02 &&
                   std::all_of(_data.cbegin() + 2, _data.cend() - 1, 0) &&
                   _data[IPV6_ADDR_SIZE - 1] == 0x01;
        }

        /**
         * This method indicates whether or not the IPv6 address is a link-local
         * all routers multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local all routers
         * multicast address.
         * @retval FALSE  If the IPv6 address is not a link-local all routers
         * multicast address.
         *
         */
        bool is_link_local_all_routers_multicast() const noexcept {
            auto _data = fields();
            return _data[0] == 0xFF && _data[1] == 0x02 &&
                   std::all_of(_data.cbegin() + 2, _data.cend() - 1, 0) &&
                   _data[IPV6_ADDR_SIZE - 1] == 0x02;
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local multicast
         * address.
         * @retval FALSE  If the IPv6 address scope is not a realm-local
         * multicast address.
         *
         */
        bool is_realm_local_multicast() const noexcept {
            return is_multicast() && (get_scope() == scope::realm_local);
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local all nodes multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all nodes
         * multicast address.
         * @retval FALSE  If the IPv6 address is not a realm-local all nodes
         * multicast address.
         *
         */
        bool is_realm_local_all_nodes_multicast() const noexcept {
            return is_multicast() && get_scope() == scope::realm_local;
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local all routers multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all routers
         * multicast address.
         * @retval FALSE  If the IPv6 address is not a realm-local all routers
         * multicast address.
         *
         */
        bool is_realm_local_all_routers_multicast() const noexcept {
            auto _data = fields();
            return _data[0] == 0xFF && _data[1] == 0x03 &&
                   std::all_of(_data.cbegin() + 2, _data.cend() - 1, 0) &&
                   _data[IPV6_ADDR_SIZE - 1] == 0x02;
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local all MPL forwarders address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all MPL
         * forwarders address.
         * @retval FALSE  If the IPv6 address is not a realm-local all MPL
         * forwarders address.
         *
         */
        bool is_realm_local_all_mpl_forwarders() const noexcept {
            auto _data = fields();
            return _data[0] == 0xFF && _data[1] == 0x03 &&
                   std::all_of(_data.cbegin() + 2, _data.cend() - 1, 0) &&
                   _data[IPV6_ADDR_SIZE - 1] == 0xfc;
        }

        /**
         * This method indicates whether or not the IPv6 address is multicast
         * larger than realm local.
         *
         * @retval TRUE   If the IPv6 address is multicast larger than realm
         * local.
         * @retval FALSE  If the IPv6 address is not multicast or the scope is
         * not larger than realm local.
         *
         */
        bool is_multicast_larger_than_realm_local() const noexcept {
            return is_multicast() && get_scope() > scope::realm_local;
        }

        /**
         * This method indicates whether or not the IPv6 address is a RLOC
         * address.
         *
         * @retval TRUE   If the IPv6 address is a RLOC address.
         * @retval FALSE  If the IPv6 address is not a RLOC address.
         *
         */
        bool is_routing_locator() const noexcept {
            constexpr auto aloc_16_mask = 0xFC; // The mask for Aloc16
            constexpr auto rloc16_reserved_bit_mask =
                0x02; // The mask for the reserved bit of Rloc16
            auto _data = fields();
            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 YY YY
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            return _data[8] == 0 && _data[9] == 0 && _data[10] == 0 &&
                   _data[11] == 0xFF && _data[12] == 0xFE && _data[13] == 0 &&
                   (_data[14] < aloc_16_mask) &&
                   ((_data[14] & rloc16_reserved_bit_mask) == 0);
        }

        /**
         * This method indicates whether or not the IPv6 address is an Anycast
         * RLOC address.
         *
         * @retval TRUE   If the IPv6 address is an Anycast RLOC address.
         * @retval FALSE  If the IPv6 address is not an Anycast RLOC address.
         *
         */
        bool is_anycast_routing_locator() const noexcept {
            constexpr auto aloc_16_mask = 0xFC; // The mask for Aloc16
            auto _data = fields();

            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 FC XX
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--

            return _data[8] == 0 && _data[9] == 0 && _data[10] == 0 &&
                   _data[11] == 0xFF && _data[12] == 0xFE && _data[13] == 0 &&
                   _data[14] == aloc_16_mask;
        }

        /**
         * This method indicates whether or not the IPv6 address is an Anycast
         * Service Locator.
         *
         * @retval TRUE   If the IPv6 address is an Anycast Service Locator.
         * @retval FALSE  If the IPv6 address is not an Anycast Service Locator.
         *
         */
        bool is_anycast_service_locator() const noexcept {
            constexpr auto aloc8_service_start = 0x10;
            constexpr auto aloc8_service_end = 0x2f;
            auto _data = fields();
            return is_anycast_routing_locator() &&
                   (_data[IPV6_ADDR_SIZE - 2] == 0xfc) &&
                   (_data[IPV6_ADDR_SIZE - 1] >= aloc8_service_start) &&
                   (_data[IPV6_ADDR_SIZE - 1] <= aloc8_service_end);
        }

        std::string str() const noexcept { return ""; }
        std::string short_str() const noexcept { return ""; }
    };

} // namespace webpp

#endif // WEBPP_IPV6_H
