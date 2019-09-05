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

                    VerifyOrExit(dst <= endp, error = OT_ERROR_PARSE);

                    break;
                } else {
                    VerifyOrExit('0' <= ch && ch <= '9',
                                 error = OT_ERROR_PARSE);
                }

                first = false;
                val = static_cast<uint16_t>((val << 4) | d);
                VerifyOrExit(++count <= 4, error = OT_ERROR_PARSE);
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
                        VerifyOrExit(dst <= endp, error = OT_ERROR_PARSE);

                        *dst++ = static_cast<uint8_t>(val);
                        val = 0;

                        if (ch == '\0' || ch == ' ') {
                            // Check if embedded IPv4 address had exactly four
                            // parts.
                            VerifyOrExit(dst == endp + 1,
                                         error = OT_ERROR_PARSE);
                            break;
                        }
                    } else {
                        VerifyOrExit('0' <= ch && ch <= '9',
                                     error = OT_ERROR_PARSE);

                        val = (10 * val) + (ch & 0xf);

                        // Single part of IPv4 address has to fit in one byte.
                        VerifyOrExit(val <= 0xff, error = OT_ERROR_PARSE);
                    }
                }
            }

        exit:
            return error;
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
