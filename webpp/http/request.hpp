#ifndef WEBPP_HTTP_REQUEST_HPP
#define WEBPP_HTTP_REQUEST_HPP

#include "../memory/allocators.hpp"
#include "../uri/path_traverser.hpp"
#include "../uri/scheme.hpp"
#include "./body.hpp"
#include "./body_concepts.hpp"
#include "./header_fields.hpp"
#include "./http_concepts.hpp"
#include "./http_version.hpp"
#include "./request_body.hpp"
#include "./verbs.hpp"

#include <concepts>

namespace webpp::http {

    /**
     * Only the protocol should be instantiating this class. There should be no
     * need for anybody else to do it.
     *
     * The user has access to this class. This class should be accessed as a const
     * variable since the user should not be able to make any changes here. Every
     * action that the user has to make should be in the "response" class or other
     * relative classes that finally end up in that class. This class is only for
     * giving information that the user or other modules need.
     *
     */
    template <HTTPHeaders HeadersType, HTTPBody BodyType>
    struct common_http_request {
        using headers_type     = HeadersType;
        using body_type        = BodyType;
        using string_type      = typename headers_type::string_type;
        using string_view_type = typename headers_type::string_view_type;
        using allocator_type   = typename string_type::allocator_type;

        static_assert(stl::same_as<typename headers_type::allocator_type, allocator_type>,
                      "They should have the same allocator type.");
        static_assert(HTTPRequestHeaders<headers_type>, "Something is wrong with the request's headers type.");
        static_assert(HTTPRequestBody<body_type>, "Something is wrong with the request's body type.");


        headers_type                    headers; // NOLINT(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] body_type body;    // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr explicit common_http_request(allocator_type const& inp_alloc = alloc) noexcept
          : headers{inp_alloc},
            body{inp_alloc} {}

        template <typename ReqT>
            requires(HTTPRequest<ReqT> && HTTPHeadersHolder<ReqT> && HTTPBodyHolder<ReqT> &&
                     !istl::cvref_as<ReqT, common_http_request>)
        constexpr explicit common_http_request(ReqT& inp_req)
          : headers{inp_req.headers},
            body{inp_req.body} {}

        constexpr common_http_request(common_http_request const&)                     = default;
        constexpr common_http_request(common_http_request&&) noexcept                 = default;
        constexpr common_http_request& operator=(common_http_request const&) noexcept = default;
        constexpr common_http_request& operator=(common_http_request&&) noexcept      = default;

        constexpr ~common_http_request() = default;

        template <typename T>
            requires(HTTPGenerallyDeserializableBody<T, common_http_request>)
        [[nodiscard]] constexpr T as() const {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (DeserializableRequestBody<T, common_http_request>) {
                return deserialize_request_body(stl::type_identity<T>{}, *this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                return this->body.template as<T>();
            }
        }

        template <typename T>
        // requires(HTTPGenerallyDeserializableBody<T, common_http_request>)
        [[nodiscard]] constexpr T as() {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (DeserializableRequestBody<T, common_http_request>) {
                return deserialize_request_body(stl::type_identity<T>{}, *this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                return this->body.template as<T>();
            }
        }

        [[nodiscard]] constexpr auto as() const {
            return auto_converter<common_http_request>{*this};
        }

        [[nodiscard]] constexpr auto as() {
            return auto_converter<common_http_request>{*this};
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, common_http_request, headers_type, body_type>)
        explicit constexpr operator T() const {
            return as<T>();
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, common_http_request, headers_type, body_type>)
        explicit constexpr operator T() {
            return as<T>();
        }
    };

    template <template <typename...> typename MidLevelRequestType, typename HeadersType, typename BodyType>
    using simple_request = MidLevelRequestType<common_http_request<HeadersType, BodyType>>;

    /**
     * Dynamic Request type
     *
     *   1. The difference between this request type and the `simple_request` alias is that this request
     *      type is dynamic and easy to use while the other one requires the Protocol to specify the
     *      right template parameters. This class can copy the data from that type of request directly.
     *   2. This request's body is writable as well as readable.
     *
     */
    template <istl::CharType CharT, Allocator AllocT = default_allocator_t<CharT>>
    struct basic_request final
      : public common_http_request<request_headers<header_fields_provider<header_field_of<CharT, AllocT>>>,
                                   request_body<body_writer<CharT, AllocT>>> {
        using common_request_type =
          common_http_request<request_headers<header_fields_provider<header_field_of<CharT, AllocT>>>,
                              request_body<body_writer<CharT, AllocT>>>;
        using headers_type     = request_headers<header_fields_provider<header_field_of<CharT, AllocT>>>;
        using body_type        = request_body<body_writer<CharT, AllocT>>;
        using string_type      = typename headers_type::string_type;
        using string_view_type = typename headers_type::string_view_type;
        using char_type        = CharT;
        using allocator_type   = AllocT;
        using scheme_type      = uri::basic_scheme<char_type>;

      private:
        // Request Pseudo-Headers (method, scheme, authority, path, protocol)
        http::verb          requested_method = verb::unknown;
        http::version       request_version;
        uri::special_scheme requested_scheme = uri::special_scheme::unknown;
        stl::string         requested_method_str; // It's a string because the user might send a custom method

        // Even when schemes cannot be mixed arbitrarily, the scheme is still a per‑request property because HTTP/2/3
        // does not implicitly know it from the connection.
        stl::string requested_scheme_str;

        stl::string requested_authority;
        stl::string requested_target; // path + query = target
        stl::string requested_protocol;

      public:
        template <HTTPRequest ReqType>
            requires(!istl::cvref_as<ReqType, basic_request>)
        constexpr explicit basic_request(ReqType& req)
          : common_request_type{req},
            requested_method{req.method(), alloc},
            requested_target{req.uri(), alloc},
            request_version{req.version()} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        constexpr explicit basic_request(
          string_type         inp_method    = "GET",
          string_type         inp_scheme    = "http",
          string_type         inp_authority = "",
          string_type         inp_target    = "/",
          string_type         inp_protocol  = "",
          http::version const ver           = http::http_2_0)
          : common_request_type{},
            requested_method{stl::move(inp_method)},
            requested_scheme{stl::move(inp_scheme)},
            requested_authority{stl::move(inp_authority)},
            requested_target{stl::move(inp_target)},
            requested_protocol{stl::move(inp_protocol)},
            request_version{ver} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr basic_request(basic_request const&)      = default;
        constexpr basic_request(basic_request&&) noexcept  = default;
        basic_request& operator=(basic_request const&)     = default;
        basic_request& operator=(basic_request&&) noexcept = default;

        constexpr ~basic_request() = default;

        [[nodiscard]] constexpr string_view_type target() const noexcept {
            return string_view_type{requested_target};
        }

        constexpr basic_request& target(string_type inp_target) {
            requested_target = stl::move(inp_target);
            return *this;
        }

        constexpr basic_request& target(string_view_type inp_target) {
            requested_target = inp_target;
            return *this;
        }

        [[nodiscard]] constexpr string_view_type method_string() const noexcept {
            if (requested_method != verb::unknown) [[unlikely]] {
                return string_view_type{requested_method_str};
            }
            return to_string(requested_method);
        }

        [[nodiscard]] constexpr verb method() const noexcept {
            return requested_method;
        }

        constexpr basic_request& method(string_type str) {
            requested_method = string_to_verb(str);
            if (requested_method == verb::unknown) [[unlikely]] {
                requested_method_str = stl::move(str);
            } else {
                requested_method_str.clear();
            }
            return *this;
        }

        [[nodiscard]] constexpr http::version version() const noexcept {
            return request_version;
        }

        constexpr basic_request& version(http::version const ver) noexcept {
            request_version = ver;
            return *this;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return this->heeaders.empty() && this->body.empty() && requested_target.empty() && requested_method.empty();
        }

        [[nodiscard]] constexpr scheme_type scheme() const noexcept {
            return scheme_type{requested_scheme};
        }
    };

    using request = basic_request<char>;

} // namespace webpp::http

#endif // WEBPP_HTTP_REQUEST_HPP
