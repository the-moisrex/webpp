#ifndef WEBPP_IPV6_HPP
#define WEBPP_IPV6_HPP

#include "../strings/append.hpp"
#include "inet_ntop.hpp"
#include "inet_pton.hpp"
#include "ipv4.hpp"

#include <array>
#include <compare>

// There are plenty of magic numbers in this file, they ain't magic, you'll know what they mean when you see
// them, so we're disabling this warning for this file.
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

namespace webpp {

    namespace details {
        define_is_specialization_of(is_specializes_array,
                                    WEBPP_SINGLE_ARG(typename, stl::size_t),
                                    WEBPP_SINGLE_ARG(typename T, stl::size_t N),
                                    WEBPP_SINGLE_ARG(T, N))
    };

    /**
     * This utility will get you the uint8_t representation of status result of ipv6 parsing
     * so you can store it in a uint8_t storage along with the prefix.
     */
    static constexpr stl::uint8_t prefix_status(inet_pton6_status status) noexcept {
        return static_cast<stl::uint8_t>(status);
    }

    struct ipv6 {
        // todo: add support for systems that support 128bit integer types
        static constexpr stl::uint8_t max_prefix_value = 128u;

        static constexpr auto IPV6_ADDR_SIZE = 16u; // Bytes
        using octets8_t                      = stl::array<stl::uint8_t, 16u>;
        using octets16_t                     = stl::array<stl::uint16_t, 8u>;
        using octets32_t                     = stl::array<stl::uint32_t, 4u>;
        using octets64_t                     = stl::array<stl::uint64_t, 2u>;
        using octets_t                       = octets8_t;
        using octets_value_t                 = typename octets_t::value_type;

        /**
         * IPv6 Address Scopes
         */
        enum class scope {
            node_local      = 0u,  // Node-Local scope
            interface_local = 1u,  // Interface-Local scope
            link_local      = 2u,  // Link-Local scope
            realm_local     = 3u,  // Realm-Local scope
            admin_local     = 4u,  // Admin-Local scope
            site_local      = 5u,  // Site-Local scope
            org_local       = 8u,  // Organization-Local scope
            global          = 14u, // Global scope
        };

      private:
        static constexpr auto interface_identifier_offset = 8u; // Interface Identifier offset in bytes
        static constexpr auto interface_identifier_size   = 8u; // Interface Identifier size in bytes

        // I didn't go with a union because in OpenThread project they did, and
        // they had to deal with endianness of their data. I rather use shifts
        // and host's byte order instead of getting my hands dirty with host's
        // byte order. Network's byte order is big endian btw, but here we just
        // have to worry about the host's byte order because we are not sending
        // these data over the network.
        octets_t     data    = {}; // filled with zeros
        stl::uint8_t _prefix = prefix_status(inet_pton6_status::valid);

        /**
         * converts 16/32/64/... bit arrays to 8bit
         * @tparam OCTET
         * @param _octets
         * @return octets8_t so I could put it in the "data"
         */
        template <typename IntType, stl::size_t N>
            requires(N > 1 && N <= 16)
        [[nodiscard]] static constexpr octets_t to_octets_t(stl::array<IntType, N> const& _octets) noexcept {
            if constexpr (N == 2) { // to 64bits
                return {
                  static_cast<stl::uint8_t>(_octets[0] >> (7u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (6u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (5u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (4u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (3u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (2u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] >> (1u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[0] & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (7u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (6u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (5u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (4u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (3u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (2u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1] >> (1u * 8u) & 0xFFu),
                  static_cast<stl::uint8_t>(_octets[1]),
                };
            } else if constexpr (N == 4) { // 4 of 32bits
                return {static_cast<stl::uint8_t>(_octets[0] >> 24u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[0] >> 16u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[0] >> 8u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[0] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[1] >> 24u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[1] >> 16u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[1] >> 8u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[1] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[2] >> 24u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[2] >> 16u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[2] >> 8u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[2] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[3] >> 24u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[3] >> 16u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[3] >> 8u & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[3] & 0xFFu)};
            } else if constexpr (N == 8) { // 8 of 16bits
                return {static_cast<stl::uint8_t>(_octets[0] >> 8u),
                        static_cast<stl::uint8_t>(_octets[0] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[1] >> 8u),
                        static_cast<stl::uint8_t>(_octets[1] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[2] >> 8u),
                        static_cast<stl::uint8_t>(_octets[2] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[3] >> 8u),
                        static_cast<stl::uint8_t>(_octets[3] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[4] >> 8u),
                        static_cast<stl::uint8_t>(_octets[4] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[5] >> 8u),
                        static_cast<stl::uint8_t>(_octets[5] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[6] >> 8u),
                        static_cast<stl::uint8_t>(_octets[6] & 0xFFu),
                        static_cast<stl::uint8_t>(_octets[7] >> 8u),
                        static_cast<stl::uint8_t>(_octets[7] & 0xFFu)};
            } else if constexpr (N == 16) { // 16 of 8bits
                return _octets;
            } else {
                // todo: test this, this used to be wrong:
                octets_t   _data           = {};
                auto       _octets_it      = _octets.cbegin();
                auto       _data_it        = _data.begin();
                auto const each_octet_size = _data.size() / _octets.size();
                for (; _octets_it != _octets.cend(); ++_octets_it) {
                    auto _octet = *_octets_it;
                    for (stl::size_t i = 0u; i < each_octet_size; i++) {
                        _octet >>= i * 8u;
                        _octet &= 0xFFu;
                        *_data_it++ = static_cast<stl::uint8_t>(*_octets_it);
                    }
                }
                return _data;
            }
        }


        /**
         * parses the string_view to the uint8 structure
         */
        constexpr void parse(istl::StringViewifiable auto&& _ipv6_data) noexcept {
            auto  ip_str  = istl::string_viewify(stl::forward<decltype(_ipv6_data)>(_ipv6_data));
            auto* inp_ptr = ip_str.data();
            auto* out_ptr = data.data();

            // set the default value to valid
            _prefix           = prefix_status(inet_pton6_status::valid);
            const auto status = inet_pton6(inp_ptr, inp_ptr + ip_str.size(), out_ptr, _prefix);
            if (status != inet_pton6_status::valid) {
                // set the status
                _prefix = prefix_status(status);
            }
        }

      public:
        static consteval ipv6 invalid() noexcept {
            return {prefix_status(inet_pton6_status::invalid_prefix)};
        }

        // Create an ipv6 at compile-time; a simple consteval constructor helper
        template <typename... Args>
        static consteval ipv6 create(Args&&... args) noexcept {
            return {stl::forward<Args>(args)...};
        }


        // initialize with ::0
        constexpr ipv6() noexcept = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename StrT>
            requires(istl::StringViewifiable<StrT> &&
                     !details::is_specializes_array_v<stl::remove_cvref_t<StrT>,
                                                      stl::array> && // it shouldn't be an array
                     !stl::same_as<stl::remove_cvref_t<StrT>, ipv6>) // so it's not copy ctor
        constexpr ipv6(StrT&& str) noexcept {
            parse(stl::forward<StrT>(str));
        }
        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <typename StrT>
            requires(istl::StringViewifiable<StrT> &&
                     !details::is_specializes_array_v<stl::remove_cvref_t<StrT>,
                                                      stl::array> && // it shouldn't be an array
                     !stl::same_as<stl::remove_cvref_t<StrT>, ipv6>) // so it's not copy ctor
        constexpr ipv6(StrT&& str, stl::uint8_t prefix_value) noexcept {
            parse(stl::forward<StrT>(str));
            if (is_valid()) {
                prefix(prefix_value);
            }
        }

        // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
        constexpr ipv6(const stl::uint8_t (&_octets)[16],
                       stl::uint8_t prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : data{_octets[0],
                 _octets[1],
                 _octets[2],
                 _octets[3],
                 _octets[4],
                 _octets[5],
                 _octets[6],
                 _octets[7],
                 _octets[8],
                 _octets[9],
                 _octets[10],
                 _octets[11],
                 _octets[12],
                 _octets[13],
                 _octets[14],
                 _octets[15]} {
            prefix(prefix_value);
        }
        // NOLINTEND(cppcoreguidelines-avoid-c-arrays)
        constexpr ipv6(octets8_t const& _octets,
                       stl::uint8_t     prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : data{_octets} {
            prefix(prefix_value);
        }
        constexpr ipv6(octets16_t const& _octets,
                       stl::uint8_t      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : data{to_octets_t(_octets)} {
            prefix(prefix_value);
        }

        constexpr ipv6(octets32_t const& _octets,
                       stl::uint8_t      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : data{to_octets_t(_octets)} {
            prefix(prefix_value);
        }

        constexpr ipv6(octets64_t const& _octets,
                       stl::uint8_t      prefix_value = prefix_status(inet_pton6_status::valid)) noexcept
          : data{to_octets_t(_octets)} {
            prefix(prefix_value);
        }


        constexpr ipv6(stl::uint8_t prefix_value) noexcept {
            prefix(prefix_value);
        }

        constexpr ipv6(ipv6 const& ip) noexcept = default;
        constexpr ipv6(ipv6&& ip) noexcept      = default;

        constexpr ipv6& operator=(ipv6 const& ip) noexcept = default;
        constexpr ipv6& operator=(ipv6&&) noexcept         = default;
        constexpr ~ipv6() noexcept                         = default;

        template <typename StrT>
            requires(istl::StringViewifiable<StrT> && !stl::is_array_v<stl::remove_cvref_t<StrT>>)
        constexpr ipv6& operator=(StrT&& str) noexcept {
            parse(stl::forward<StrT>(str));
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }

        constexpr ipv6& operator=(octets8_t const& _octets) noexcept {
            data    = _octets;
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }

        // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
        constexpr ipv6& operator=(const stl::uint8_t (&_octets)[16]) noexcept {
            stl::copy_n(stl::begin(_octets), 16u, std::begin(data));
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }
        // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

        constexpr ipv6& operator=(octets16_t const& _octets) noexcept {
            data    = to_octets_t(_octets);
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }

        constexpr ipv6& operator=(octets32_t const& _octets) noexcept {
            data    = to_octets_t(_octets);
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }

        constexpr ipv6& operator=(octets64_t const& _octets) noexcept {
            data    = to_octets_t(_octets);
            _prefix = prefix_status(inet_pton6_status::valid);
            return *this;
        }

        constexpr auto operator<=>(ipv6 const& other) const noexcept = default;
        // todo: add other stuff for operator<=>

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr bool operator==(StrT&& ip) const noexcept {
            // only compare the octets and not the prefix
            return ipv6(istl::string_viewify<stl::string_view>(stl::forward<StrT>(ip))).data == data;
        }

        [[nodiscard]] constexpr bool operator==(ipv6 ip) const noexcept {
            return _prefix == ip._prefix && data == ip.data;
        }

        [[nodiscard]] constexpr bool operator==(octets8_t ip) const noexcept {
            return data == ip;
        }

        [[nodiscard]] constexpr bool operator==(octets16_t ip) const noexcept {
            return octets16() == ip;
        }

        [[nodiscard]] constexpr bool operator==(octets32_t ip) const noexcept {
            return octets32() == ip;
        }

        [[nodiscard]] constexpr bool operator==(octets64_t ip) const noexcept {
            return octets64() == ip;
        }


        explicit operator octets8_t() const noexcept {
            return octets8();
        }

        explicit operator octets16_t() const noexcept {
            return octets16();
        }

        explicit operator octets32_t() const noexcept {
            return octets32();
        }

        explicit operator octets64_t() const noexcept {
            return octets64();
        }

        /**
         * @brief get the octets in 8bit format
         * @return the octets in 8bit format
         */
        [[nodiscard]] constexpr octets8_t octets8() const noexcept {
            return data;
        }

        /**
         * @brief get all the octets in 8bit format
         * @details same as octets8 method
         */
        [[nodiscard]] constexpr octets_t octets() const noexcept {
            return octets8();
        }

        /**
         * @brief return all the octets in 16bit format
         */
        [[nodiscard]] constexpr octets16_t octets16() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto const            _octets = octets8();
            octets16_t            ndata   = {};
            constexpr stl::size_t len     = ndata.size();
            using t                       = uint16_t;
            for (stl::size_t i = 0; i < len; i++) {
                ndata[i] = static_cast<uint16_t>((static_cast<t>(_octets[i * 2u + 0u]) << (16u - 8u * 1u)) |
                                                 (static_cast<t>(_octets[i * 2u + 1u]) << (16u - 8u * 2u)));
            }
            return ndata;
        }

        /**
         * @brief return all octets in 32bit format
         */
        [[nodiscard]] constexpr octets32_t octets32() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto const            _octets = octets8();
            octets32_t            ndata   = {};
            constexpr stl::size_t len     = ndata.size();
            using t                       = uint32_t;
            for (stl::size_t i = 0; i < len; i++) {
                ndata[i] = (static_cast<t>(_octets[i * 2u + 0u]) << (32u - 8u * 1u)) |
                           (static_cast<t>(_octets[i * 2u + 1u]) << (32u - 8u * 2u)) |
                           (static_cast<t>(_octets[i * 2u + 2u]) << (32u - 8u * 3u)) |
                           (static_cast<t>(_octets[i * 2u + 3u]) << (32u - 8u * 4u));
            }
            return ndata;
        }

        /**
         * @brief return all octets in 64bit format
         */
        [[nodiscard]] constexpr octets64_t octets64() const noexcept {
            // IP: XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------

            auto const            _octets = octets8();
            octets64_t            ndata   = {};
            constexpr stl::size_t len     = ndata.size();
            using t                       = uint64_t;
            for (stl::size_t i = 0; i < len; i++) {
                ndata[i] = (static_cast<t>(_octets[i * 2u + 0u]) << (64u - 8u * 1u)) |
                           (static_cast<t>(_octets[i * 2u + 1u]) << (64u - 8u * 2u)) |
                           (static_cast<t>(_octets[i * 2u + 2u]) << (64u - 8u * 3u)) |
                           (static_cast<t>(_octets[i * 2u + 3u]) << (64u - 8u * 4u)) |
                           (static_cast<t>(_octets[i * 2u + 4u]) << (64u - 8u * 5u)) |
                           (static_cast<t>(_octets[i * 2u + 5u]) << (64u - 8u * 6u)) |
                           (static_cast<t>(_octets[i * 2u + 6u]) << (64u - 8u * 7u)) |
                           (static_cast<t>(_octets[i * 2u + 7u]) << (64u - 8u * 8u));
            }
            return ndata;
        }

        /**
         * This method returns the IPv6 address scope.
         * @returns The IPv6 address scope.
         */
        [[nodiscard]] constexpr stl::uint8_t scope() const noexcept {
            if (is_multicast()) {
                return multicast_scope();
            } else if (is_link_local()) {
                return static_cast<stl::uint8_t>(scope::link_local);
            } else if (is_loopback()) {
                return static_cast<stl::uint8_t>(scope::node_local);
            }
            return static_cast<stl::uint8_t>(scope::global);
        }

        /**
         * Return the scope for a multicast address.
         * This method may only be called on multicast addresses.
         */
        [[nodiscard]] constexpr stl::uint8_t multicast_scope() const noexcept {
            return octets8()[1] & 0xfu;
        }


        /**
         * Creates an ipv6 instance with all but most significant num_bits set to 0.
         *
         * @param [in] num_bits number of bits to mask
         * @return ipv6 instance with bits set to 0
         */
        [[nodiscard]] constexpr ipv6 mask(stl::size_t num_bits) const noexcept {
            num_bits                = stl::min<stl::size_t>(num_bits, max_prefix_value);
            constexpr auto _0s      = uint64_t(0);
            constexpr auto _1s      = ~_0s;
            auto const     fragment = _1s << ((max_prefix_value - num_bits) % 64u);
            auto const     hi       = num_bits <= 64 ? fragment : _1s;
            auto const     lo       = num_bits <= 64 ? 0ull : fragment;

            auto const _octets = octets64();
            return {octets64_t{hi & _octets[0], lo & _octets[1]}};
        }

        /**
         * This method indicates whether or not the IPv6 address is the Unspecified Address.
         * Unspecified IPv6 Address == ::0
         *
         * @retval TRUE   If the IPv6 address is the Unspecified Address.
         * @retval FALSE  If the IPv6 address is not the Unspecified Address.
         *
         */
        [[nodiscard]] constexpr bool is_unspecified() const noexcept {
            return is_zero();
        }

        /**
         * This method indicates whether or not the IPv6 address is the Loopback Address.
         *
         * @retval TRUE   If the IPv6 address is the Loopback Address.
         * @retval FALSE  If the IPv6 address is not the Loopback Address.
         */
        [[nodiscard]] constexpr bool is_loopback() const noexcept {
            auto const _octets = octets8();
            // either ::1 or v4-mapped of ipv4 loopback address (::ffff:127.0.0.1)
            return _octets == octets8_t{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ||
                   _octets == octets8_t{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1};
        }

        /**
         * This method indicates whether or not the IPv6 address scope is Interface-Local.
         *
         * @retval TRUE   If the IPv6 address scope is Interface-Local.
         * @retval FALSE  If the IPv6 address scope is not Interface-Local.
         */
        [[nodiscard]] constexpr bool is_link_local() const noexcept {
            auto const _octets = octets8();
            return (_octets[0] == 0xfeu) && ((_octets[1] & 0xc0u) == 0x80u);
        }

        /**
         * This method indicates whether or not the IPv6 address is multicast address.
         *
         * @retval TRUE   If the IPv6 address is a multicast address.
         * @retval FALSE  If the IPv6 address scope is not a multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            auto const _octets = octets8();
            return _octets[0] == 0xffu;
        }

        /**
         * Determine whether the address is a global multicast address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_multicast_global() const noexcept {
            auto const _octets = octets8();
            return ((_octets[0] == 0xffu) && ((_octets[1] & 0x0fu) == 0x0eu));
        }

        /**
         * Determine whether the address is a link-local multicast address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_multicast_link_local() const noexcept {
            auto const _octets = octets8();
            return ((_octets[0] == 0xffu) && ((_octets[1] & 0x0fu) == 0x02u));
        }

        /**
         * Determine whether the address is a node-local multicast address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_multicast_node_local() const noexcept {
            auto const _octets = octets8();
            return ((_octets[0] == 0xffu) && ((_octets[1] & 0x0fu) == 0x01u));
        }

        /**
         * Determine whether the address is a org-local multicast address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_multicast_org_local() const noexcept {
            auto const _octets = octets8();
            return ((_octets[0] == 0xffu) && ((_octets[1] & 0x0fu) == 0x08u));
        }

        /**
         * Determine whether the address is a site-local multicast address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_multicast_site_local() const noexcept {
            auto const _octets = octets8();
            return ((_octets[0] == 0xffu) && ((_octets[1] & 0x0fu) == 0x05u));
        }

        /**
         * Determine whether the address is site local
         * @return bool
         */
        [[nodiscard]] constexpr bool is_site_local() const noexcept {
            auto const _octets = octets8();
            return (_octets[0] == 0xfeu) && ((_octets[1] & 0xc0u) == 0xc0u);
        }

        /**
         * Determine whether the address is a mapped IPv4 address
         * @return bool
         */
        [[nodiscard]] constexpr bool is_v4_mapped() const noexcept {
            auto const _octets = octets8();
            return (_octets[0] == 0) && (_octets[1] == 0) && (_octets[2] == 0) && (_octets[3] == 0) &&
                   (_octets[4] == 0) && (_octets[5] == 0) && (_octets[6] == 0) && (_octets[7] == 0) &&
                   (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0xff) && (_octets[11] == 0xff);
        }

        /**
         * This method indicates whether or not the IPv6 address is a link-local multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local multicast address.
         * @retval FALSE  If the IPv6 address scope is not a link-local multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_link_local_multicast() const noexcept {
            return is_multicast() && scope() == static_cast<stl::uint8_t>(scope::link_local);
        }

        /**
         * This method indicates whether or not the IPv6 address is a link-local
         * all nodes multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local all nodes multicast address.
         * @retval FALSE  If the IPv6 address is not a link-local all nodes multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_link_local_all_nodes_multicast() const noexcept {
            auto const _octets = octets8();
            return _octets[0] == 0xFFu && _octets[1] == 0x02u && (_octets[2] == 0) && (_octets[3] == 0) &&
                   (_octets[4] == 0) && (_octets[5] == 0) && (_octets[6] == 0) && (_octets[7] == 0) &&
                   (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0) && (_octets[11] == 0) &&
                   (_octets[12] == 0) && (_octets[13] == 0) && (_octets[14] == 0) && (_octets[15] == 0x01u);
        }

        /**
         * This method indicates whether or not the IPv6 address is a link-local
         * all routers multicast address.
         *
         * @retval TRUE   If the IPv6 address is a link-local all routers multicast address.
         * @retval FALSE  If the IPv6 address is not a link-local all routers multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_link_local_all_routers_multicast() const noexcept {
            auto const _octets = octets();
            return _octets[0] == 0xFFu && _octets[1] == 0x02u && (_octets[2] == 0) && (_octets[3] == 0) &&
                   (_octets[4] == 0) && (_octets[5] == 0) && (_octets[6] == 0) && (_octets[7] == 0) &&
                   (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0) && (_octets[11] == 0) &&
                   (_octets[12] == 0) && (_octets[13] == 0) && (_octets[14] == 0) && (_octets[15] == 0x02u);
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local multicast address.
         * @retval FALSE  If the IPv6 address scope is not a realm-local multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_realm_local_multicast() const noexcept {
            return is_multicast() && (scope() == static_cast<stl::uint8_t>(scope::realm_local));
        }

        /**
         * This method indicates whether or not the IPv6 address is a
         * realm-local all nodes multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all nodes multicast address.
         * @retval FALSE  If the IPv6 address is not a realm-local all nodes multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_realm_local_all_nodes_multicast() const noexcept {
            return is_multicast() && scope() == static_cast<stl::uint8_t>(scope::realm_local);
        }

        /**
         * This method indicates whether or not the IPv6 address is a realm-local all
         * routers multicast address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all routers multicast address.
         * @retval FALSE  If the IPv6 address is not a realm-local all routers multicast address.
         *
         */
        [[nodiscard]] constexpr bool is_realm_local_all_routers_multicast() const noexcept {
            auto const _octets = octets();
            return _octets[0] == 0xFFu && _octets[1] == 0x03u && (_octets[2] == 0) && (_octets[3] == 0) &&
                   (_octets[4] == 0) && (_octets[5] == 0) && (_octets[6] == 0) && (_octets[7] == 0) &&
                   (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0) && (_octets[11] == 0) &&
                   (_octets[12] == 0) && (_octets[13] == 0) && (_octets[14] == 0) && (_octets[15] == 0x02u);
        }

        /**
         * This method indicates whether or not the IPv6 address is a realm-local all MPL forwarders address.
         *
         * @retval TRUE   If the IPv6 address is a realm-local all MPL forwarders address.
         * @retval FALSE  If the IPv6 address is not a realm-local all MPL forwarders address.
         *
         */
        [[nodiscard]] constexpr bool is_realm_local_all_mpl_forwarders() const noexcept {
            auto const _octets = octets8();
            return _octets[0] == 0xFFu && _octets[1] == 0x03u && (_octets[2] == 0) && (_octets[3] == 0) &&
                   (_octets[4] == 0) && (_octets[5] == 0) && (_octets[6] == 0) && (_octets[7] == 0) &&
                   (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0) && (_octets[11] == 0) &&
                   (_octets[12] == 0) && (_octets[13] == 0) && (_octets[14] == 0) && (_octets[15] == 0xfcu);
        }

        /**
         * This method indicates whether or not the IPv6 address is multicast larger than realm local.
         *
         * @retval TRUE   If the IPv6 address is multicast larger than realm local.
         * @retval FALSE  If the IPv6 address is not multicast or the scope is not larger than realm local.
         */
        [[nodiscard]] constexpr bool is_multicast_larger_than_realm_local() const noexcept {
            return is_multicast() && scope() > static_cast<stl::uint8_t>(scope::realm_local);
        }

        /**
         * This method indicates whether or not the IPv6 address is a RLOC address.
         *
         * @retval TRUE   If the IPv6 address is a RLOC address.
         * @retval FALSE  If the IPv6 address is not a RLOC address.
         */
        [[nodiscard]] constexpr bool is_routing_locator() const noexcept {
            constexpr auto aloc_16_mask             = 0xFCu; // The mask for Aloc16
            constexpr auto rloc16_reserved_bit_mask = 0x02u; // The mask for the reserved bit of Rloc16
            auto const     _octets                  = octets();
            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 YY YY
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            return _octets[8] == 0 && _octets[9] == 0 && _octets[10] == 0 && _octets[11] == 0xFF &&
                   _octets[12] == 0xFE && _octets[13] == 0 && (_octets[14] < aloc_16_mask) &&
                   ((_octets[14] & rloc16_reserved_bit_mask) == 0);
        }

        /**
         * This method indicates whether or not the IPv6 address is an Anycast RLOC address.
         *
         * @retval TRUE   If the IPv6 address is an Anycast RLOC address.
         * @retval FALSE  If the IPv6 address is not an Anycast RLOC address.
         */
        [[nodiscard]] constexpr bool is_anycast_routing_locator() const noexcept {
            constexpr auto aloc_16_mask = 0xFC; // The mask for Aloc16
            auto const     _octets      = octets();

            // XX XX XX XX XX XX XX XX 00 00 00 FF FE 00 FC XX
            // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--

            return _octets[8] == 0 && _octets[9] == 0 && _octets[10] == 0 && _octets[11] == 0xFF &&
                   _octets[12] == 0xFE && _octets[13] == 0 && _octets[14] == aloc_16_mask;
        }

        /**
         * This method indicates whether or not the IPv6 address is an Anycast Service Locator.
         *
         * @retval TRUE   If the IPv6 address is an Anycast Service Locator.
         * @retval FALSE  If the IPv6 address is not an Anycast Service Locator.
         */
        [[nodiscard]] constexpr bool is_anycast_service_locator() const noexcept {
            constexpr auto aloc8_service_start = 0x10;
            constexpr auto aloc8_service_end   = 0x2f;
            auto const     _octets             = octets();
            return is_anycast_routing_locator() && (_octets[IPV6_ADDR_SIZE - 2] == 0xfc) &&
                   (_octets[IPV6_ADDR_SIZE - 1] >= aloc8_service_start) &&
                   (_octets[IPV6_ADDR_SIZE - 1] <= aloc8_service_end);
        }

        /**
         * This method indicates whether or not the IPv6 address is Subnet-Router Anycast (RFC 4291),
         *
         * @retval TRUE   If the IPv6 address is a Subnet-Router Anycast address.
         * @retval FALSE  If the IPv6 address is not a Subnet-Router Anycast address.
         */
        [[nodiscard]] constexpr bool is_subnet_router_anycast() const noexcept {
            // IP: XX XX XX XX XX XX XX XX 00 00 00 00 00 00 00 00
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------
            auto const _octets = octets();
            return (_octets[8] == 0) && (_octets[9] == 0) && (_octets[10] == 0) && (_octets[11] == 0) &&
                   (_octets[12] == 0) && (_octets[13] == 0) && (_octets[14] == 0) && (_octets[15] == 0);
        }

        /**
         * This method indicates whether or not the IPv6 address is Reserved Subnet Anycast (RFC 2526),
         *
         * @retval TRUE   If the IPv6 address is a Reserved Subnet Anycast address.
         * @retval FALSE  If the IPv6 address is not a Reserved Subnet Anycast address.
         */
        [[nodiscard]] constexpr bool is_reserved_subnet_anycast() const noexcept {
            // IP: XX XX XX XX XX XX XX XX FD FF FF FF FF FF FF 80
            // 08: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
            // 16: --0-- --1-- --2-- --3-- --4-- --5-- --6-- --7--
            // 32: -----0----- -----1----- -----2----- -----3-----
            // 64: -----------0----------- -----------1-----------
            auto const _octets = octets8();
            return _octets[8] == 0xFD && _octets[15] == 0x80 && (_octets[9] == 0xFFu) &&
                   (_octets[10] == 0xFFu) && (_octets[11] == 0xFFu) && (_octets[12] == 0xFFu) &&
                   (_octets[13] == 0xFFu) && (_octets[14] == 0xFFu);
        }

        /**
         * This method indicates whether or not the IPv6 address contains Reserved IPv6 IID (RFC 5453),
         *
         * @retval TRUE   If the IPv6 address contains a reserved IPv6 IID.
         * @retval FALSE  If the IPv6 address does not contain a reserved IPv6 IID.
         *
         */
        [[nodiscard]] constexpr bool is_iid_reserved() const noexcept {
            return is_subnet_router_anycast() || is_reserved_subnet_anycast() || is_anycast_routing_locator();
        }

        /**
         * This method returns a pointer to the Interface Identifier.
         * @returns A pointer to the Interface Identifier.
         */
        [[nodiscard]] constexpr octets8_t::iterator iid() noexcept {
            return data.begin() + interface_identifier_offset;
        }

        /**
         * This method returns a pointer to the Interface Identifier.
         * @returns A pointer to the Interface Identifier.
         */
        [[nodiscard]] constexpr octets8_t::const_iterator iid() const noexcept {
            return data.cbegin() + interface_identifier_offset;
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        /**
         * This method sets the Interface Identifier.
         * @param piid A reference to the Interface Identifier.
         */
        constexpr void iid(const stl::uint8_t* piid) noexcept {
            auto const _end = piid + interface_identifier_size;
            auto       _iid = iid();
            for (auto it = piid; it != _end; it++) {
                *_iid++ = *it;
            }
        }

        /**
         * This method sets the Interface Identifier.
         * @param A reference to the Interface Identifier.
         */
        constexpr void iid(const octets8_t::const_iterator& piid) noexcept {
            auto       _iid = iid();
            auto const _end = _iid + interface_identifier_size;
            auto       pit  = piid;
            for (auto it = _iid; it != _end; it++) {
                *it = *pit++;
            }
        }
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

        // /**
        //  * This method sets the Interface Identifier.
        //  * @param[in]  aExtAddress  A reference to the extended address.
        //  */
        // void set_iid(const Mac::ExtAddress& aExtAddress) {
        //     // TODO
        // }
        //
        // /**
        //  * This method converts the IPv6 Interface Identifier to an IEEE
        //  * 802.15.4 Extended Address.
        //  * @param[out]  aExtAddress  A reference to the extended address.
        //  */
        // void to_ext_address(Mac::ExtAddress& aExtAddress) const {
        //     // TODO
        // }
        //
        // /**
        //  * This method converts the IPv6 Interface Identifier to an IEEE
        //  * 802.15.4 MAC Address.
        //  * @param[out]  aMacAddress  A reference to the MAC address.
        //  */
        // void to_ext_address(Mac::Address& aMacAddress) const {
        //     // TODO
        // }
        //
        // /**
        //  * This method returns the number of IPv6 prefix bits that match.
        //  * @param[in]  other  The IPv6 address to match against.
        //  * @returns The number of IPv6 prefix bits that match.
        //  */
        // stl::uint8_t prefix_match(const otIp6Address& other) const {
        //     // TODO
        // }

        /**
         * @brief checks if the specified ip is valid or not
         * @return true if it is an unspecified ip address.
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _prefix <= max_prefix_value || _prefix == prefix_status(inet_pton6_status::valid);
        }

        /**
         * Get the ipv4 part of the ipv6 if it's v4-mapped.
         */
        [[nodiscard]] constexpr ipv4 mapped_v4() const noexcept {
            if (!is_valid() || !is_v4_mapped()) {
                return {};
            }
            return {data[12], data[13], data[14], data[15]};
        }

        /**
         * Return true if the IP address is private, as per RFC 1918 and RFC 4193.
         * For example, 192.168.xxx.xxx or fc00::/7 addresses.
         */
        [[nodiscard]] constexpr bool is_private() const noexcept {
            const ipv4 v4 = mapped_v4();
            // we check is_zero instead of is_v4_mapped because mapped_v4 already checks that.
            if (!v4.is_zero() && v4.is_private()) {
                return true;
            }
            return is_loopback() || starts_with(stl::array<stl::uint8_t, 2>{0xfcu, 0x00u}, 7);
        }

        /**
         * Check if the IP is all zero
         */
        [[nodiscard]] constexpr bool is_zero() const noexcept {
            auto const _octets = octets8();
            return _octets == octets8_t{};
        }

        /**
         * Check if Link Local Broadcast (ff02::1)
         */
        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return octets64_t{0xff02'0000'0000'0000ull, 0x1ull} == octets64();
        }


        /**
         * Is Routable
         */
        [[nodiscard]] constexpr bool is_routable() const noexcept {
            return
              // 2000::/3 is the only assigned global unicast block
              starts_with(stl::array<stl::uint8_t, 2>{0x20u, 0x00u}, 3u) ||
              // ffxe::/16 are global scope multicast addresses,
              // which are eligible to be routed over the internet
              (is_multicast() && multicast_scope() == 0xeu);
        }

        /**
         * Return true if the IP address is a special purpose address, as defined per RFC 6890.
         */
        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            return !is_routable();
        }

        /**
         * Check if the specified ipv6 binary starts with the specified inp_octets up to inp_prefix bits.
         */
        template <stl::size_t N>
            requires(N <= IPV6_ADDR_SIZE)
        [[nodiscard]] constexpr bool starts_with(stl::array<stl::uint8_t, N> inp_octets,
                                                 stl::size_t                 inp_prefix) const noexcept {
            const auto masked = mask(inp_prefix);
            return stl::equal(inp_octets.data(), inp_octets.data() + N, masked.octets8().data());
        }


        [[nodiscard]] constexpr bool starts_with(ipv6 const& ip, stl::size_t inp_prefix) const noexcept {
            const auto masked    = mask(inp_prefix).octets8();
            const auto ip_octets = ip.mask(inp_prefix).octets8();
            return stl::equal(ip_octets.begin(), ip_octets.end(), masked.begin());
        }


        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT expanded_string(Args&&... str_args) const noexcept {
            StrT output{stl::forward<Args>(str_args)...};
            expanded_string_to(output);
            return output;
        }

        /**
         * @brief long string representation of the ip
         */
        constexpr void expanded_string_to(istl::String auto& output) const noexcept {
            using char_type                   = istl::char_type_of_t<decltype(output)>;
            stl::array<char_type, 40> buffer  = {};
            auto const                _octets = octets16();

            auto it = fmt::format_to(buffer.data(),
                                     "{:04x}:{:04x}:{:04x}:{:04x}:{:04x}:{:04x}:{:04x}:{:04x}",
                                     _octets[0],
                                     _octets[1],
                                     _octets[2],
                                     _octets[3],
                                     _octets[4],
                                     _octets[5],
                                     _octets[6],
                                     _octets[7]);
            output.append(buffer.data(), it);
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT string(Args&&... str_args) const noexcept {
            StrT output{stl::forward<Args>(str_args)...};
            to_string(output);
            return output;
        }

        /**
         * @brief return the short string representation of ip version 6 + the prefix
         */
        constexpr void to_string(istl::String auto& output) const noexcept {
            resize_and_append(output, max_ipv6_str_len + 5, [this](auto* buf) constexpr noexcept {
                auto it = inet_ntop6(data.data(), buf);
                if (has_prefix()) {
                    *it++ = '/';
                    if (_prefix < 10) {
                        *it++ = static_cast<char>('0' + _prefix);
                    } else if (_prefix < 100) {
                        *it++ = static_cast<char>('0' + _prefix / 10);
                        *it++ = static_cast<char>('0' + _prefix % 10);
                    } else {
                        *it++ = static_cast<char>('0' + _prefix / 100);
                        *it++ = static_cast<char>('0' + _prefix % 100 / 10);
                        *it++ = static_cast<char>('0' + _prefix % 10);
                    }
                    *it++ = '\0';
                }
                return it;
            });
        }


        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT ip_string(Args&&... str_args) const noexcept {
            StrT output{stl::forward<Args>(str_args)...};
            ip_to_string(output);
            return output;
        }

        /**
         * @brief return the short string representation of ip version 6
         */
        constexpr void ip_to_string(istl::String auto& output) const noexcept {
            resize_and_append(output, max_ipv6_str_len, [this](auto* buf) constexpr noexcept {
                return inet_ntop6(data.data(), buf);
            });
        }


        /**
         * Get the prefix if exists or 255 otherwise
         */
        [[nodiscard]] constexpr stl::uint8_t prefix() const noexcept {
            return _prefix;
        }

        /**
         * Check if the ip has a prefix or not
         * @return bool an indication of weather or not the ip has a prefix or not
         */
        [[nodiscard]] constexpr bool has_prefix() const noexcept {
            return _prefix <= max_prefix_value;
        }

        /**
         * Set prefix for this ip address
         * @param prefix
         */
        constexpr ipv6& prefix(stl::uint8_t prefix_value) noexcept {
            if (prefix_value == prefix_status(inet_pton6_status::valid)) {
                _prefix = prefix_status(inet_pton6_status::valid);
            } else if (prefix_value > max_prefix_value) {
                data    = {}; // reset the ip if it was not valid
                _prefix = prefix_status(inet_pton6_status::invalid_prefix);
            } else {
                _prefix = prefix_value;
            }
            return *this;
        }

        /**
         * Clears the prefix from this ip
         */
        constexpr ipv6& clear_prefix() noexcept {
            return prefix(prefix_status(inet_pton6_status::valid));
        }

        /**
         * Get the ip in reversed order
         */
        [[nodiscard]] constexpr ipv6 reversed() const noexcept {
            return ipv6{octets_t{data[14],
                                 data[15],
                                 data[12],
                                 data[13],
                                 data[10],
                                 data[11],
                                 data[8],
                                 data[9],
                                 data[6],
                                 data[7],
                                 data[4],
                                 data[5],
                                 data[2],
                                 data[3],
                                 data[0],
                                 data[1]},
                        _prefix};
        }


        [[nodiscard]] constexpr inet_pton6_status status() const noexcept {
            if (_prefix <= max_prefix_value) {
                return inet_pton6_status::valid;
            }
            return static_cast<inet_pton6_status>(_prefix);
        }

        template <typename StrT>
        constexpr void status_to(StrT& output) const {
            set_string(output, webpp::to_string(status()));
        }


        template <typename StrT = stl::string_view, typename... Args>
        [[nodiscard]] constexpr auto status_string(Args&&... args) const {
            StrT str{stl::forward<Args>(args)...};
            status_to(str);
            return str;
        }
    };

} // namespace webpp

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

#endif // WEBPP_IPV6_HPP
