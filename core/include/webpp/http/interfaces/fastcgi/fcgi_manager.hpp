// Created by moisrex on 8/21/20.

#ifndef WEBPP_FCGI_MANAGER_HPP
#define WEBPP_FCGI_MANAGER_HPP

#include "./fcgi_protocols.hpp"

namespace webpp::fcgi {

    struct manager {

        /**
         * Explanation of this algorithm is in FastCGI specs:
         *    http://www.mit.edu/~yandros/doc/specs/fcgi-spec.html#S3.4
         *
         *
         *    typedef struct {
         *        unsigned char nameLengthB0;  // nameLengthB0  >> 7 == 0
         *        unsigned char valueLengthB0; // valueLengthB0 >> 7 == 0
         *        unsigned char nameData[nameLength];
         *        unsigned char valueData[valueLength];
         *    } FCGI_NameValuePair11;
         *
         *    typedef struct {
         *        unsigned char nameLengthB0;  // nameLengthB0  >> 7 == 0
         *        unsigned char valueLengthB3; // valueLengthB3 >> 7 == 1
         *        unsigned char valueLengthB2;
         *        unsigned char valueLengthB1;
         *        unsigned char valueLengthB0;
         *        unsigned char nameData[nameLength];
         *        unsigned char valueData[valueLength
         *                                  ((B3 & 0x7f) << 24) + (B2 << 16) + (B1 << 8) + B0];
         *    } FCGI_NameValuePair14;
         *
         *    typedef struct {
         *        unsigned char nameLengthB3;  // nameLengthB3  >> 7 == 1
         *        unsigned char nameLengthB2;
         *        unsigned char nameLengthB1;
         *        unsigned char nameLengthB0;
         *        unsigned char valueLengthB0; // valueLengthB0 >> 7 == 0
         *        unsigned char nameData[nameLength
         *                                 ((B3 & 0x7f) << 24) + (B2 << 16) + (B1 << 8) + B0];
         *        unsigned char valueData[valueLength];
         *    } FCGI_NameValuePair41;
         *
         *    typedef struct {
         *        unsigned char nameLengthB3;  // nameLengthB3  >> 7 == 1
         *        unsigned char nameLengthB2;
         *        unsigned char nameLengthB1;
         *        unsigned char nameLengthB0;
         *        unsigned char valueLengthB3; // valueLengthB3 >> 7 == 1
         *        unsigned char valueLengthB2;
         *        unsigned char valueLengthB1;
         *        unsigned char valueLengthB0;
         *        unsigned char nameData[nameLength
         *                                 ((B3 & 0x7f) << 24) + (B2 << 16) + (B1 << 8) + B0];
         *        unsigned char valueData[valueLength
         *                                  ((B3 & 0x7f) << 24) + (B2 << 16) + (B1 << 8) + B0];
         *    } FCGI_NameValuePair44;
         *
         *
         */
        static bool process_header_params(const char* data, const char* const data_end, const char*& name,
                                          const char*& value, const char*& end) noexcept {
            if (data >= data_end)
                return false; // no more params for you

            stl::size_t name_size;
            stl::size_t value_size;

            // calculating name length
            if (static_cast<uint8_t>(*data) & 0x80u) { // if the first bit is 1
                // it means we've got a longer name length than uint8_t, we've got uint32_t
                const auto* const size = reinterpret_cast<const uint8_t*>(data);
                data += sizeof(uint32_t);

                if (data >= data_end)
                    return false; // no more params for you

                name_size = join_pieces<uint32_t, uint8_t>(size) & 0x7FFFFFFFu;
            } else {
                name_size = static_cast<uint8_t>(*data);
                ++data;
            }

            if (data >= data_end)
                return false;

            // calculating value length
            if (static_cast<uint8_t>(*data) & 0x80u) { // if the first bit is 1
                // it means we've got a longer value length than uint8_t, we've got uint32_t
                const auto* const size = reinterpret_cast<const uint8_t*>(data);
                data += sizeof(uint32_t);

                if (data >= data_end)
                    return false; // no more params for you

                value_size = join_pieces<uint32_t, uint8_t>(size) & 0x7FFFFFFFu;
            } else {
                value_size = static_cast<uint8_t>(*data);
                ++data;
            }

            name = static_cast<const char*>(data);
            value = name + name_size;
            end = value + value_size;

            return end <= data_end;
        }

        void handle_header(header&& _header) noexcept {
            switch (_header.type) {
                case record_type::get_values:
                    const char* name;
                    const char* value;
                    const char* end;
            }
        }
    };

} // namespace webpp::fcgi

#endif // WEBPP_FCGI_MANAGER_HPP
