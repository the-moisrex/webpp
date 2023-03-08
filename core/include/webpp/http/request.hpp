#ifndef WEBPP_HTTP_REQUEST_HPP
#define WEBPP_HTTP_REQUEST_HPP

#include "../traits/enable_traits.hpp"
#include "../version.hpp"
#include "./body.hpp"
#include "./header_fields.hpp"
#include "./http_concepts.hpp"
#include "./request_body.hpp"
#include "./request_headers.hpp"
#include "./request_view.hpp"

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
    template <typename REL, typename HeadersType, typename BodyType>
    struct common_http_request : public enable_traits_with<typename HeadersType::traits_type, REL> {
        using headers_type       = HeadersType;
        using body_type          = BodyType;
        using root_extensions    = typename headers_type::root_extensions;
        using traits_type        = typename headers_type::traits_type;
        using etraits            = enable_traits_with<traits_type, REL>;
        using string_type        = traits::general_string<traits_type>;
        using string_view_type   = traits::string_view<traits_type>;
        using request_extensions = REL;

        static_assert(HTTPRequestHeaders<headers_type>,
                      "Something is wrong with the request's headers type.");
        static_assert(HTTPRequestBody<body_type>, "Something is wrong with the request's body type.");


        headers_type                    headers; // NOLINT(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] body_type body;    // NOLINT(misc-non-private-member-variables-in-classes)

      public:
        template <typename ServerType>
        constexpr common_http_request(ServerType& inp_server) noexcept
          : etraits{inp_server},
            headers{inp_server},
            body{inp_server} {}

        constexpr common_http_request(common_http_request const&)                     = default;
        constexpr common_http_request(common_http_request&&) noexcept                 = default;
        constexpr common_http_request& operator=(common_http_request const&) noexcept = default;
        constexpr common_http_request& operator=(common_http_request&&) noexcept      = default;
        constexpr ~common_http_request()                                              = default;


        /**
         * Get the Web++ Library version
         */
        [[nodiscard]] string_view_type framework_version() const noexcept {
            return webpp_version;
        }


        template <typename T>
            requires(HTTPDeserializableBody<T, common_http_request>)
        [[nodiscard]] constexpr T as() const {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (requires {
                              { deserialize_request_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_request_body<T>(*this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                return this->body.template as<T>();
            }
        }

        template <typename T>
        // requires(HTTPDeserializableBody<T, common_http_request>)
        [[nodiscard]] constexpr T as() {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (requires {
                              { deserialize_request_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_request_body<T>(*this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                return this->body.template as<T>();
            }
        }

        [[nodiscard]] constexpr auto_converter<common_http_request> as() const {
            return {*this};
        }

        [[nodiscard]] constexpr auto_converter<common_http_request> as() {
            return {*this};
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, common_http_request, headers_type, body_type, request_view>)
        constexpr operator T() const {
            return as<T>();
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, common_http_request, headers_type, body_type, request_view>)
        constexpr operator T() {
            return as<T>();
        }
    };

    template <template <typename...> typename MidLevelRequestType, typename HeadersType, typename BodyType>
    struct request_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_extensions;

        template <typename RootExtensions, typename TraitsType, typename RequestEList>
        using mid_level_extensie_type =
          MidLevelRequestType<common_http_request<RequestEList, HeadersType, BodyType>>;
    };


    template <template <typename...> typename MidLevelRequestType, typename HeadersType, typename BodyType>
    using simple_request = typename HeadersType::root_extensions::template extensie_type<
      typename HeadersType::traits_type,
      request_descriptor<MidLevelRequestType, HeadersType, BodyType>>;


    /**
     * Dynamic Request type
     *
     *   1. The difference between this request type and the `request_view` type is that this class
     *      owns its data while the request view class doesn't own its data.
     *   2. The difference between this request type and the `simple_request` alias is that this request
     *      type is dynamic and easy to use while the other one requires the Protocol to specify the
     *      right template parameters. This class can copy the data from that type of request directly.
     *   3. This request's body is writable as well as readable.
     *
     */
    template <Traits TraitsType = default_dynamic_traits>
    struct basic_request final
      : public common_http_request<
          istl::nothing_type,
          simple_request_headers<header_fields_provider<TraitsType, empty_extension_pack>>,
          simple_request_body<TraitsType, empty_extension_pack, body_writer<TraitsType>>>,
        public details::request_view_interface {

        using common_request_type = common_http_request<
          istl::nothing_type,
          simple_request_headers<header_fields_provider<TraitsType, empty_extension_pack>>,
          simple_request_body<TraitsType, empty_extension_pack, body_writer<TraitsType>>>;
        using headers_type = simple_request_headers<header_fields_provider<TraitsType, empty_extension_pack>>;
        using body_type    = simple_request_body<TraitsType, empty_extension_pack, body_writer<TraitsType>>;
        using traits_type  = typename headers_type::traits_type;
        using root_extensions = typename headers_type::root_extensions;

        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;

      private:
        string_type   requested_uri;    // todo: isn't it better to have a uri instead?
        string_type   requested_method; // It's a string because the user might send a custom method
        http::version request_version;

      protected:
        using pstring_type = typename request_view::string_type;

        template <typename T>
        [[nodiscard]] inline pstring_type pstringify(T&& str) const {
            return istl::stringify_of<pstring_type>(stl::forward<T>(str),
                                                    alloc::general_alloc_for<pstring_type>(*this));
        }

        [[nodiscard]] pstring_type get_method() const override {
            return pstringify(this->method());
        }

        [[nodiscard]] pstring_type get_uri() const override {
            return pstringify(this->uri());
        }

        [[nodiscard]] http::version get_version() const noexcept override {
            return this->version();
        }

      public:
        template <HTTPRequest ReqType>
            requires(!istl::same_as_cvref<ReqType, basic_request>)
        constexpr basic_request(ReqType& req)
          : common_request_type{req},
            requested_uri{req.uri(), alloc::general_alloc_for<string_type>(*this)},
            requested_method{req.method(), alloc::general_alloc_for<string_type>(*this)},
            request_version{req.version()} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET, typename MStrT = string_view_type, typename UStrT = string_view_type>
            requires(!istl::same_as_cvref<ET, basic_request> && istl::StringifiableOf<string_type, UStrT> &&
                     istl::StringifiableOf<string_type, MStrT>)
        constexpr basic_request(ET&&          et,
                                MStrT&&       method = "GET",
                                UStrT&&       url    = "/",
                                http::version ver    = http::http_2_0)
          : common_request_type{et},
            requested_uri{istl::stringify_of<string_type>(stl::forward<UStrT>(url),
                                                          alloc::general_alloc_for<string_type>(*this))},
            requested_method{istl::stringify_of<string_type>(stl::forward<MStrT>(method),
                                                             alloc::general_alloc_for<string_type>(*this))},
            request_version{ver} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr basic_request(basic_request const&)      = default;
        constexpr basic_request(basic_request&&) noexcept  = default;
        basic_request& operator=(basic_request const&)     = default;
        basic_request& operator=(basic_request&&) noexcept = default;
        constexpr ~basic_request() final                   = default;


        // Get a request view from this request
        [[nodiscard]] constexpr request_view view() const noexcept {
            return {*this};
        }

        [[nodiscard]] constexpr string_type const& uri() const noexcept {
            return requested_uri;
        }


        [[nodiscard]] constexpr string_type const& method() const noexcept {
            return requested_method;
        }

        [[nodiscard]] constexpr http::version version() const noexcept {
            return request_version;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return this->heeaders.empty() && this->body.empty() && requested_uri.empty() &&
                   requested_method.empty();
        }
    };


    using request = basic_request<default_dynamic_traits>;

} // namespace webpp::http

#endif // WEBPP_HTTP_REQUEST_HPP
