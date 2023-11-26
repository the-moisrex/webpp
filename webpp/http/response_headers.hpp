// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_HEADERS_HPP
#define WEBPP_RESPONSE_HEADERS_HPP

#include "../std/format.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "header_fields.hpp"
#include "headers.hpp"
#include "status_code.hpp"

namespace webpp::http {

    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Or rfc5987.
     */
    template <HTTPResponseHeaderFieldsProvider FieldsProviderType>
    class response_headers : public headers_container<FieldsProviderType> {
        using container = headers_container<FieldsProviderType>;

      public:
        using fields_provider_type = FieldsProviderType;
        using field_type           = typename fields_provider_type::field_type;
        using string_type          = typename field_type::string_type;

        template <typename... Args>
            requires(stl::is_constructible_v<container, Args...>)
        constexpr response_headers(Args&&... args) noexcept : container{stl::forward<Args>(args)...} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(stl::is_constructible_v<container, ET>)
        constexpr response_headers(ET&& et, http::status_code code = http::status_code::ok) noexcept(
          stl::is_nothrow_constructible_v<container, ET>)
          : container{et},
            m_status_code{static_cast<http::status_code_type>(code)} {}

        template <EnabledTraits ET>
            requires(!stl::is_constructible_v<container, ET> && stl::is_default_constructible_v<container>)
        constexpr response_headers(ET&&, http::status_code code = http::status_code::ok) noexcept(
          stl::is_nothrow_default_constructible_v<container>)
          : container{},
            m_status_code{static_cast<http::status_code_type>(code)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

      private:
        http::status_code_type m_status_code = static_cast<http::status_code_type>(http::status_code::ok);

      public:
        // set the response http status code
        constexpr response_headers& operator=(http::status_code code) noexcept {
            m_status_code = static_cast<status_code_type>(code);
            return *this;
        }

        [[nodiscard]] constexpr http::status_code_type status_code_integer() const noexcept {
            return m_status_code;
        }

        [[nodiscard]] constexpr http::status_code status_code() const noexcept {
            return static_cast<http::status_code>(m_status_code);
        }

        constexpr void status_code(http::status_code code) noexcept {
            m_status_code = static_cast<status_code_type>(code);
        }

        constexpr void status_code(http::status_code_type code) noexcept {
            m_status_code = code;
        }

        template <typename StringType>
        constexpr void string_to(StringType& out) const {
            // todo check performance
            // TODO: add support for other HTTP versions
            // res << "HTTP/1.1" << " " << status_code() << " " <<
            // status_reason_phrase(status_code()) << "\r\n";
            stl::size_t size = 1;
            for (auto const& field : *this) {
                size += field.name.size() + field.value.size() + 4;
            }
            out.reserve(size);
            for (auto const& field : *this) {
                // todo: make sure value is secure and doesn't have any newlines
                fmt::format_to(stl::back_insert_iterator<string_type>(out),
                               "{}: {}\r\n",
                               field.name,
                               field.value);
            }
        }

        template <istl::String StringType = string_type>
        constexpr void to_string(StringType& out) const {
            string_to<StringType>(out);
        }

        [[nodiscard]] constexpr bool operator==(response_headers const& other) const noexcept {
            return m_status_code == other.m_status_code &&
                   static_cast<container const&>(*this) == static_cast<container const&>(other);
        }

        [[nodiscard]] constexpr bool operator!=(response_headers const& other) const noexcept {
            return !operator==(other);
        }
    };



} // namespace webpp::http

#endif // WEBPP_RESPONSE_HEADERS_HPP
