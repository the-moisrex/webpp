#ifndef WEBPP_IP_H
#define WEBPP_IP_H

#include "../strings/ascii.hpp"
#include "../traits/traits_concepts.hpp"
#include "../validators/validators.hpp"
#include "casts.hpp"

#include <array>

namespace webpp {

    /**
     * @brief considers this ip as a subnet and converts it into a int
     * prefix
     */
    constexpr uint8_t to_prefix(uint32_t octets) noexcept {
        uint8_t prefix = 0u;
        for (uint32_t mask = 0x80'00'00'00u; mask != 0u; mask >>= 1u)
            if ((octets & mask) == mask)
                prefix++;
            else
                return prefix;
        return prefix;
    }

    constexpr uint8_t to_prefix(stl::array<uint8_t, 4> octets) noexcept {
        uint8_t prefix = 0u;
        for (auto const& octet : octets)
            for (uint8_t mask = 0b1000'0000; mask != 0u; mask >>= 1u)
                if ((octet & mask) == mask)
                    prefix++;
                else
                    return prefix;
        return prefix;
    }

    /**
     * Convert string to prefix
     * @param octets
     */
    template <Traits TraitsType>
    constexpr uint8_t to_prefix(typename TraitsType::string_view_type const& _data) noexcept {
        using traits_type = TraitsType;

        if (_data.size() > 15 || _data.size() < 7) {
            return 0u;
        }
        stl::size_t first_dot = 0u;
        stl::size_t len       = _data.size();
        while (_data[first_dot] != '.' && first_dot != len)
            first_dot++;

        auto octet_1 = _data.substr(0u, first_dot);
        if (first_dot == len || octet_1.empty() || octet_1.size() > 3 || !is::digit(octet_1) ||
            (starts_with(octet_1, '0') && octet_1 != "0")) {
            return 0u;
        }

        stl::size_t second_dot = first_dot + 1;
        while (_data[second_dot] != '.' && second_dot != len)
            second_dot++;

        auto octet_2 = _data.substr(first_dot + 1u, second_dot - (first_dot + 1));
        if (second_dot == len || octet_2.empty() || octet_2.size() > 3 || !is::digit(octet_2) ||
            (starts_with(octet_2, '0') && octet_2 != "0")) {
            return 0u;
        }

        stl::size_t third_dot = second_dot + 1;
        while (_data[third_dot] != '.' && third_dot != len)
            third_dot++;

        auto octet_3 = _data.substr(second_dot + 1u, third_dot - (second_dot + 1));
        if (first_dot == len || octet_3.empty() || octet_3.size() > 3 || !is::digit(octet_3) ||
            (starts_with(octet_3, '0') && octet_3 != "0")) {
            return 0u; // parsing failed.
        }

        auto octet_4 = _data.substr(third_dot + 1u);

        if (octet_4.empty() || octet_4.size() > 3 || !is::digit(octet_4) ||
            (starts_with(octet_4, '0') && octet_4 != "0")) {
            return 0u;
        }

        return to_prefix({to_uint8(octet_1), to_uint8(octet_2), to_uint8(octet_3), to_uint8(octet_4)});
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr uint32_t to_subnet(uint8_t prefix) noexcept {
        return 0xFF'FF'FF'FFu << (32u - prefix);
    }

    /**
     * Convert a prefix to a subnet
     * @param prefix
     * @return bool
     */
    constexpr stl::array<uint8_t, 4> to_subnet_array(uint8_t prefix) noexcept {
        auto subnet = to_subnet(prefix);
        return {static_cast<uint8_t>(subnet >> 24u & 0xFFu), static_cast<uint8_t>(subnet >> 16u & 0xFFu),
                static_cast<uint8_t>(subnet >> 8u & 0xFFu), static_cast<uint8_t>(subnet & 0xFFu)};
    }

    template <Traits TraitsType>
    struct ipv4 {
        using traits_type      = TraitsType;
        using string_view_type = typename traits_type::string_view_type;
        using string_type      = typename traits_type::string_type;
        using char_type        = typename traits_type::char_type;

      private:
        mutable uint32_t data = 0u; // all bits are used

        // 255 means that the ip doesn't have a prefix
        // 254 means the ip is not valid
        // 253 means the prefix was not valid
        mutable uint8_t _prefix = 255u;

        constexpr void parse(string_view_type const& _data) const noexcept {
            if (_data.size() > 15 || _data.size() < 7) {
                _prefix = 254u; // the ip is not valid
                return;
            }
            stl::size_t first_dot = 0u;
            stl::size_t len       = _data.size();
            while (_data[first_dot] != '.' && first_dot != len)
                first_dot++;

            auto octet_1 = _data.substr(0u, first_dot);
            if (first_dot == len || octet_1.empty() || octet_1.size() > 3 || !is::digit(octet_1) ||
                (starts_with(octet_1, '0') && octet_1 != "0")) {
                _prefix = 254u; // the ip is not valid
                return;
            }

            stl::size_t second_dot = first_dot + 1;
            while (_data[second_dot] != '.' && second_dot != len)
                second_dot++;

            auto octet_2 = _data.substr(first_dot + 1u, second_dot - (first_dot + 1));
            if (second_dot == len || octet_2.empty() || octet_2.size() > 3 || !is::digit(octet_2) ||
                (starts_with(octet_2, '0') && octet_2 != "0")) {
                _prefix = 254u; // the ip is not valid
                return;
            }

            stl::size_t third_dot = second_dot + 1;
            while (_data[third_dot] != '.' && third_dot != len)
                third_dot++;

            auto octet_3 = _data.substr(second_dot + 1u, third_dot - (second_dot + 1));
            if (third_dot == len || octet_3.empty() || octet_3.size() > 3 || !is::digit(octet_3) ||
                (starts_with(octet_3, '0') && octet_3 != "0")) {
                _prefix = 254u; // the ip is not valid
                return;         // parsing failed.
            }

            stl::size_t slash = third_dot + 1;
            while (slash != len && _data[slash] != '/')
                slash++;

            auto octet_4 = _data.substr(third_dot + 1u, slash - (third_dot + 1));

            if (octet_4.empty() || octet_4.size() > 3 || !is::digit(octet_4) ||
                (starts_with(octet_4, '0') && octet_4 != "0")) {
                _prefix = 254u; // the ip is not valid
                return;
            }

            if (slash != len) {
                auto prefix_str = _data.substr(slash + 1);
                if (prefix_str.empty() || (starts_with(prefix_str, '0') && prefix_str != "0") ||
                    !is::digit(prefix_str)) {
                    _prefix = 254u; // the ip is not valid
                    return;
                }
                auto __prefix = to_uint(prefix_str);
                if (__prefix > 32) {
                    _prefix = 254; // the ip is not valid
                    return;
                }
                _prefix = static_cast<uint8_t>(__prefix);
            }

            auto oc1 = to_uint(octet_1);
            auto oc2 = to_uint(octet_2);
            auto oc3 = to_uint(octet_3);
            auto oc4 = to_uint(octet_4);

            if (oc1 > 255 || oc2 > 255 || oc3 > 255 || oc4 > 255) {
                _prefix = 254u; // the ip is not valid
                return;
            }

            data = parse({static_cast<uint8_t>(oc1), static_cast<uint8_t>(oc2), static_cast<uint8_t>(oc3),
                          static_cast<uint8_t>(oc4)});

            if (_prefix == 254u)
                _prefix = 255u; // the ip is valid
        }

        constexpr uint32_t parse(stl::array<uint8_t, 4u> const& ip) const noexcept {
            return static_cast<uint32_t>(ip[0] << 24u) | static_cast<uint32_t>(ip[1] << 16u) |
                   static_cast<uint32_t>(ip[2] << 8u) | static_cast<uint32_t>(ip[3]);
        }

      public:
        constexpr ipv4(ipv4 const& ip) = default;

        constexpr ipv4(ipv4&& ip) = default;

        constexpr explicit ipv4(string_view_type const& ip) noexcept : _prefix(255) {
            parse(ip);
        }

        constexpr explicit ipv4(char_type const* const ip) noexcept : _prefix(255) {
            parse(ip);
        }

        constexpr ipv4(string_view_type const& ip, string_view_type const& subnet) noexcept
          : _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {
            parse(ip);
        }

        constexpr ipv4(string_view_type const& ip, stl::array<uint8_t, 4> const& subnet) noexcept
          : _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {
            parse(ip);
        }

        constexpr ipv4(string_view_type const& ip, uint8_t __prefix) noexcept
          : _prefix(__prefix > 32 && __prefix != 255u ? 253u : __prefix) {
            parse(ip);
        }

        constexpr ipv4(uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4,
                       uint8_t __prefix = 255) noexcept
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(__prefix > 32 && __prefix != 255u ? 253u : __prefix) {}

        constexpr ipv4(uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4,
                       stl::string_view const& subnet) noexcept
          : data(parse({octet1, octet2, octet3, octet4})),
            _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {}

        constexpr explicit ipv4(uint32_t const& ip, uint8_t prefix = 255) noexcept
          : data(ip),
            _prefix(prefix > 32 && prefix != 255u ? 253u : prefix) {}

        constexpr explicit ipv4(uint32_t const& ip, string_view_type subnet) noexcept
          : data(ip),
            _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {}

        constexpr ipv4(stl::array<uint8_t, 4> const& ip, uint8_t prefix = 255) noexcept
          : data(parse(ip)),
            _prefix(prefix > 32 && prefix != 255u ? 253u : prefix) {}

        constexpr ipv4(stl::array<uint8_t, 4> const& ip, string_view_type const& subnet) noexcept
          : data(parse(ip)),
            _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {}

        constexpr ipv4(stl::array<uint8_t, 4> const& ip, stl::array<uint8_t, 4> const& subnet) noexcept
          : data(parse(ip)),
            _prefix(is::subnet<traits_type>(subnet) ? to_prefix<traits_type>(subnet) : 253u) {}

        explicit operator string_type() {
            return str();
        }

        explicit operator const stl::enable_if_t<stl::is_integral_v<char_type>, void>*() {
            return str().c_str();
        }

        explicit operator uint32_t() {
            return integer();
        }

        ipv4& operator=(ipv4 const& ip) = default;
        ipv4& operator=(ipv4&& ip) = default;

        ipv4& operator=(string_view_type const& ip) noexcept {
            parse(ip);
            _prefix = 255u;
            return *this;
        }

        ipv4& operator=(uint32_t ip) noexcept {
            data    = ip;
            _prefix = 255u;
            return *this;
        }

        constexpr bool operator==(stl::array<uint8_t, 4> const& other) const noexcept {
            return data == parse(other);
        }

        constexpr bool operator!=(stl::array<uint8_t, 4> const& other) const noexcept {
            return data != parse(other);
        }

        constexpr bool operator<(stl::array<uint8_t, 4> const& other) const noexcept {
            return data < parse(other);
        }

        constexpr bool operator>(stl::array<uint8_t, 4> const& other) const noexcept {
            return data > parse(other);
        }

        constexpr bool operator<=(stl::array<uint8_t, 4> const& other) const noexcept {
            return data <= parse(other);
        }

        constexpr bool operator>=(stl::array<uint8_t, 4> const& other) const noexcept {
            return data >= parse(other);
        }

        constexpr bool operator==(ipv4 const& other) const noexcept {
            return data == other.data && _prefix == other._prefix;
        }

        constexpr bool operator!=(ipv4 const& other) const noexcept {
            return !operator==(other);
        }

        constexpr bool operator<(ipv4 const& other) const noexcept {
            return data < other.data;
        }

        constexpr bool operator>(ipv4 const& other) const noexcept {
            return data > other.data;
        }

        constexpr bool operator>=(ipv4 const& other) const noexcept {
            return data >= other.data;
        }

        constexpr bool operator<=(ipv4 const& other) const noexcept {
            return data <= other.data;
        }

        constexpr bool operator!=(string_view_type const& ip) const noexcept {
            return operator!=(ipv4<traits_type>(ip));
        }

        constexpr bool operator==(string_view_type const& ip) const noexcept {
            return operator==(ipv4<traits_type>(ip));
        }

        constexpr bool operator<(string_view_type const& ip) const noexcept {
            return operator<(ipv4<traits_type>(ip));
        }

        constexpr bool operator>(string_view_type const& ip) const noexcept {
            return operator>(ipv4<traits_type>(ip));
        }

        constexpr bool operator<=(string_view_type const& ip) const noexcept {
            return operator<=(ipv4<traits_type>(ip));
        }

        constexpr bool operator>=(string_view_type const& ip) const noexcept {
            return operator>=(ipv4<traits_type>(ip));
        }

        constexpr bool operator==(uint32_t const& ip) const noexcept {
            return integer() == ip;
        }

        constexpr bool operator!=(uint32_t const& ip) const noexcept {
            return !operator==(ip);
        }

        constexpr bool operator<(uint32_t const& ip) const noexcept {
            return integer() < ip;
        }

        constexpr bool operator>(uint32_t const& ip) const noexcept {
            return integer() > ip;
        }

        constexpr bool operator<=(uint32_t const& ip) const noexcept {
            return integer() <= ip;
        }

        constexpr bool operator>=(uint32_t const& ip) const noexcept {
            return integer() >= ip;
        }

        friend stl::basic_ostream<char_type>& operator<<(stl::basic_ostream<char_type>& stream,
                                                         ipv4<traits_type> const&       ip) {
            stream << ip.str();
            return stream;
        }

        friend stl::basic_istream<char_type>& operator>>(stl::basic_istream<char_type>& stream,
                                                         ipv4<traits_type>&             ip) {
            stl::basic_string<char_type> str;
            stream >> str;
            ip = str;
            return stream;
        }

        /**
         * @brief get string representation of the ip
         * @return
         */
        [[nodiscard]] string_type str() const noexcept {
            auto _octets = octets();
            // todo: use lib{fmt}
            stl::basic_ostringstream<char_type> s;
            s << static_cast<unsigned int>(_octets[0]) << '.' << static_cast<unsigned int>(_octets[1]) << '.'
              << static_cast<unsigned int>(_octets[2]) << '.' << static_cast<unsigned int>(_octets[3]);
            return s.str();
        }

        /**
         * @brief get the integer representation of the ip address
         * @return
         */
        [[nodiscard]] constexpr uint32_t integer() const noexcept {
            return data;
        }

        /**
         * @brief get the 4 octets of the ip address
         * @return
         */
        [[nodiscard]] constexpr stl::array<uint8_t, 4u> octets() const noexcept {
            uint32_t _data = integer();
            return stl::array<uint8_t, 4u>(
              {static_cast<uint8_t>(_data >> 24u), static_cast<uint8_t>(_data >> 16u & 0x0FFu),
               static_cast<uint8_t>(_data >> 8u & 0x0FFu), static_cast<uint8_t>(_data & 0x0FFu)});
        }

        /**
         * @brief check if the ip is in the specified range or not
         * @param start
         * @param finish
         * @return
         */
        [[nodiscard]] constexpr bool in_range(ipv4<traits_type> const& start,
                                              ipv4<traits_type> const& finish) const noexcept {
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
         * @param __prefix
         */
        ipv4<traits_type>& prefix(uint8_t __prefix) noexcept {
            _prefix = __prefix > 32 && __prefix != 255u ? 253u : __prefix;
            return *this;
        }

        /**
         * Set prefix with a subnet string
         * @param _subnet
         */
        ipv4<traits_type>& prefix(string_view_type const& _subnet) noexcept {
            return prefix(to_prefix<traits_type>(_subnet));
        }

        /**
         * Set prefix with a subnet array
         * @param _subnet
         */
        ipv4<traits_type>& prefix(stl::array<uint8_t, 4> const& _subnet) noexcept {
            return prefix(to_prefix<traits_type>(_subnet));
        }

        /**
         * Remove prefix from the ip address
         */
        ipv4& clear_prefix() noexcept {
            return prefix(255u);
        }

        /**
         * Check if the ip contains a prefix or not
         * @return bool an indication on weather or not the ip contains a prefix
         * or not
         */
        [[nodiscard]] constexpr bool has_prefix() const noexcept {
            return _prefix <= 32;
        }

        /**
         * Check if the specified subnet or prefix was valid or not
         * @return bool
         */
        [[nodiscard]] constexpr bool has_valid_prefix() const noexcept {
            return _prefix != 253u;
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
         * @brief checks if the ip is in private range or not regardless of the
         * prefix
         * @return
         */
        [[nodiscard]] constexpr bool is_private() const noexcept {
            constexpr ipv4<traits_type> class_C(stl::array<uint8_t, 4u>{192, 168, 0, 0}, 16);
            constexpr ipv4<traits_type> class_B_start(stl::array<uint8_t, 4u>{172, 16, 0, 0});
            constexpr ipv4              class_B_finish(stl::array<uint8_t, 4u>{172, 31, 255, 255});
            constexpr ipv4<traits_type> class_A(stl::array<uint8_t, 4u>{10, 0, 0, 0}, 8);
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
            return _prefix != 254u;
        }

        /**
         * Get the ip in reversed order
         * @return
         */
        [[nodiscard]] constexpr ipv4 reversed() const noexcept {
            return stl::array<uint8_t, 4>{
              static_cast<uint8_t>(data & 0xFFu), static_cast<uint8_t>(data >> 8u & 0xFFu),
              static_cast<uint8_t>(data >> 16u & 0xFFu), static_cast<uint8_t>(data >> 24u & 0xFFu)};
        }

        /**
         * TODO: implement this thing
         * @brief get the geographical location of the ip address based on
         * predefined rules
         * @return coordinates or string location
         */
        // [[nodiscard]] string_type geographic_location() const noexcept;
    };

    template <Traits TraitsType>
    constexpr bool operator==(uint32_t const& one, ipv4<TraitsType> const& two) {
        return two == one;
    }

    template <Traits TraitsType>
    constexpr bool operator!=(uint32_t const& one, ipv4<TraitsType> const& two) {
        return two == one;
    }

    template <Traits TraitsType>
    constexpr bool operator<(uint32_t const& one, ipv4<TraitsType> const& two) {
        return one < two.integer();
    }

    template <Traits TraitsType>
    constexpr bool operator>(uint32_t const& one, ipv4<TraitsType> const& two) {
        return one > two.integer();
    }

    template <Traits TraitsType>
    constexpr bool operator<=(uint32_t const& one, ipv4<TraitsType> const& two) {
        return one <= two.integer();
    }

    template <Traits TraitsType>
    constexpr bool operator>=(uint32_t const& one, ipv4<TraitsType> const& two) {
        return one >= two.integer();
    }

    template <Traits TraitsType>
    constexpr bool operator==(typename TraitsType::string_view_type const& one, ipv4<TraitsType> const& two) {
        return two == one;
    }

    template <Traits TraitsType>
    constexpr bool operator!=(typename TraitsType::string_view_type const& one, ipv4<TraitsType> const& two) {
        return two != one;
    }

    template <Traits TraitsType>
    constexpr bool operator<(typename TraitsType::string_view_type const& one, ipv4<TraitsType> const& two) {
        return ipv4(one) < two;
    }

    template <Traits TraitsType>
    constexpr bool operator>(typename TraitsType::string_view_type const& one, ipv4<TraitsType> const& two) {
        return ipv4(one) > two;
    }

    template <Traits TraitsType>
    constexpr bool operator<=(typename TraitsType::string_view_type const& one, ipv4<TraitsType> const& two) {
        return ipv4(one) <= two;
    }

    template <Traits TraitsType>
    constexpr bool operator>=(typename TraitsType::stirng_view_type const& one, ipv4<TraitsType> const& two) {
        return ipv4(one) >= two;
    }

    template <Traits TraitsType>
    constexpr bool operator==(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return two == one;
    }

    template <Traits TraitsType>
    constexpr bool operator!=(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return two != one;
    }

    template <Traits TraitsType>
    constexpr bool operator<(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return ipv4<TraitsType>(one) < two;
    }

    template <Traits TraitsType>
    constexpr bool operator>(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return ipv4<TraitsType>(one) > two;
    }

    template <Traits TraitsType>
    constexpr bool operator<=(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return ipv4<TraitsType>(one) <= two;
    }

    template <Traits TraitsType>
    constexpr bool operator>=(stl::array<uint8_t, 4> const& one, ipv4<TraitsType> const& two) {
        return ipv4<TraitsType>(one) >= two;
    }

} // namespace webpp


#endif // WEBPP_IP_H
