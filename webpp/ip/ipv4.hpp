#ifndef WEBPP_IP_IPV4_HPP
#define WEBPP_IP_IPV4_HPP

#include "../convert/casts.hpp"
#include "../strings/append.hpp"
#include "../strings/to_case.hpp"
#include "inet_ntop.hpp"
#include "inet_pton.hpp"
#include "ip_validators.hpp"

#include <array>
#include <compare>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp {

    using ipv4_octet  = stl::uint8_t;
    using ipv4_octets = stl::array<ipv4_octet, ipv4_byte_count>;

    /**
     * @brief considers this ip as a subnet and converts it into a int prefix
     */
    constexpr ipv4_octet to_prefix(stl::uint32_t octets) noexcept {
        ipv4_octet prefix = 0U;
        for (stl::uint32_t mask = 0x80'00'00'00U; mask != 0U; mask >>= 1U) {
            if ((octets & mask) == mask) {
                prefix++;
            } else {
                return prefix;
            }
        }
        return prefix;
    }

    constexpr ipv4_octet to_prefix(ipv4_octets octets) noexcept {
        ipv4_octet prefix = 0U;
        for (auto const& octet : octets) {
            for (ipv4_octet mask = 0b1000'0000; mask != 0U; mask >>= 1U) {
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
        ipv4_octets bin; // NOLINT(*-pro-type-member-init)
        auto        beg = str.begin();
        if (inet_pton4(beg, str.end(), bin.data()) == inet_pton4_status::valid) {
            return to_prefix(bin);
        }
        return 0U;
    }


    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr stl::uint32_t to_subnet(ipv4_octet prefix) noexcept {
        return 0xFF'FF'FF'FFU << static_cast<stl::uint32_t>(ipv4_max_prefix - prefix);
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr ipv4_octets to_subnet_array(ipv4_octet prefix) noexcept {
        auto subnet = to_subnet(prefix);
        return {static_cast<ipv4_octet>(subnet >> 24U & 0xFFU),
                static_cast<ipv4_octet>(subnet >> 16U & 0xFFU),
                static_cast<ipv4_octet>(subnet >> 8U & 0xFFU),
                static_cast<ipv4_octet>(subnet & 0xFFU)};
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
        stl::uint32_t data    = 0U; // all bits are used
        ipv4_octet    _prefix = prefix_status(inet_pton4_status::valid);

        template <istl::StringViewifiable StrT>
        constexpr void parse(StrT&& inp_str) noexcept {
            const auto str = istl::string_viewify(stl::forward<StrT>(inp_str));

            // make sure prefix is set to mark the ip to be valid:
            _prefix = prefix_status(inet_pton4_status::valid);

            ipv4_octets bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
            auto        beg = str.begin();
            auto const  res = inet_pton4(beg, str.end(), bin.data(), _prefix);
            if (webpp::is_valid(res)) {
                data = parse(bin);
            } else {
                // set the error
                _prefix = prefix_status(res);
            }
        }

        static constexpr stl::uint32_t parse(ipv4_octets ip_addr) noexcept {
            return static_cast<stl::uint32_t>(ip_addr[0] << 24U) |
                   static_cast<stl::uint32_t>(ip_addr[1] << 16U) |
                   static_cast<stl::uint32_t>(ip_addr[2] << 8U) | static_cast<stl::uint32_t>(ip_addr[3]);
        }

      public:
        // Create an ipv4 at compile-time; a simple consteval constructor helper
        template <typename... Args>
        static consteval ipv4 create(Args&&... args) noexcept {
            return ipv4{stl::forward<Args>(args)...};
        }

        static consteval ipv4 invalid() noexcept {
            ipv4 ip_addr;
            ip_addr._prefix = prefix_status(inet_pton4_status::invalid_prefix);
            return ip_addr;
        }

        // 0.0.0.0
        static consteval ipv4 any() noexcept {
            return ipv4{0x00000000U};
        }

        // 127.0.0.1
        static consteval ipv4 loopback() noexcept {
            return ipv4{0x7F000001U};
        }

        // 255.255.255.255
        static consteval ipv4 broadcast() noexcept {
            return ipv4{0xFFFFFFFFU};
        }

        // initialize with 0.0.0.0
        constexpr ipv4() noexcept = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename T>
            requires(!istl::cvref_as<T, ipv4> && istl::StringViewifiable<T>)
        constexpr explicit ipv4(T&& ip_addr) noexcept {
            parse(stl::forward<T>(ip_addr));
        }
        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <istl::StringViewifiable IPStrT, istl::StringViewifiable SubStrT>
        constexpr ipv4(IPStrT&& ip_addr, SubStrT&& subnet) noexcept {
            parse(stl::forward<IPStrT>(ip_addr));
            if (is_valid()) {
                _prefix = (is::subnet(subnet) ? to_prefix(stl::forward<SubStrT>(subnet))
                                              : prefix_status(inet_pton4_status::invalid_prefix));
            }
        }

        template <istl::StringViewifiable IPStrT>
        constexpr ipv4(IPStrT&& ip_addr, ipv4_octets subnet) noexcept {
            parse(stl::forward<IPStrT>(ip_addr));
            if (is_valid()) {
                _prefix =
                  is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix);
            }
        }

        template <istl::StringViewifiable IPStrT>
        constexpr ipv4(IPStrT&& ip_addr, ipv4_octet prefix_val) noexcept {
            parse(stl::forward<IPStrT>(ip_addr));
            if (is_valid()) {
                _prefix =
                  prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
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
            _prefix(prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
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

        constexpr explicit ipv4(stl::uint32_t ip_addr,
                                ipv4_octet    prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : data(ip_addr),
            _prefix(prefix > ipv4_max_prefix && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        template <istl::StringViewifiable StrT>
        constexpr explicit ipv4(stl::uint32_t ip_addr, StrT&& subnet) noexcept
          : data(ip_addr),
            _prefix(is::subnet(subnet) ? to_prefix(stl::forward<StrT>(subnet))
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr explicit ipv4(ipv4_octets ip_addr,
                                ipv4_octet  prefix = prefix_status(inet_pton4_status::valid)) noexcept
          : data(parse(ip_addr)),
            _prefix(prefix > ipv4_max_prefix && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        template <istl::StringViewifiable StrT>
        constexpr ipv4(ipv4_octets ip_addr, StrT&& subnet) noexcept
          : data(parse(ip_addr)),
            _prefix(is::subnet(subnet) ? to_prefix(stl::forward<StrT>(subnet))
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr ipv4(ipv4_octets ip_addr, ipv4_octets subnet) noexcept
          : data(parse(ip_addr)),
            _prefix(is::subnet(subnet) ? to_prefix(subnet)
                                       : prefix_status(inet_pton4_status::invalid_prefix)) {}

        // NOLINTEND(bugprone-easily-swappable-parameters)

        constexpr explicit operator stl::uint32_t() const noexcept {
            return integer();
        }

        constexpr ipv4(ipv4 const&)                = default;
        constexpr ipv4(ipv4&&)                     = default;
        constexpr ~ipv4() noexcept                 = default;
        constexpr ipv4& operator=(ipv4 const&)     = default;
        constexpr ipv4& operator=(ipv4&&) noexcept = default;

        template <istl::StringViewifiable StrT>
        constexpr ipv4& operator=(StrT&& ip_addr) noexcept {
            parse(stl::forward<StrT>(ip_addr));
            _prefix = prefix_status(inet_pton4_status::valid);
            return *this;
        }

        constexpr ipv4& operator=(stl::uint32_t ip_addr) noexcept {
            data    = ip_addr;
            _prefix = prefix_status(inet_pton4_status::valid);
            return *this;
        }

        constexpr stl::strong_ordering operator<=>(ipv4 const&) const noexcept = default;
        constexpr stl::strong_ordering operator<=>(ipv4_octets other) const noexcept {
            return data <=> parse(other);
        }

        constexpr stl::strong_ordering operator<=>(stl::uint32_t const& ip_addr) const noexcept {
            return data <=> ip_addr;
        }

        template <istl::StringViewifiable StrT>
        constexpr stl::strong_ordering operator<=>(StrT&& ip_addr) const noexcept {
            // only compare the octets not the prefix
            return ipv4(istl::string_viewify<stl::string_view>(stl::forward<StrT>(ip_addr))).data <=> data;
        }

        template <istl::StringViewifiable StrT>
        [[nodiscard]] constexpr bool operator==(StrT&& ip_addr) const noexcept {
            // only compare the octets not the prefix
            return ipv4(istl::string_viewify<stl::string_view>(stl::forward<StrT>(ip_addr))).data == data;
        }

        [[nodiscard]] constexpr bool operator==(ipv4 ip_addr) const noexcept {
            return _prefix == ip_addr._prefix && data == ip_addr.data;
        }

        [[nodiscard]] constexpr bool operator==(stl::uint32_t ip_addr) const noexcept {
            return data == ip_addr;
        }

        [[nodiscard]] constexpr bool operator==(ipv4_octets ip_addr) const noexcept {
            return data == parse(ip_addr);
        }

        /**
         * @brief get string representation of the ip
         */
        template <typename StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr auto string(Args&&... args) const {
            StrT str{stl::forward<Args>(args)...};
#ifdef __cpp_lib_string_resize_and_overwrite
            str.resize_and_overwrite(max_ipv4_str_len, [this](auto* buf, stl::size_t) constexpr noexcept {
                auto const _octets = octets();
                return static_cast<stl::size_t>(inet_ntop4(_octets.data(), buf) - buf);
            });
#else
            to_string(str);
#endif
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
            return ipv4_octets({static_cast<ipv4_octet>(_data >> 24U),
                                static_cast<ipv4_octet>(_data >> 16U & 0x0FFU),
                                static_cast<ipv4_octet>(_data >> 8U & 0x0FFU),
                                static_cast<ipv4_octet>(_data & 0x0FFU)});
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
            _prefix = prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
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
            return _prefix <= ipv4_max_prefix;
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
         * @param ip_addr
         * @param prefix
         * @return bool
         */
        [[nodiscard]] constexpr bool is_in_subnet(ipv4 const& ip_addr) const noexcept {
            auto uint_val = integer();
            auto uint_ip  = ip_addr.integer();
            uint_val &= 0xFF'FF'FF'FFU << static_cast<stl::uint32_t>(ipv4_max_prefix - ip_addr.prefix());
            uint_ip &= 0xFF'FF'FF'FFU << static_cast<stl::uint32_t>(ipv4_max_prefix - ip_addr.prefix());
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
            return 0xFFFFFFFFU == integer();
        }

        /**
         * Return true if the IP address is a special purpose address, as defined per
         * RFC 6890 (i.e. 0.0.0.0 or 255.255.255.255).
         */
        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            auto const ip_addr = integer();
            return is_private() || (ip_addr <= 0x00FFFFFFU) ||           // 0.0.0.0      - 0.255.255.255
                   (ip_addr >= 0xC0000000U && ip_addr <= 0xC00000FFU) || // 192.0.0.0    - 192.0.0.255
                   (ip_addr >= 0xC0000200U && ip_addr <= 0xC00002FFU) || // 192.0.2.0    - 192.0.2.255
                   (ip_addr >= 0xC6120000U && ip_addr <= 0xC613FFFFU) || // 198.18.0.0   - 198.19.255.255
                   (ip_addr >= 0xC6336400U && ip_addr <= 0xC63364FFU) || // 198.51.100.0 - 198.51.100.255
                   (ip_addr >= 0xCB007100U && ip_addr <= 0xCB0071FFU) || // 203.0.113.0  - 203.0.113.255
                   (ip_addr >= 0xE0000000U && ip_addr <= 0xFFFFFFFFU);   // 224.0.0.0    - 255.255.255.255
        }


        /**
         * Return true if the IP address is a multicast address
         */
        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return (integer() & 0xf0000000U) == 0xe0000000U;
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
            return _prefix <= ipv4_max_prefix || _prefix == prefix_status(inet_pton4_status::valid);
        }

        /**
         * Get the ip in reversed order; the prefix is unchanged
         */
        [[nodiscard]] constexpr ipv4 reversed() const noexcept {
            return {static_cast<ipv4_octet>(data & 0xFFU),
                    static_cast<ipv4_octet>(data >> 8U & 0xFFU),
                    static_cast<ipv4_octet>(data >> 16U & 0xFFU),
                    static_cast<ipv4_octet>(data >> 24U & 0xFFU),
                    _prefix};
        }

        /**
         * Creates an ipv4 instance with all but most significant num_bits set to 0.
         *
         * @param [in] num_bits number of bits to mask
         * @return ipv6 instance with bits set to 0
         */
        [[nodiscard]] constexpr ipv4 mask(stl::size_t num_bits) const noexcept {
            num_bits                     = stl::min<stl::size_t>(num_bits, ipv4_max_prefix);
            stl::uint32_t const fragment = ~0U << (ipv4_max_prefix - num_bits);
            return ipv4{data & fragment};
        }

        /**
         * Check if the specified ipv6 binary starts with the specified inp_octets up to inp_prefix bits.
         */
        template <stl::size_t N>
            requires(N <= ipv4_byte_count)
        [[nodiscard]] constexpr bool starts_with(stl::array<stl::uint8_t, N> inp_octets,
                                                 stl::size_t                 inp_prefix) const noexcept {
            if constexpr (N == 0) {
                return is_zero();
            } else if constexpr (N == 1) {
                return starts_with({inp_octets[0], 0, 0, 0}, inp_prefix);
            } else if constexpr (N == 2) {
                return starts_with({inp_octets[0], inp_octets[1], 0, 0}, inp_prefix);
            } else if constexpr (N == 3) {
                return starts_with({inp_octets[0], inp_octets[1], inp_octets[2], 0}, inp_prefix);
            } else if constexpr (N == 4) {
                return starts_with({inp_octets[0], inp_octets[1], inp_octets[2], inp_octets[3]}, inp_prefix);
            }
        }


        [[nodiscard]] constexpr bool starts_with(ipv4 const& ip_addr, stl::size_t inp_prefix) const noexcept {
            return mask(inp_prefix).data == ip_addr.mask(inp_prefix).data;
        }


        // Get the parsing result
        [[nodiscard]] constexpr inet_pton4_status status() const noexcept {
            if (_prefix <= ipv4_max_prefix) {
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
// NOLINTEND(*-magic-numbers)


#endif // WEBPP_IP_IPV4_HPP
