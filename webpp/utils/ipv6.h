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

        union fields_t {
            uint8_t m8[IPV6_ADDR_SIZE];                      // 8-bit
            uint16_t m16[IPV6_ADDR_SIZE / sizeof(uint16_t)]; // 16-bit
            uint32_t m32[IPV6_ADDR_SIZE / sizeof(uint32_t)]; // 32-bit
            uint64_t m64[IPV6_ADDR_SIZE / sizeof(uint64_t)]; // 64-bit
        };

        std::variant<std::string_view, fields_t> data;

      public:
        constexpr fields_t fields() const noexcept {
            if (std::holds_alternative<fields_t>(data))
                return std::get<fields_t>(data);
            // todo: parse
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
        constexpr bool is_unspecified() const noexcept {
            auto _data = fields();
            return _data.m64[0] == 0 && _data.m64[1] == 0;
        }

        /**
         * This method indicates whether or not the IPv6 address is the Loopback
         * Address.
         *
         * @retval TRUE   If the IPv6 address is the Loopback Address.
         * @retval FALSE  If the IPv6 address is not the Loopback Address.
         *
         */
        constexpr bool is_loopback() const noexcept {
            auto _data = fields();
            return _data.m32[0] == 0 && _data.m32[1] == 0 &&
                   _data.m32[2] == 0 && _data.m32[3] == HostSwap32(1);
        }

        std::string str() const noexcept { return ""; }
        std::string short_str() const noexcept { return ""; }
    };

} // namespace webpp

#endif // WEBPP_IPV6_H
