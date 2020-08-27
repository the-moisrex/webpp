// Created by moisrex on 8/21/20.

#ifndef WEBPP_FCGI_MANAGER_HPP
#define WEBPP_FCGI_MANAGER_HPP

#include "./fcgi_protocols.hpp"

namespace webpp::fastcgi {

    struct fcgi_manager {

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
        static bool process_header_params(const char* data, const char* const data_end,
                                          stl::string_view& name, stl::string_view& value) noexcept {
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

            name     = stl::string_view{static_cast<const char*>(data), name_size};
            value    = stl::string_view{name.data() + name_size, value_size};
            auto end = value.data() + value_size;

            return end <= data_end;
        }

        void send(auto&& block) const noexcept {
            // todo
        }

        void handle_header(header&& _header) noexcept {
            extern char* data; // todo
            extern char* data_end;
            switch (_header.type) {
                case record_type::get_values: {
                    stl::string_view name, value;
                    while (process_header_params(data, data_end, name, value)) {
                        switch (name.size()) {

                            case decltype(max_conns_reply)::real_name_length:
                                if (stl::equal(name.data(), name.data() + name.size(),
                                               max_conns_reply.name)) {
                                    send(max_conns_reply);
                                }
                                break;

                            case decltype(max_reqs_reply)::real_name_length:
                                if (stl::equal(name.data(), name.data() + name.size(), max_reqs_reply.name)) {
                                    send(max_reqs_reply);
                                }
                                break;

                            case decltype(mpxs_conns_reply)::real_name_length:
                                if (stl::equal(name.data(), name.data() + name.size(),
                                               mpxs_conns_reply.name)) {
                                    send(max_reqs_reply);
                                }
                                break;
                        }
                    }
                }
                default: {
                    unknown_type unknown;
                    // todo
                    send(unknown);
                }
            }
        }
    };

} // namespace webpp::fastcgi

#endif // WEBPP_FCGI_MANAGER_HPP
