#ifndef WEBPP_IP_IPV4_HPP
#define WEBPP_IP_IPV4_HPP

#include "../convert/casts.hpp"
#include "../strings/append.hpp"
#include "../strings/to_case.hpp"
#include "../validators/validators.hpp"
#include "inet_ntop.hpp"
#include "inet_pton.hpp"

#include <array>
#include <compare>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
namespace webpp {

    using ipv4_octet  = stl::uint8_t;
    using ipv4_octets = stl::array<ipv4_octet, 4u>;

    /**
     * @brief considers this ip as a subnet and converts it into a int prefix
     */
    constexpr ipv4_octet to_prefix(stl::uint32_t octets) noexcept {
        ipv4_octet prefix = 0u;
        for (stl::uint32_t mask = 0x80'00'00'00u; mask != 0u; mask >>= 1u) {
            if ((octets & mask) == mask) {
                prefix++;
            } else {
                return prefix;
            }
        }
        return prefix;
    }

    constexpr ipv4_octet to_prefix(ipv4_octets octets) noexcept {
        ipv4_octet prefix = 0u;
        for (auto const& octet : octets) {
            for (ipv4_octet mask = 0b1000'0000; mask != 0u; mask >>= 1u) {
                if ((octet & mask) == mask) {
                    prefix++;
                } else {
                    return prefix;
                }
            }
        }
        return prefix;
    }

    /**
     * Convert string to prefix
     * @param octets
     */
    template <istl::StringViewifiable StrT>
    constexpr ipv4_octet to_prefix(StrT&& inp_str) noexcept {
        const auto  str = istl::string_viewify(stl::forward<StrT>(inp_str));
        ipv4_octets bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
        if (inet_pton4(str.data(), str.data() + str.size(), bin.data()) == inet_pton4_status::valid) {
            return to_prefix(bin);
        }
        return 0u;
    }


    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr stl::uint32_t to_subnet(ipv4_octet prefix) noexcept {
        return 0xFF'FF'FF'FFu << (32u - prefix);
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr ipv4_octets to_subnet_array(ipv4_octet prefix) noexcept {
        auto subnet = to_subnet(prefix);
        return {static_cast<ipv4_octet>(subnet >> 24u & 0xFFu),
                static_cast<ipv4_octet>(subnet >> 16u & 0xFFu),
                static_cast<ipv4_octet>(subnet >> 8u & 0xFFu),
                static_cast<ipv4_octet>(subnet & 0xFFu)};
    }

    /**
     * This class gets you the uint8_t representation of the status enum result so
     * you can store the status and the prefix value in the same uint8_t storage.
     */
    static constexpr ipv4_octet prefix_status(inet_pton4_status status) noexcept {
        return static_cast<ipv4_octet>(status);
    }

    struct ipv4 {
      private:
        stl::uint32_t data    = 0u; // all bits are used
        ipv4_octet    _prefix = prefix_status(inet_pton4_status::valid);

        template <istl::StringViewifiable StrT>
        constexpr void parse(StrT&& inp_str) noexcept {
            const auto str = istl::string_viewify(stl::forward<StrT>(inp_str));

            // make sure prefix is set to mark the ip to be valid:
            _prefix = prefix_status(inet_pton4_status::valid);

            ipv4_octets bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
            auto const  res = inet_pton4(str.data(), str.data() + str.size(), bin.data(), _prefix);
            if (res == inet_pton4_status::valid) {
                data = parse(bin);
            } else {
                // set the error
                _prefix = prefix_status(res);
            }
        }

        static constexpr stl::uint32_t parse(ipv4_octets ip) noexcept {
            return static_cast<stl::uint32_t>(ip[0] << 24u) | static_cast<stl::uint32_t>(ip[1] << 16u) |
                   static_cast<stl::uint32_t>(ip[2] << 8u) | static_cast<stl::uint32_t>(ip[3]);
        }

      public:
        // initialize with 0.0.0.0
        constexpr ipv4() noexcept = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename T>
            requires(!istl::cvref_as<T, ipv4> && istl::StringViewifiable<T>)
        constexpr explicit ipv4(T&& ip) noexcept {
            parse(stl::forward<T>(ip));
        }
        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <istl::StringViewifiable IPStrT, istl::StringViewifiable SubStrT>
        constexpr ipv4(IPStrT&& ip, SubStrT&& subnet) noexcept {
            parse(stl::forward<IPStrT>(ip));
            if (is_valid()) {
                _prefix = (is::subnet(subnet) ? to_prefix(stl::forward<SubStrT>(subnet))
                                              : prefix_status(inet_pton4_status::invalid_prefix));
            }
        }

        template <istl::StringViewifiable IPStrT>
        constexpr ipv4(IPStrT&& ip, ipv4_octets subnet) noexcept {
            parse(stl::forward<IPStrT>(ip));
            if (is_valid()) {
                _prefix =
                  is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix);
            }
        }

        template <istl::StringViewifiable IPStrT>
        constexpr ipv4(IPStrT&& ip, ipv4_octet prefix_val) noexcept {
            parse(stl::forward<IPStrT>(ip));
            if (is_valid()) {
                _prefix = prefix_val > 32 && prefix_val != prefix_status(inet_pton4_status::valid)
                            ? prefix_status(inet_pton4_status::invalid_prefix)
                            : prefix_val;
            }
        }
        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        constexpr ipv4(ipv4_octet octet1,
                       ipv4_octet octet2,
                       ipv4_octet octet3,
                       ipv4_octet octet4,
                       ipv4_octet prefix_val = prefix_status(inet_pton4_status::valid)) noexcept
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(prefix_val > 32 && prefix_val != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix_val) {}

        constexpr ipv4(ipv4_octet       octet1,
                       ipv4_octet       octet2,
                       ipv4_octet       octet3,
                       ipv4_octet       octet4,
                       stl::string_view subnet) noexcept
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(is::subnet(subnet) ? to_prefix(subnet)
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr explicit ipv4(stl::uint32_t ip,
                                ipv4_octet    prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : data(ip),
            _prefix(prefix > 32u && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        template <istl::StringViewifiable StrT>
        constexpr explicit ipv4(stl::uint32_t ip, StrT&& subnet) noexcept
          : data(ip),
            _prefix(is::subnet(subnet) ? to_prefix(stl::forward<StrT>(subnet))
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr ipv4(ipv4_octets ip, ipv4_octet prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : data(parse(ip)),
            _prefix(prefix > 32u && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        template <istl::StringViewifiable StrT>
        constexpr ipv4(ipv4_octets ip, StrT&& subnet) noexcept
          : data(parse(ip)),
            _prefix(is::subnet(subnet) ? to_prefix(stl::forward<StrT>(subnet))
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr ipv4(ipv4_octets ip, ipv4_octets subnet) noexcept
          : data(parse(ip)),
            _prefix(is::subnet(subnet) ? to_prefix(subnet)
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        // NOLINTEND(bugprone-easily-swappable-parameters)

        constexpr explicit operator stl::uint32_t() const noexcept {
            return integer();
        }

        constexpr ipv4(ipv4 const& ip)                = default;
        constexpr ipv4(ipv4&& ip)                     = default;
        constexpr ~ipv4() noexcept                    = default;
        constexpr ipv4& operator=(ipv4 const& ip)     = default;
        constexpr ipv4& operator=(ipv4&& ip) noexcept = default;

        template <istl::StringViewifiable StrT>
        constexpr ipv4& operator=(StrT&& ip) noexcept {
            parse(stl::forward<StrT>(ip));
            _prefix = prefix_status(inet_pton4_status::valid);
            return *this;
        }

        constexpr ipv4& operator=(stl::uint32_t ip) noexcept {
            data    = ip;
            _prefix = prefix_status(inet_pton4_status::valid);
            return *this;
        }

        constexpr stl::strong_ordering operator<=>(ipv4 const&) const noexcept = default;
        constexpr stl::strong_ordering operator<=>(ipv4_octets other) const noexcept {
            return data <=> parse(other);
        }

        constexpr stl::strong_ordering operator<=>(stl::uint32_t const& ip) const noexcept {
            return data <=> ip;
        }

        template <istl::StringViewifiable StrT>
        constexpr stl::strong_ordering operator<=>(StrT&& ip) const noexcept {
            // only compare the octets not the prefix
            return ipv4(istl::string_viewify<stl::string_view>(stl::forward<StrT>(ip))).data <=> data;
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr bool operator==(StrT&& ip) const noexcept {
            // only compare the octets not the prefix
            return ipv4(istl::string_viewify<stl::string_view>(stl::forward<StrT>(ip))).data == data;
        }

        [[nodiscard]] constexpr bool operator==(ipv4 ip) const noexcept {
            return _prefix == ip._prefix && data == ip.data;
        }

        [[nodiscard]] constexpr bool operator==(stl::uint32_t ip) const noexcept {
            return data == ip;
        }

        [[nodiscard]] constexpr bool operator==(ipv4_octets ip) const noexcept {
            return data == parse(ip);
        }

        /**
         * @brief get string representation of the ip
         */
        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr auto string(Args&&... args) const {
            StrT str{stl::forward<Args>(args)...};
            to_string(str);
            return str;
        }

        constexpr void to_string(istl::String auto& out) const {
            resize_and_append(out, max_ipv4_str_len, [this](auto* buf) constexpr noexcept {
                const auto _octets = octets();
                return inet_ntop4(_octets.data(), buf);
            });
        }

        /**
         * @brief get the integer representation of the ip address
         * @return
         */
        [[nodiscard]] constexpr stl::uint32_t integer() const noexcept {
            return data;
        }

        /**
         * @brief get the 4 octets of the ip address
         * @return
         */
        [[nodiscard]] constexpr ipv4_octets octets() const noexcept {
            stl::uint32_t const _data = integer();
            return ipv4_octets({static_cast<ipv4_octet>(_data >> 24u),
                                static_cast<ipv4_octet>(_data >> 16u & 0x0FFu),
                                static_cast<ipv4_octet>(_data >> 8u & 0x0FFu),
                                static_cast<ipv4_octet>(_data & 0x0FFu)});
        }

        /**
         * @brief check if the ip is in the specified range or not
         * @param start
         * @param finish
         * @return
         */
        [[nodiscard]] constexpr bool in_range(ipv4 const& start, ipv4 const& finish) const noexcept {
            return *this >= start && *this <= finish;
        }

        /**
         * Get the prefix you specified in the constructor
         * @return
         */
        [[nodiscard]] constexpr auto prefix() const noexcept {
            return _prefix;
        }

        /**
         * Change the prefix of the ip
         * @param prefix_val
         */
        constexpr ipv4& prefix(ipv4_octet prefix_val) noexcept {
            _prefix = prefix_val > 32 && prefix_val != prefix_status(inet_pton4_status::valid)
                        ? prefix_status(inet_pton4_status::invalid_prefix)
                        : prefix_val;
            return *this;
        }

        /**
         * Set prefix with a subnet string
         * @param _subnet
         */
        template <istl::StringViewifiable StrT>
        constexpr ipv4& prefix(StrT&& _subnet) noexcept {
            return prefix(to_prefix(stl::forward<StrT>(_subnet)));
        }

        /**
         * Set prefix with a subnet array
         * @param _subnet
         */
        constexpr ipv4& prefix(ipv4_octets _subnet) noexcept {
            return prefix(to_prefix(_subnet));
        }

        /**
         * Remove prefix from the ip address
         */
        constexpr ipv4& clear_prefix() noexcept {
            return prefix(prefix_status(inet_pton4_status::valid));
        }

        /**
         * Check if the ip contains a prefix or not
         * @return bool an indication on weather or not the ip contains a prefix or not
         */
        [[nodiscard]] constexpr bool has_prefix() const noexcept {
            return _prefix <= 32;
        }

        /**
         * Check if the specified subnet or prefix was valid or not
         * @return bool
         */
        [[nodiscard]] constexpr bool has_valid_prefix() const noexcept {
            return _prefix != prefix_status(inet_pton4_status::invalid_prefix);
        }

        /**
         * @brief checks if the ip in this class is in the specified subnet or
         * not regardless of the the prefix that is specified in the ctor
         * @param ip
         * @param prefix
         * @return bool
         */
        [[nodiscard]] constexpr bool is_in_subnet(ipv4 const& ip) const noexcept {
            auto uint_val = integer();
            auto uint_ip  = ip.integer();
            uint_val &= 0xFFFFFFFFu << (32u - ip.prefix());
            uint_ip &= 0xFFFFFFFFu << (32u - ip.prefix());
            return uint_val == uint_ip;
        }

        /**
         * Is Loopback
         */
        [[nodiscard]] constexpr bool is_loopback() const noexcept {
            return is_in_subnet({127, 0, 0, 0, 8});
        }


        /**
         * Is Link Local
         */
        [[nodiscard]] constexpr bool is_link_local() const noexcept {
            return is_in_subnet({169, 254, 0, 0, 16});
        }

        /**
         * Returns true if the IP address is qualifies as broadcast
         */
        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return 0xFFFFFFFFu == integer();
        }

        /**
         * Return true if the IP address is a special purpose address, as defined per
         * RFC 6890 (i.e. 0.0.0.0 or 255.255.255.255).
         */
        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            auto const ip = integer();
            return is_private() || (ip <= 0x00FFFFFFu) ||      // 0.0.0.0      - 0.255.255.255
                   (ip >= 0xC0000000u && ip <= 0xC00000FFu) || // 192.0.0.0    - 192.0.0.255
                   (ip >= 0xC0000200u && ip <= 0xC00002FFu) || // 192.0.2.0    - 192.0.2.255
                   (ip >= 0xC6120000u && ip <= 0xC613FFFFu) || // 198.18.0.0   - 198.19.255.255
                   (ip >= 0xC6336400u && ip <= 0xC63364FFu) || // 198.51.100.0 - 198.51.100.255
                   (ip >= 0xCB007100u && ip <= 0xCB0071FFu) || // 203.0.113.0  - 203.0.113.255
                   (ip >= 0xE0000000u && ip <= 0xFFFFFFFFu);   // 224.0.0.0    - 255.255.255.255
        }


        /**
         * Return true if the IP address is a multicast address
         */
        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return (integer() & 0xf0000000u) == 0xe0000000u;
        }

        /**
         * @brief checks if the ip is in private range or not regardless of the prefix
         */
        [[nodiscard]] constexpr bool is_private() const noexcept {
            constexpr ipv4 class_C(ipv4_octets{192, 168, 0, 0}, 16);
            constexpr ipv4 class_B_start(ipv4_octets{172, 16, 0, 0});
            constexpr ipv4 class_B_finish(ipv4_octets{172, 31, 255, 255});
            constexpr ipv4 class_A(ipv4_octets{10, 0, 0, 0}, 8);
            return is_in_subnet(class_C) || in_range(class_B_start, class_B_finish) || is_in_subnet(class_A);
        }

        /**
         * @brief checks if the ip address is in public range or not
         * @return
         */
        [[nodiscard]] constexpr bool is_public() const noexcept {
            return !is_private();
        }

        /**
         * @brief check if all the octets are zero or not
         * @return true if all the octets are zero
         */
        [[nodiscard]] constexpr bool is_zero() const noexcept {
            return data == 0;
        }

        /**
         * Check if the ip you specified is valid or not (the ctor will not
         * throw an error if the specified string is not a valid ipv4 address)
         * @return bool
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return _prefix <= 32u || _prefix == prefix_status(inet_pton4_status::valid);
        }

        /**
         * Get the ip in reversed order; the prefix is unchanged
         */
        [[nodiscard]] constexpr ipv4 reversed() const noexcept {
            return {static_cast<ipv4_octet>(data & 0xFFu),
                    static_cast<ipv4_octet>(data >> 8u & 0xFFu),
                    static_cast<ipv4_octet>(data >> 16u & 0xFFu),
                    static_cast<ipv4_octet>(data >> 24u & 0xFFu),
                    _prefix};
        }


        // Get the parsing result
        [[nodiscard]] constexpr inet_pton4_status status() const noexcept {
            if (_prefix <= 32u) {
                return inet_pton4_status::valid;
            }
            return static_cast<inet_pton4_status>(_prefix);
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


#endif // WEBPP_IP_IPV4_HPP
