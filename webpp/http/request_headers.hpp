// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../convert/casts.hpp"
#include "./header_fields.hpp"
#include "./headers.hpp"
#include "./http_concepts.hpp"

#include <concepts>

namespace webpp::http {


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     * Boost/Beast is using std::multiset-like system; should we do the same thing instead of vector-like?
     */
    template <HTTPRequestHeaderFieldsProvider FieldsProviderType>
    class request_headers : public headers_container<FieldsProviderType> {
        using fields_provider_type = headers_container<FieldsProviderType>;

        static_assert(HTTPRequestHeaderFieldsProvider<fields_provider_type>,
                      "Fields vector is supposed to satisfy the needs of the HTTPRequestHeaderFieldOwner concept.");

      public:
        using field_type       = typename fields_provider_type::field_type;
        using name_type        = typename field_type::name_type;
        using value_type       = typename field_type::value_type;
        using string_type      = typename fields_provider_type::string_type;
        using string_view_type = typename fields_provider_type::string_view_type;

        /**
         * The Args template parameters here are reserved for any other field providers
         */
        template <typename... Args>
            requires stl::constructible_from<fields_provider_type, Args...>
        explicit constexpr request_headers(Args&&... args) : fields_provider_type{stl::forward<Args>(args)...} {}

        template <HTTPHeadersHolder T>
        explicit constexpr request_headers(T& holder) : fields_provider_type{holder} {}

        constexpr request_headers(request_headers const&)                = default;
        constexpr request_headers(request_headers&&) noexcept            = default;
        constexpr request_headers& operator=(request_headers const&)     = default;
        constexpr request_headers& operator=(request_headers&&) noexcept = default;
        constexpr ~request_headers()                                     = default;

        /**
         * Get the Content-Type as a size_t; if not specified, zero is returned.
         */
        [[nodiscard]] constexpr stl::size_t content_length() const noexcept {
            return try_to_size_t(this->view("content-length")).value_or(0);
        }
    };



} // namespace webpp::http

#endif // WEBPP_REQUEST_HEADERS_HPP
