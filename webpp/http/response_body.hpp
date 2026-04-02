#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "body.hpp"

namespace webpp::http {

    /**
     * @brief Response Body
     *
     * Implements: CStreamBasedBodyCommunicator, StreamBasedBodyCommunicator, TextBasedBodyCommunicator
     */
    template <istl::CharType CharT, Allocator AllocT = default_allocator_t<CharT>>
    struct response_body : public body_writer<CharT, AllocT> {
        using string_communicator_type  = string_response_body_communicator<CharT, AllocT>;
        using cstream_communicator_type = cstream_response_body_communicator<AllocT>;
        using stream_communicator_type  = stream_response_body_communicator<CharT, AllocT>;
        using stream_type               = typename stream_communicator_type::element_type;
        using body_communicator_type    = body_writer<CharT, AllocT>;


        static constexpr auto log_cat = "ResBody";

        using body_writer<CharT, AllocT>::body_writer; // ctor

        constexpr response_body() noexcept = default;

        constexpr response_body(response_body const&)                = default;
        constexpr response_body(response_body&&) noexcept            = default;
        constexpr response_body& operator=(response_body const&)     = default;
        constexpr response_body& operator=(response_body&&) noexcept = default;
        constexpr ~response_body() noexcept                          = default;

        using body_communicator_type::operator=;

        // template <typename T>
        // explicit constexpr response_body(T&& obj) {
        //     this->template set<T>(stl::forward<T>(obj));
        // }
    };

} // namespace webpp::http

#endif // WEBPP_RESPONSE_BODY_HPP
