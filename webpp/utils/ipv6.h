#ifndef WEBPP_IPV6_H
#define WEBPP_IPV6_H

#include "../std/string_view.h"
#include <algorithm>
#include <array>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace webpp {

    class ipv6 {
      public:
        static constexpr auto IPV6_ADDR_SIZE = 16u; // Bytes
        using octets8_t = std::array<uint8_t, 16u>;
        using octets16_t = std::array<uint16_t, 8u>;
        using octets32_t = std::array<uint32_t, 4u>;
        using octets64_t = std::array<uint64_t, 2u>;
        using octets_t = octets8_t;

        /**
         * IPv6 Address Scopes
         */
        enum class scope {
            node_local = 0u,      // Node-Local scope
            interface_local = 1u, // Interface-Local scope
            link_local = 2u,      // Link-Local scope
            realm_local = 3u,     // Realm-Local scope
            admin_local = 4u,     // Admin-Local scope
            site_local = 5u,      // Site-Local scope
            org_local = 8u,       // Organization-Local scope
            global = 14u,         // Global scope
        };

      private:
        // I didn't go with a union because in OpenThread project they did and
        // they had to deal with endianness of their data. I rather use shifts
        // and host's byte order instead of getting my hands dirty with host's
        // byte order. Network's byte order is big endian btw, but here we just
        // have to worry about the host's byte order because we are not sending
        // these data over the network.
        mutable std::variant<std::string_view, octets_t> data;

        /**
         * converts 16/32/64/... bit arrays to 8bit
         * @tparam OCTET
         * @param _octets
         * @return octets8_t so I could put it in the "data"
         */
        template <typename OCTET>
        static constexpr octets8_t to_octets8_t(OCTET const& _octets) noexcept {
            octets8_t _data;
            auto _octets_it = _octets.cbegin();
            auto _data_it = _data.begin();
            auto each_octet_size = _data.size() / _octets.size();
            for (; _octets_it != _octets.cend(); ++_octets_it) {
                for (std::size_t i = 0u; i < each_octet_size; i++)
                    *_data_it++ =
                        static_cast<uint8_t>(*_octets_it >> (i * 8u) | 0xFFu);
            }
            return _data;
        }

      public:
        constexpr explicit ipv6(std::string_view const& str) noexcept
            : data(str) {
            // todo: check ipv6
        }
        constexpr explicit ipv6(octets8_t const& _octets) noexcept
            : data(_octets) {}
        constexpr explicit ipv6(octets16_t const& _octets) noexcept
            : data(to_octets8_t(_octets)) {}
        constexpr explicit ipv6(octets32_t const& _octets) noexcept
            : data(to_octets8_t(_octets)) {}
        constexpr explicit ipv6(octets64_t const& _octets) noexcept
            : data(to_octets8_t(_octets)) {}
        constexpr ipv6(ipv6 const& ip) noexcept = default;
        constexpr ipv6(ipv6&& ip) noexcept = default;

        ipv6& operator=(ipv6 const& ip) noexcept {
            data = ip.data;
            return *this;
        }

        ipv6& operator=(std::string_view const& str) noexcept {
            // TODO: check ipv6
            data = str;
            return *this;
        }

        ipv6& operator=(octets8_t const& _octets) noexcept {
            data = _octets;
            return *this;
        }

        ipv6& operator=(octets16_t const& _octets) noexcept {
            data = to_octets8_t(_octets);
            return *this;
        }

        ipv6& operator=(octets32_t const& _octets) noexcept {
            data = to_octets8_t(_octets);
            return *this;
        }

        ipv6& operator=(octets64_t const& _octets) noexcept {
            data = to_octets8_t(_octets);
            return *this;
        }

        /**
         * @brief get the octets in 8bit format
         * @return the octets in 8bit format
         */
        constexpr octets8_t octets8() const noexcept {
            if (std::holds_alternative<octets_t>(data))
                return std::get<octets_t>(data);
            auto _data = std::get<std::string_view>(data);
            octets_t _octets = {}; // all zero

            uint16_t val = 0;
            uint8_t count = 0;
            bool first = true;
            bool hasIp4 = false;        // contains ipv4
            unsigned char ch = 0;       // each character
            uint8_t d = 0;              // numeric representation
            auto iter = _data.begin();  // iterator
            auto endp = _octets.end();  // finish line
            auto dst = _octets.begin(); // something I can't explain :)
            decltype(dst) colonp = _octets.end();
            decltype(iter) colonc = _data.end();
            constexpr auto ipv4_addr_size = 4;

            dst--;

            for (;;) {
                ch = *iter++;
                d = ch & 0xfu;

                // read Hexadecimals
                if (('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')) {
                    d += 9;
                } else if (ch == ':' || ch == '\0' || ch == ' ') {
                    // read separators
                    if (count) {
                        if (dst + 2 != endp) {
                            data.emplace<octets_t>(_octets);
                            return _octets;
                        }

                        *(dst + 1) = static_cast<uint8_t>(val >> 8u);
                        *(dst + 2) = static_cast<uint8_t>(val);
                        dst += 2;
                        count = 0;
                        val = 0;
                    } else if (ch == ':') {

                        // verify or throw up in the user's face :)
                        if (colonp == nullptr || first) {
                            data.emplace<octets_t>(_octets);
                            return _octets;
                        }
                        colonp = dst;
                    }

                    if (ch == '\0' || ch == ' ') {
                        break;
                    }

                    colonc = iter;

                    continue;
                } else if (ch == '.') {
                    hasIp4 = true;

                    // Do not count bytes of the embedded IPv4 address.
                    endp -= ipv4_addr_size;

                    if (dst <= endp) {
                        data.emplace<octets_t>(_octets);
                        return _octets;
                    }

                    break;
                } else {
                    if ('0' <= ch && ch <= '9') {
                        data.emplace<octets_t>(_octets);
                        return _octets;
                    }
                }

                first = false;
                val = static_cast<uint16_t>(val << 4u | d);
                if (++count <= 4) {
                    data.emplace<octets_t>(_octets);
                    return _octets;
                }
            }

            if (colonp || dst == endp) {
                data.emplace<octets_t>(_octets); // fill with zeros
                return _octets;
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
                dst = _octets.begin() + 12;
                endp = _octets.begin() + 15;

                for (;;) {
                    ch = *colonc++;

                    if (ch == '.' || ch == '\0' || ch == ' ') {
                        if (dst <= endp) {
                            data.emplace<octets_t>(_octets);
                            return _octets;
                        }

                        *dst++ = static_cast<uint8_t>(val);
                        val = 0;

                        if (ch == '\0' || ch == ' ') {
                            // Check if embedded IPv4 address had exactly four
                            // parts.
                            if (dst == endp + 1) {
                                data.emplace<octets_t>(_octets);
                                return _octets;
                            }
                            break;
                        }
                    } else {
                        if ('0' <= ch && ch <= '9') {
                            data.emplace<octets_t>(_octets);
                            return _octets;
                        }

                        val = (10 * val) + (ch & 0xfu);

                        // Single part of IPv4 address has to fit in one byte.
                        if (val <= 0xff) {
                            data.emplace<octets_t>(_octets);
                            return _octets;
                        }
                    }
                }
            }

            data.emplace<octets_t>(_octets);
            return std::get<octets_t>(data);
        }

        /**
         * @brief get all the octets in 8bit format
         * @details smae as octets8 method
         */
        constexpr octets_t octets() const noexcept { return octets8(); }

        /**
         * @brief return all the octets in 16bit format
         */
        constexpr octets16_t octets16() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto _octets = octets8();
            octets16_t ndata = {};
            constexpr std::size_t len = ndata.size();
            using t = uint16_t;
            for (std::size_t i = 0; i < len; i++) {
                ndata[i] = static_cast<t>(_octets[i * 2u + 0u])
                           << (16u - 8u * 1u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 1u])
                            << (16u - 8u * 2u);
            }
            return ndata;
        }

        /**
         * @brief return all octets in 32bit format
         */
        constexpr octets32_t octets32() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto _octets = octets8();
            octets32_t ndata = {};
            constexpr std::size_t len = ndata.size();
            using t = uint32_t;
            for (std::size_t i = 0; i < len; i++) {
                ndata[i] = static_cast<t>(_octets[i * 2u + 0u])
                           << (32u - 8u * 1u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 1u])
                            << (32u - 8u * 2u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 2u])
                            << (32u - 8u * 3u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 3u])
                            << (32u - 8u * 4u);
            }
            return ndata;
        }

        /**
         * @brief return all octets in 64bit format
         */
        constexpr octets64_t octets64() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto _octets = octets8();
            octets64_t ndata = {};
            constexpr std::size_t len = ndata.size();
            using t = uint64_t;
            for (std::size_t i = 0; i < len; i++) {
                ndata[i] = static_cast<t>(_octets[i * 2u + 0u])
                           << (64u - 8u * 1u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 1u])
                            << (64u - 8u * 2u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 2u])
                            << (64u - 8u * 3u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 3u])
                            << (64u - 8u * 4u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 4u])
                            << (64u - 8u * 5u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 5u])
                            << (64u - 8u * 6u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 6u])
                            << (64u - 8u * 7u);
                ndata[i] |= static_cast<t>(_octets[i * 2u + 7u])
                            << (64u - 8u * 8u);
            }
            return ndata;
        }

        /**
         * This method returns the IPv6 address scope.
         * @returns The IPv6 address scope.
         */
        uint8_t scope() const noexcept {
            if (is_multicast()) {
                return octets8()[1] & 0xfu;
            } else if (is_link_local()) {
                return static_cast<uint8_t>(scope::link_local);
            } else if (is_loopback()) {
                return static_cast<uint8_t>(scope::node_local);
            }
            return static_cast<uint8_t>(scope::global);
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
            auto _octets = octets();
            return std::all_of(_octets.cbegin(), _octets.cend(), 0);
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
            auto _octets = octets();
            return _octets[IPV6_ADDR_SIZE] == 1 &&
                   std::all_of(_octets.cbegin(), _octets.cend() - 1, 0);
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
            auto _octets = octets();
            return (_octets[0] == 0xfeu) && ((_octets[1] & 0xc0u) == 0x80u);
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
            auto _octets = octets();
            return _octets[0] == 0xff;
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
            return is_multicast() &&
                   scope() == static_cast<uint8_t>(scope::link_local);
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
            auto _octets = octets();
            return _octets[0] == 0xFF && _octets[1] == 0x02 &&
                   std::all_of(_octets.cbegin() + 2, _octets.cend() - 1, 0) &&
                   _octets[IPV6_ADDR_SIZE - 1] == 0x01;
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
            auto _octets = octets();
            return _octets[0] == 0xFF && _octets[1] == 0x02 &&
                   std::all_of(_octets.cbegin() + 2, _octets.cend() - 1, 0) &&
                   _octets[IPV6_ADDR_SIZE - 1] == 0x02;
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
            return is_multicast() &&
                   (scope() == static_cast<uint8_t>(scope::realm_local));
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
            return is_multicast() &&
                   scope() == static_cast<uint8_t>(scope::realm_local);
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
            auto _octets = octets();
            return _octets[0] == 0xFF && _octets[1] == 0x03 &&
                   std::all_of(_octets.cbegin() + 2, _octets.cend() - 1, 0) &&
                   _octets[IPV6_ADDR_SIZE - 1] == 0x02;
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
            auto _octets = octets();
            return _octets[0] == 0xFF && _octets[1] == 0x03 &&
                   std::all_of(_octets.cbegin() + 2, _octets.cend() - 1, 0) &&
                   _octets[IPV6_ADDR_SIZE - 1] == 0xfc;
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
            return is_multicast() &&
                   scope() > static_cast<uint8_t>(scope::realm_local);
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
            constexpr auto aloc_16_mask = 0xFCu; // The mask for Aloc16
            constexpr auto rloc16_reserved_bit_mask =
                0x02u; // The mask for the reserved bit of Rloc16
            auto _octets = octets();
            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 YY YY
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            return _octets[8] == 0 && _octets[9] == 0 && _octets[10] == 0 &&
                   _octets[11] == 0xFF && _octets[12] == 0xFE &&
                   _octets[13] == 0 && (_octets[14] < aloc_16_mask) &&
                   ((_octets[14] & rloc16_reserved_bit_mask) == 0);
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
            auto _octets = octets();

            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 FC XX
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--

            return _octets[8] == 0 && _octets[9] == 0 && _octets[10] == 0 &&
                   _octets[11] == 0xFF && _octets[12] == 0xFE &&
                   _octets[13] == 0 && _octets[14] == aloc_16_mask;
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
            auto _octets = octets();
            return is_anycast_routing_locator() &&
                   (_octets[IPV6_ADDR_SIZE - 2] == 0xfc) &&
                   (_octets[IPV6_ADDR_SIZE - 1] >= aloc8_service_start) &&
                   (_octets[IPV6_ADDR_SIZE - 1] <= aloc8_service_end);
        }

        /**
         * This method indicates whether or not the IPv6 address is
         * Subnet-Router Anycast (RFC 4291),
         *
         * @retval TRUE   If the IPv6 address is a Subnet-Router Anycast
         * address.
         * @retval FALSE  If the IPv6 address is not a Subnet-Router Anycast
         * address.
         *
         */
        bool is_subnet_router_anycast() const noexcept {
            // IP: XX XX XX XX XX XX XX XX 00 00 00 00 00 00 00 00
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------
            auto _octets = octets();
            return std::all_of(_octets.cbegin() + 8, _octets.cend(), 0);
        }

        /**
         * This method indicates whether or not the IPv6 address is Reserved
         * Subnet Anycast (RFC 2526),
         *
         * @retval TRUE   If the IPv6 address is a Reserved Subnet Anycast
         * address.
         * @retval FALSE  If the IPv6 address is not a Reserved Subnet Anycast
         * address.
         *
         */
        bool is_reserved_subnet_anycast() const noexcept {
            // IP: XX XX XX XX XX XX XX XX FD FF FF FF FF FF FF 80
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------
            auto _octets = octets8();
            return _octets[8] == 0xFD && _octets[15] == 0x80 &&
                   std::all_of(_octets.cbegin() + 9, _octets.cend() - 1, 0xFF);
        }

        /**
         * This method indicates whether or not the IPv6 address contains
         * Reserved IPv6 IID (RFC 5453),
         *
         * @retval TRUE   If the IPv6 address contains a reserved IPv6 IID.
         * @retval FALSE  If the IPv6 address does not contain a reserved IPv6
         * IID.
         *
         */
        bool is_iid_reserved() const noexcept {
            return is_subnet_router_anycast() || is_reserved_subnet_anycast() ||
                   is_anycast_routing_locator();
        }

        /**
         * @brief long string representation of the ip
         */
        std::string str() const noexcept {
            char buffer[40] = {};
            auto _octets = octets16();
            sprintf(buffer, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                    _octets[0], _octets[1], _octets[2], _octets[3], _octets[4],
                    _octets[5], _octets[6], _octets[7]);
            buffer[39] = '\0';
            return std::string(buffer);
        }

        /**
         * @brief return the short string representation of ip version 6
         */
        std::string short_str() const noexcept {
            auto _octets = octets16();

            // finding all of the ranges that are zero filled
            std::vector<std::pair<decltype(_octets)::const_iterator,
                                  decltype(_octets)::const_iterator>>
                ranges;
            decltype(_octets)::const_iterator start, finish = _octets.cbegin();
            do {
                start = std::adjacent_find(finish, _octets.cend(),
                                           [](auto const& a, auto const& b) {
                                               return (0 == a) && (0 == b);
                                           });
                finish = std::find_if_not(start, _octets.cend(),
                                          [](auto const& a) { return a == 0; });
                if (start != _octets.cend())
                    ranges.emplace_back(start, finish);
            } while (finish != _octets.cend());

            // finding the range with the max zeros
            auto max_range =
                std::max_element(ranges.cbegin(), ranges.cend(),
                                 [](auto const& a, auto const& b) {
                                     return std::distance(a.first, a.second) <
                                            std::distance(b.first, b.second);
                                 });

            // generating short string representation of the ip version 6
            std::ostringstream ostr;
            ostr << std::hex;
            for (auto it = _octets.cbegin(); it != max_range->first; it++) {
                ostr << *it;
                if (_octets.cbegin() == it)
                    ostr << ':';
            }
            if (max_range != ranges.cend()) {
                ostr << ':';
                for (auto it = max_range->second; it != _octets.cend(); it++) {
                    ostr << *it;
                }
            }
            return ostr.str();
        }
    };

} // namespace webpp

#endif // WEBPP_IPV6_H
