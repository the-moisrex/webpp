#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "../std/functional.hpp"
#include "../std/vector.hpp"
#include "../traits/enable_traits.hpp"
#include "body.hpp"
#include "http_concepts.hpp"

#include <algorithm>
#include <variant>

namespace webpp::http {

    /**
     * @brief Response Body
     *
     * Implements: CStreamBasedBodyCommunicator, StreamBasedBodyCommunicator, TextBasedBodyCommunicator
     */
    template <Traits TraitsType>
    struct response_body : public body_writer<TraitsType> {
        using traits_type               = TraitsType;
        using string_communicator_type  = string_response_body_communicator<traits_type>;
        using cstream_communicator_type = cstream_response_body_communicator<traits_type>;
        using stream_communicator_type  = stream_response_body_communicator<traits_type>;
        using stream_type               = typename stream_communicator_type::element_type;
        using body_communicator_type    = body_writer<TraitsType>;


        static constexpr auto log_cat = "ResBody";

      public:
        constexpr response_body(response_body const&)                = default;
        constexpr response_body(response_body&&) noexcept            = default;
        constexpr response_body& operator=(response_body const&)     = default;
        constexpr response_body& operator=(response_body&&) noexcept = default;
        constexpr ~response_body() noexcept                          = default;

        using body_communicator_type::operator=;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, response_body>)
        explicit constexpr response_body(ET&& et) : body_communicator_type{et.get_traits()} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET, typename T>
        constexpr response_body(ET&& et, T&& obj) : body_communicator_type{et.get_traits()} {
            this->template set<T>(stl::forward<T>(obj));
        }
    };

} // namespace webpp::http

#endif // WEBPP_RESPONSE_BODY_HPP
