// Created by moisrex on 12/19/22.

#ifndef WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP
#define WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP

#include "../std/type_traits.hpp"
#include "../traits/traits.hpp"

#include <iostream>

namespace webpp::http::cgi_proto {

    /**
     * The middle man between CGI request body and the framework's request body.
     *
     * This type implements HTTPRequestBodyCommunicator
     */
    template <typename ProtocolType>
    struct cgi_request_body_communicator {
        using protocol_type    = ProtocolType;
        using traits_type      = typename protocol_type::traits_type;
        using char_type        = traits::char_type<traits_type>;
        using byte_type        = stl::byte;
        using size_type        = stl::streamsize;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::string<traits_type>;


      private:
        string_type body_content;

      public:
        cgi_request_body_communicator(auto& inp_cgi) : body_content{get_alloc_for<string_type>(inp_cgi)} {
            auto const content_length_str = inp_cgi.env("CONTENT_LENGTH");
            if (!content_length_str.empty()) {
                auto const content_length = to_uint(content_length_str);
                body_content.resize(content_length);
                // todo: see if there's a better way to do this
                // stl::cin.rdbuf()->pubsetbuf(body_content.data(), content_length);
                // while (stl::fgets(body_content.data(), content_length, stdin) == nullptr);
                stl::cin.get(body_content.data(), content_length + 1);
            } else {
                // we don't know how much the user is going to send. so we use a small size buffer:

                // TODO: add something here
            }
        }

        // set the body with char type as the byte type
        void write(char const* data, size_type count) noexcept {
            body_content.append(data, static_cast<stl::size_t>(count));
        }

        // set the body
        void write(byte_type const* data, size_type count) noexcept {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            body_content.append(reinterpret_cast<char const*>(data), static_cast<stl::size_t>(count));
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        /**
         * Read the body of the string
         */
        [[nodiscard]] size_type read(char* data, size_type count) const {
            return protocol_type::read(data, count);
        }

        /**
         * Read the body of the string
         */
        [[nodiscard]] size_type read(byte_type* data, size_type count) const {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            return protocol_type::read(reinterpret_cast<char*>(data), count);
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        [[nodiscard]] size_type read(byte_type* data) const {
            return protocol_type::read(data);
        }

        /**
         * Even though this request body is a CStreamBasedBodyCommunicator, and CStreams don't require being
         * SizableBody, we can do such thing for CGI request body communicator.
         */
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return body_content.size();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return body_content.empty();
        }
    };

} // namespace webpp::http::cgi_proto

#endif // WEBPP_CGI_REQUEST_BODY_COMMUNICATOR_HPP
