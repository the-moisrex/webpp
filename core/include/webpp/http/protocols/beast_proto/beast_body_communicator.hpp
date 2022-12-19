// Created by moisrex on 12/15/22.

#ifndef WEBPP_BEAST_BODY_COMMUNICATOR_HPP
#define WEBPP_BEAST_BODY_COMMUNICATOR_HPP

#include "../../../std/type_traits.hpp"
#include "../../../traits/traits.hpp"


namespace webpp::http::beast_proto {

    /**
     * The middle man between beast request body and the framework's request body.
     */
    template <typename ProtocolType>
    struct beast_request_body_communicator {
        using protocol_type             = ProtocolType;
        using traits_type               = typename protocol_type::traits_type;
        using char_type                 = traits::char_type<traits_type>;
        using size_type                 = stl::size_t;
        using http_worker_type          = typename protocol_type::http_worker_type;
        using beast_request_parser_type = typename http_worker_type::beast_request_parser_type;
        using body_type                 = typename http_worker_type::beast_body_type;
        using body_ptr                  = stl::add_pointer_t<body_type>;


        void set_beast_parser(beast_request_parser_type& input_parser) noexcept {
            body = input_parser.get().body();
        }

        [[nodiscard]] size_type read(char_type* data, size_type count) const noexcept {
            return stl::copy_n(body->data(), count, data) - body->data();
        }

        [[nodiscard]] size_type read(char_type* data) const noexcept {
            return stl::copy_n(body->data(), size(), data) - body->data();
        }

        [[nodiscard]] size_type size() const noexcept {
            return body->payload_size();
        }

      private:
        body_ptr body = nullptr;
    };


} // namespace webpp::http::beast_proto

#endif // WEBPP_BEAST_BODY_COMMUNICATOR_HPP
