#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

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
    template <typename REL, typename ServerType>
    struct common_http_request : public enable_traits_with<typename ServerType::traits_type, REL> {
        using server_type         = ServerType;
        using server_ref          = stl::add_lvalue_reference_t<server_type>;
        using root_extensions     = typename server_type::root_extensions;
        using traits_type         = typename server_type::traits_type;
        using etraits             = enable_traits_with<traits_type, REL>;
        using string_type         = traits::general_string<traits_type>;
        using string_view_type    = traits::string_view<traits_type>;
        using char_type           = traits::char_type<traits_type>;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using fields_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char_type>;
        // using fields_allocator_type = traits::general_allocator<traits_type, char_type>;
        using request_extensions = REL;
        using fields_provider    = header_fields_provider<traits_type, root_extensions>;
        using headers_type       = simple_request_headers<traits_type, root_extensions, fields_provider>;
        using body_type          = simple_request_body<traits_type, server_type>;
        using response_type      = simple_response<traits_type, root_extensions>;

        static_assert(HTTPRequestHeaders<headers_type>,
                      "Something is wrong with the request's headers type.");
        static_assert(HTTPRequestBody<body_type>, "Something is wrong with the request's body type.");


        headers_type                    headers; // NOLINT(misc-non-private-member-variables-in-classes)
        [[no_unique_address]] body_type body;    // NOLINT(misc-non-private-member-variables-in-classes)
        server_ref                      server;  // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr common_http_request(server_ref inp_server) noexcept
          : etraits{inp_server},
            headers{inp_server},
            body{inp_server},
            server{inp_server} {}

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



        [[nodiscard]] constexpr HTTPResponse auto response() const {
            response_type res{*this};
            // todo: calculate the default response headers based on the request here
            return res;
        }


        template <typename T>
        constexpr T as() const {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (requires {
                              { deserialize_request_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_request_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_request_body<T>(this->body) } -> stl::same_as<T>;
                                 }) {
                return deserialize_request_body<T>(this->body);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(this->body) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(this->body);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        constexpr auto as() const {
            return auto_converter<common_http_request>{.obj = *this};
        }

        template <typename T>
            requires(!istl::part_of<T, body_type, headers_type>)
        constexpr operator T() const {
            return as<T>();
        }
    };

    /**
     * If you want to add features to all of the request types, you can use this type
     */
    template <typename ReqType>
    struct final_request final : public ReqType {
        using traits_type      = typename ReqType::traits_type;
        using string_view_type = traits::string_view<traits_type>;

        using ReqType::ReqType;

        constexpr final_request(final_request const&)                = default;
        constexpr final_request(final_request&&) noexcept            = default;
        constexpr final_request& operator=(final_request const&)     = default;
        constexpr final_request& operator=(final_request&&) noexcept = default;
        constexpr ~final_request()                                   = default;
    };

    template <template <typename...> typename MidLevelRequestType, typename ServerType>
    struct request_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_extensions;

        template <typename RootExtensions, typename TraitsType, typename RequestEList>
        using mid_level_extensie_type = MidLevelRequestType<common_http_request<RequestEList, ServerType>>;

        // empty final extensie
        // template <RootExtensionList RootExtensions,
        //           Traits            TraitsType,
        //           typename MidLevelRequestWithExtensions,
        //           typename... extra>
        // using final_extensie_type = final_request<TraitsType, MidLevelRequestWithExtensions>;
    };


    template <typename ServerType, template <typename...> typename MidLevelRequestType>
    using simple_request = typename ServerType::root_extensions::template extensie_type<
      typename ServerType::traits_type,
      request_descriptor<MidLevelRequestType, ServerType>>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_H
