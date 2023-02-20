#ifndef WEBPP_REQUEST_HPP
#define WEBPP_REQUEST_HPP

#include "../memory/object.hpp"
#include "../traits/enable_traits.hpp"
#include "../version.hpp"
#include "./header_fields.hpp"
#include "./http_concepts.hpp"
#include "./request_body.hpp"
#include "./request_headers.hpp"
#include "./response.hpp"

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
        constexpr T as() const {
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

        constexpr auto as() const {
            return auto_converter<common_http_request>{.obj = *this};
        }

        template <typename T>
            requires(!istl::part_of<stl::remove_cvref_t<T>, body_type, headers_type>)
        constexpr operator T() const {
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
     *
     */
    struct request
      : public common_http_request<
          istl::nothing_type,
          simple_request_headers<header_fields_provider<default_dynamic_traits, empty_extension_pack>>,
          simple_request_body<default_dynamic_traits,
                              empty_extension_pack,
                              body_reader<default_dynamic_traits>>>,
        public details::request_view_interface {

        using headers_type =
          simple_request_headers<header_fields_provider<default_dynamic_traits, empty_extension_pack>>;
        using body_type = simple_request_body<default_dynamic_traits,
                                              empty_extension_pack,
                                              body_reader<default_dynamic_traits>>;
        using traits_type     = typename headers_type::traits_type;
        using root_extensions = typename headers_type::root_extensions;

        // todo: implement request view interface
        // todo: implement copying from simple_requests
    };

} // namespace webpp::http

#endif // WEBPP_REQUEST_HPP
