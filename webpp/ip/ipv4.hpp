#ifndef WEBPP_IP_IPV4_HPP
#define WEBPP_IP_IPV4_HPP

#include "../convert/casts.hpp"
#include "../socket/host_bytes.hpp"
#include "../strings/append.hpp"
#include "../strings/to_case.hpp"
#include "inet_ntop.hpp"
#include "inet_pton.hpp"
#include "ip_validators.hpp"

#include <array>
#include <compare>
#include <concepts>
#include <string_view>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp {

    using ipv4_octet  = stl::uint8_t;
    using ipv4_octets = stl::array<ipv4_octet, ipv4_byte_count>;

    /**
     * @brief considers this ip as a subnet and converts it into an int prefix
     */
    constexpr ipv4_octet to_prefix(stl::uint32_t const octets) noexcept {
        ipv4_octet prefix = 0U;
        for (stl::uint32_t mask = 0x8000'0000U; mask != 0U; mask >>= 1U) {
            if ((octets & mask) == mask) {
                prefix++;
            } else {
                return prefix;
            }
        }
        return prefix;
    }

    constexpr ipv4_octet to_prefix(ipv4_octets const octets) noexcept {
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
     * @param str
     */
    template <typename CharT>
    constexpr ipv4_octet to_prefix(stl::basic_string_view<CharT> const str) noexcept {
        ipv4_octets bin; // NOLINT(*-pro-type-member-init)
        auto        beg = str.begin();
        if (inet_pton4(beg, str.end(), bin.data()) == inet_pton4_status::valid) {
            return to_prefix(bin);
        }
        return 0U;
    }

    constexpr ipv4_octet to_prefix(stl::string_view const str) noexcept {
        return to_prefix<char>(str);
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     */
    constexpr stl::uint32_t to_subnet(ipv4_octet const prefix) noexcept {
        return 0xFFFF'FFFFU << static_cast<stl::uint32_t>(ipv4_max_prefix - prefix);
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     */
    constexpr ipv4_octets to_subnet_array(ipv4_octet const prefix) noexcept {
        auto const subnet = to_subnet(prefix);
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

    template <bool WithPrefix = true>
    struct [[nodiscard]] basic_ipv4 {
        using octets_t = ipv4_octets;

      private:
        using ip_prefix_type = details::ip_prefix_t<inet_pton4_status, WithPrefix>;

        stl::uint32_t                        data    = 0U; // all bits are used
        [[no_unique_address]] ip_prefix_type _prefix = prefix_status(inet_pton4_status::valid);

        template <typename Iter>
            requires WithPrefix
        constexpr void parse(Iter beg, Iter endptr) noexcept {
            // make sure prefix is set to mark the ip to be valid:
            _prefix = prefix_status(inet_pton4_status::valid);

            ipv4_octets bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
            auto const  res = inet_pton4(beg, endptr, bin.data(), _prefix);
            if (webpp::is_valid(res)) {
                data = parse(bin);
            } else {
                // set the error
                _prefix = prefix_status(res);
            }
        }

        template <typename Iter>
            requires(!WithPrefix)
        constexpr stl::uint8_t parse(Iter beg, Iter endptr) noexcept {
            stl::uint8_t cur_prefix = prefix_status(inet_pton4_status::valid);
            ipv4_octets  bin; // NOLINT(cppcoreguidelines-pro-type-member-init)
            auto const   res = inet_pton4(beg, endptr, bin.data(), cur_prefix);
            if (webpp::is_valid(res)) {
                data = parse(bin);
            }
            return cur_prefix;
        }

        [[nodiscard]] static constexpr stl::uint32_t parse(ipv4_octets const ip_addr) noexcept {
            return static_cast<stl::uint32_t>(ip_addr[0] << 24U) | static_cast<stl::uint32_t>(ip_addr[1] << 16U) |
                   static_cast<stl::uint32_t>(ip_addr[2] << 8U) | static_cast<stl::uint32_t>(ip_addr[3]);
        }

        template <typename CharT>
        constexpr decltype(auto) parse(stl::basic_string_view<CharT> const ip_addr) noexcept {
            return parse(ip_addr.begin(), ip_addr.end());
        }

      public:
        // Create an ipv4 at compile-time; a simple consteval constructor helper
        template <typename... Args>
        static consteval basic_ipv4 create(Args&&... args) noexcept {
            return basic_ipv4{stl::forward<Args>(args)...};
        }

        static consteval basic_ipv4 invalid() noexcept {
            basic_ipv4 ip_addr;
            ip_addr._prefix = prefix_status(inet_pton4_status::invalid_prefix);
            return ip_addr;
        }

        // 0.0.0.0
        static consteval basic_ipv4 any() noexcept {
            return basic_ipv4{0x0000'0000U};
        }

        // 127.0.0.1
        static consteval basic_ipv4 loopback() noexcept {
            return basic_ipv4{0x7F00'0001U};
        }

        // 255.255.255.255
        static consteval basic_ipv4 broadcast() noexcept {
            return basic_ipv4{0xFFFF'FFFFU};
        }

        // initialize with 0.0.0.0
        constexpr basic_ipv4() noexcept = default;

        template <typename CharT>
        constexpr explicit basic_ipv4(stl::basic_string_view<CharT> const ip_addr) noexcept {
            parse(ip_addr);
        }

        // specialization to force ipv4::create("...") work
        constexpr explicit basic_ipv4(stl::string_view const ip_addr) noexcept {
            parse(ip_addr);
        }

        template <stl::random_access_iterator IterT>
            requires(!istl::StringViewifiable<IterT>)
        constexpr explicit basic_ipv4(IterT startp, IterT endp) noexcept {
            parse(startp, endp);
        }

        template <typename CharT>
            requires WithPrefix
        constexpr basic_ipv4(stl::basic_string_view<CharT> const ip_addr,
                             stl::basic_string_view<CharT> const subnet) noexcept {
            parse(ip_addr);
            if (is_valid()) {
                _prefix = (is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix));
            }
        }

        template <typename CharT>
            requires WithPrefix
        constexpr basic_ipv4(stl::basic_string_view<CharT> const ip_addr, ipv4_octets const subnet) noexcept {
            parse(ip_addr);
            if (is_valid()) {
                _prefix = is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix);
            }
        }

        template <typename CharT>
            requires WithPrefix
        constexpr basic_ipv4(stl::basic_string_view<CharT> const ip_addr, ipv4_octet const prefix_val) noexcept {
            parse(ip_addr);
            if (is_valid()) {
                _prefix = prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
                            ? prefix_status(inet_pton4_status::invalid_prefix)
                            : prefix_val;
            }
        }

        // NOLINTBEGIN(bugprone-easily-swappable-parameters)
        constexpr basic_ipv4(
          ipv4_octet const octet1,
          ipv4_octet const octet2,
          ipv4_octet const octet3,
          ipv4_octet const octet4,
          ipv4_octet const prefix_val = prefix_status(inet_pton4_status::valid)) noexcept
            requires(WithPrefix)
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix_val) {}

        template <typename CharT>
        constexpr basic_ipv4(
          ipv4_octet const                    octet1,
          ipv4_octet const                    octet2,
          ipv4_octet const                    octet3,
          ipv4_octet const                    octet4,
          stl::basic_string_view<CharT> const subnet) noexcept
            requires(WithPrefix)
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr basic_ipv4(ipv4_octet const octet1,
                             ipv4_octet const octet2,
                             ipv4_octet const octet3,
                             ipv4_octet const octet4) noexcept
            requires(!WithPrefix)
          : data(parse({octet1, octet2, octet3, octet4})) {}

        constexpr explicit basic_ipv4(stl::uint32_t const ip_addr) noexcept
            requires(!WithPrefix)
          : data(ip_addr) {}

        constexpr explicit basic_ipv4(stl::uint32_t const ip_addr,
                                      ipv4_octet const    prefix = prefix_status(inet_pton4_status::valid)) noexcept
            requires(WithPrefix)
          : data(ip_addr),
            _prefix(prefix > ipv4_max_prefix && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        template <typename CharT>
            requires(WithPrefix)
        constexpr explicit basic_ipv4(stl::uint32_t const ip_addr, stl::basic_string_view<CharT> const subnet) noexcept
          : data(ip_addr),
            _prefix(is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr explicit basic_ipv4(ipv4_octets const ip_addr,
                                      ipv4_octet const  prefix = prefix_status(inet_pton4_status::valid)) noexcept
            requires(WithPrefix)
          : data(parse(ip_addr)),
            _prefix(prefix > ipv4_max_prefix && prefix != prefix_status(inet_pton4_status::valid)
                      ? prefix_status(inet_pton4_status::invalid_prefix)
                      : prefix) {}

        constexpr explicit basic_ipv4(ipv4_octets const ip_addr) noexcept
            requires(!WithPrefix)
          : data(parse(ip_addr)) {}

        template <typename CharT>
        constexpr basic_ipv4(ipv4_octets const ip_addr, stl::basic_string_view<CharT> const subnet) noexcept
            requires(WithPrefix)
          : data(parse(ip_addr)),
            _prefix(is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix)) {}

        constexpr basic_ipv4(ipv4_octets const ip_addr, ipv4_octets const subnet) noexcept
            requires(WithPrefix)
          : data(parse(ip_addr)),
            _prefix(is::subnet(subnet) ? to_prefix(subnet) : prefix_status(inet_pton4_status::invalid_prefix)) {}

        // NOLINTEND(bugprone-easily-swappable-parameters)

        constexpr explicit operator stl::uint32_t() const noexcept {
            return integer();
        }

        constexpr basic_ipv4(basic_ipv4 const&)                = default;
        constexpr basic_ipv4(basic_ipv4&&)                     = default;
        constexpr ~basic_ipv4() noexcept                       = default;
        constexpr basic_ipv4& operator=(basic_ipv4 const&)     = default;
        constexpr basic_ipv4& operator=(basic_ipv4&&) noexcept = default;

        template <typename CharT>
        constexpr basic_ipv4& operator=(stl::basic_string_view<CharT> const ip_addr) noexcept {
            parse(ip_addr);
            return *this;
        }

        constexpr basic_ipv4& operator=(stl::uint32_t const ip_addr) noexcept {
            data    = ip_addr;
            _prefix = prefix_status(inet_pton4_status::valid);
            return *this;
        }

        constexpr stl::strong_ordering operator<=>(basic_ipv4 const&) const noexcept = default;

        constexpr stl::strong_ordering operator<=>(ipv4_octets const other) const noexcept {
            return data <=> parse(other);
        }

        constexpr stl::strong_ordering operator<=>(stl::uint32_t const& ip_addr) const noexcept {
            return data <=> ip_addr;
        }

        template <typename CharT>
        constexpr stl::strong_ordering operator<=>(stl::basic_string_view<CharT> const ip_addr) const noexcept {
            // only compare the octets not the prefix
            return basic_ipv4{ip_addr}.data <=> data;
        }

        constexpr stl::strong_ordering operator<=>(stl::string_view const ip_addr) const noexcept {
            // only compare the octets not the prefix
            return basic_ipv4{ip_addr}.data <=> data;
        }

        template <typename CharT>
        [[nodiscard]] constexpr bool operator==(stl::basic_string_view<CharT> const ip_addr) const noexcept {
            // only compare the octets not the prefix
            return basic_ipv4{ip_addr}.data == data;
        }

        [[nodiscard]] constexpr bool operator==(stl::string_view const ip_addr) const noexcept {
            // only compare the octets not the prefix
            return basic_ipv4{ip_addr}.data == data;
        }

        [[nodiscard]] constexpr bool operator==(basic_ipv4 const ip_addr) const noexcept {
            return _prefix == ip_addr._prefix && data == ip_addr.data;
        }

        [[nodiscard]] constexpr bool operator==(stl::uint32_t const ip_addr) const noexcept {
            return data == ip_addr;
        }

        [[nodiscard]] constexpr bool operator==(ipv4_octets const ip_addr) const noexcept {
            return data == parse(ip_addr);
        }

        [[nodiscard]] constexpr bool operator==(ipv4_octet const* const ip_addr) const noexcept {
            // NOLINTNEXTLINE(*-pro-bounds-pointer-arithmetic)
            return *this == ipv4_octets{ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]};
        }

        /**
         * @brief get string representation of the ip
         */
        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr auto string(Args&&... args) const {
            using char_type = istl::char_type_of_t<StrT>;
            StrT str{stl::forward<Args>(args)...};
            istl::resize_and_overwrite(str, max_ipv4_str_len, [this](char_type* buf, stl::size_t) constexpr noexcept {
                auto const _octets = octets();
                return static_cast<stl::size_t>(inet_ntop4(_octets.data(), buf) - buf);
            });
            return str;
        }

        constexpr void to_string(istl::String auto& out) const {
            resize_and_append(out, max_ipv4_str_len, [this](auto* buf) constexpr noexcept {
                auto const _octets = octets();
                return inet_ntop4(_octets.data(), buf);
            });
        }

        template <typename Iter>
        [[nodiscard]] constexpr bool str_equal(Iter pos, Iter const end) const noexcept {
            auto const _octets = octets();
            return inet_ntop4_streq(_octets.data(), pos, end);
        }

        [[nodiscard]] constexpr bool str_equal(stl::string_view ip_str) const noexcept {
            return str_equal(ip_str.begin(), ip_str.end());
        }

        // Get the string size
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return inet_ntop4_size(octets().data());
        }

        /**
         * @brief get the integer representation of the ip address
         * @return
         */
        [[nodiscard]] constexpr stl::uint32_t integer() const noexcept {
            return data;
        }

        /**
         * Get the big-endian encoded integer value of the IP
         */
        [[nodiscard]] constexpr stl::uint32_t net_integer() const noexcept {
            return hton<stl::uint32_t>(data);
        }

        /**
         * @brief get the 4 octets of the ip address
         * @return
         */
        [[nodiscard]] constexpr ipv4_octets octets() const noexcept {
            auto const  val = net_integer();
            ipv4_octets out;
            stl::memcpy(out.data(), &val, sizeof(val));
            return out;
        }

        /**
         * @brief check if the ip is in the specified range or not
         * @param start
         * @param finish
         * @return
         */
        [[nodiscard]] constexpr bool in_range(basic_ipv4 const& start, basic_ipv4 const& finish) const noexcept {
            return *this >= start && *this <= finish;
        }

        /**
         * Get the prefix you specified in the constructor
         * @return
         */
        [[nodiscard]] constexpr auto prefix() const noexcept
            requires(WithPrefix)
        {
            return _prefix;
        }

        /**
         * Change the prefix of the ip
         * @param prefix_val
         */
        constexpr basic_ipv4& prefix(ipv4_octet const prefix_val) noexcept
            requires(WithPrefix)
        {
            _prefix = prefix_val > ipv4_max_prefix && prefix_val != prefix_status(inet_pton4_status::valid)
                        ? prefix_status(inet_pton4_status::invalid_prefix)
                        : prefix_val;
            return *this;
        }

        /**
         * Set prefix with a subnet string
         * @param subnet
         */
        template <typename CharT>
        constexpr basic_ipv4& prefix(stl::basic_string_view<CharT> const subnet) noexcept
            requires(WithPrefix)
        {
            return prefix(to_prefix(subnet));
        }

        /**
         * Set prefix with a subnet array
         * @param subnet
         */
        constexpr basic_ipv4& prefix(ipv4_octets const subnet) noexcept
            requires(WithPrefix)
        {
            return prefix(to_prefix(subnet));
        }

        /**
         * Remove prefix from the ip address
         */
        constexpr basic_ipv4& clear_prefix() noexcept
            requires(WithPrefix)
        {
            return prefix(prefix_status(inet_pton4_status::valid));
        }

        /**
         * Check if the ip contains a prefix or not
         * @return bool an indication on weather or not the ip contains a prefix or not
         */
        [[nodiscard]] constexpr bool has_prefix() const noexcept
            requires(WithPrefix)
        {
            return _prefix <= ipv4_max_prefix;
        }

        /**
         * Check if the specified subnet or prefix was valid or not
         * @return bool
         */
        [[nodiscard]] constexpr bool has_valid_prefix() const noexcept
            requires(WithPrefix)
        {
            return _prefix != prefix_status(inet_pton4_status::invalid_prefix);
        }

        /**
         * @brief checks if the ip in this class is in the specified subnet or
         * not regardless of the prefix that is specified in the ctor
         * @param ip_addr
         * @return bool
         */
        [[nodiscard]] constexpr bool is_in_subnet(basic_ipv4 const& ip_addr) const noexcept {
            auto uint_val  = integer();
            auto uint_ip   = ip_addr.integer();
            uint_val      &= 0xFFFF'FFFFU << static_cast<stl::uint32_t>(ipv4_max_prefix - ip_addr.prefix());
            uint_ip       &= 0xFFFF'FFFFU << static_cast<stl::uint32_t>(ipv4_max_prefix - ip_addr.prefix());
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
         * Returns true if the IP address qualifies as broadcast
         */
        [[nodiscard]] constexpr bool is_broadcast() const noexcept {
            return 0xFFFF'FFFFU == integer();
        }

        /**
         * Return true if the IP address is a special purpose address, as defined per
         * RFC 6890 (i.e. 0.0.0.0 or 255.255.255.255).
         */
        [[nodiscard]] constexpr bool is_nonroutable() const noexcept {
            auto const ip_addr = integer();
            return is_private() || (ip_addr <= 0x00FF'FFFFU) ||            // 0.0.0.0      - 0.255.255.255
                   (ip_addr >= 0xC000'0000U && ip_addr <= 0xC000'00FFU) || // 192.0.0.0    - 192.0.0.255
                   (ip_addr >= 0xC000'0200U && ip_addr <= 0xC000'02FFU) || // 192.0.2.0    - 192.0.2.255
                   (ip_addr >= 0xC612'0000U && ip_addr <= 0xC613'FFFFU) || // 198.18.0.0   - 198.19.255.255
                   (ip_addr >= 0xC633'6400U && ip_addr <= 0xC633'64FFU) || // 198.51.100.0 - 198.51.100.255
                   (ip_addr >= 0xCB00'7100U && ip_addr <= 0xCB00'71FFU) || // 203.0.113.0  - 203.0.113.255
                   (ip_addr >= 0xE000'0000U && ip_addr <= 0xFFFF'FFFFU);   // 224.0.0.0    - 255.255.255.255
        }

        /**
         * Return true if the IP address is a multicast address
         */
        [[nodiscard]] constexpr bool is_multicast() const noexcept {
            return (integer() & 0xf000'0000U) == 0xe000'0000U;
        }

        /**
         * @brief checks if the ip is in private range or not regardless of the prefix
         */
        [[nodiscard]] constexpr bool is_private() const noexcept {
            constexpr basic_ipv4 class_C(ipv4_octets{192, 168, 0, 0}, 16);
            constexpr basic_ipv4 class_B_start(ipv4_octets{172, 16, 0, 0});
            constexpr basic_ipv4 class_B_finish(ipv4_octets{172, 31, 255, 255});
            constexpr basic_ipv4 class_A(ipv4_octets{10, 0, 0, 0}, 8);
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
        [[nodiscard]] constexpr bool is_valid() const noexcept
            requires(WithPrefix)
        {
            return _prefix <= ipv4_max_prefix || _prefix == prefix_status(inet_pton4_status::valid);
        }

        /**
         * Get the ip in reversed order; the prefix is unchanged
         */
        [[nodiscard]] constexpr basic_ipv4 reversed() const noexcept {
            auto const oc1 = static_cast<ipv4_octet>(data & 0xFFU);
            auto const oc2 = static_cast<ipv4_octet>(data >> 8U & 0xFFU);
            auto const oc3 = static_cast<ipv4_octet>(data >> 16U & 0xFFU);
            auto const oc4 = static_cast<ipv4_octet>(data >> 24U & 0xFFU);

            if constexpr (WithPrefix) {
                return {oc1, oc2, oc3, oc4, _prefix};
            } else {
                return {oc1, oc2, oc3, oc4};
            }
        }

        /**
         * Creates an ipv4 instance with all but most significant num_bits set to 0.
         *
         * @param [in] num_bits number of bits to mask
         * @return ipv6 instance with bits set to 0
         */
        [[nodiscard]] constexpr basic_ipv4 mask(stl::size_t num_bits) const noexcept {
            num_bits                     = stl::min<stl::size_t>(num_bits, ipv4_max_prefix);
            stl::uint32_t const fragment = ~0U << (ipv4_max_prefix - num_bits);
            return basic_ipv4{data & fragment};
        }

        /**
         * Check if the specified ipv6 binary starts with the specified inp_octets up to inp_prefix bits.
         */
        template <stl::size_t N>
            requires(N <= ipv4_byte_count)
        [[nodiscard]] constexpr bool starts_with(stl::array<stl::uint8_t, N> const inp_octets,
                                                 stl::size_t const                 inp_prefix) const noexcept {
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
            } else {
                return false; // just to get rid of the warnings
            }
        }

        [[nodiscard]] constexpr bool starts_with(basic_ipv4 const& ip_addr,
                                                 stl::size_t const inp_prefix) const noexcept {
            return mask(inp_prefix).data == ip_addr.mask(inp_prefix).data;
        }

        // Get the parsing result
        [[nodiscard]] constexpr inet_pton4_status status() const noexcept
            requires(WithPrefix)
        {
            if (_prefix <= ipv4_max_prefix) {
                return inet_pton4_status::valid;
            }
            return static_cast<inet_pton4_status>(_prefix);
        }

        template <typename StrT>
        constexpr void status_to(StrT& output) const
            requires(WithPrefix)
        {
            set_string(output, webpp::to_string(status()));
        }

        template <typename StrT = stl::string_view, typename... Args>
        [[nodiscard]] constexpr auto status_string(Args&&... args) const
            requires(WithPrefix)
        {
            StrT str{stl::forward<Args>(args)...};
            status_to(str);
            return str;
        }
    };

    using ipv4      = basic_ipv4<>;      // with prefix
    using pure_ipv4 = basic_ipv4<false>; // no prefix

} // namespace webpp

// NOLINTEND(*-magic-numbers)


#endif // WEBPP_IP_IPV4_HPP
