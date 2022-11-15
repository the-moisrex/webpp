// Created by moisrex on 10/10/22.

#ifndef WEBPP_REQUEST_VIEW_HPP
#define WEBPP_REQUEST_VIEW_HPP

#include "../std/span.hpp"
#include "../traits/default_traits.hpp"
#include "header_fields.hpp"
#include "http_concepts.hpp"
#include "request_headers.hpp"
#include "version.hpp"


namespace webpp::http {


    struct basic_request_view;

    namespace details {

        /**
         * This request type can hold other HTTP request types.
         */
        struct request_view_interface {
            using traits_type      = default_dynamic_traits;
            using string_view_type = traits::string_view<traits_type>;
            using string_type      = traits::general_string<traits_type>;

          protected:
            template <typename StrT, EnabledTraits ET>
            inline string_type stringify(StrT&& str, ET&& et) const {
                return istl::stringify_of<string_type>(str, alloc::general_alloc_for<string_type>(et));
            }

            [[nodiscard]] virtual string_type   get_uri() const              = 0;
            [[nodiscard]] virtual string_type   get_method() const           = 0;
            [[nodiscard]] virtual http::version get_version() const noexcept = 0;


            friend struct http::basic_request_view;

          public:
            constexpr request_view_interface() noexcept                               = default;
            constexpr request_view_interface(request_view_interface const&) noexcept  = default;
            constexpr request_view_interface(request_view_interface&&) noexcept       = default;
            request_view_interface& operator=(request_view_interface&&) noexcept      = default;
            request_view_interface& operator=(request_view_interface const&) noexcept = default;
            virtual ~request_view_interface() {}
        };


        /**
         * An HTTPRequest that meets the requirements of a "request view".
         */
        template <typename T>
        concept HTTPRequestViewifiable =
          stl::is_base_of_v<request_view_interface, stl::decay_t<T>> && HTTPRequest<T> && requires {
            typename T::headers_type;
            requires HTTPRequestHeaderFieldsOwner<typename T::headers_type>;
        };

        /**
         * Will provide a std::span of the provided parent request header type;
         * The data owner can be "header_fields_provider" but the protocols can have their own providers; but
         * they have to make sure this dynamic provider works for their provider as well.
         */
        template <RootExtensionList RootExtensions>
        struct dynamic_header_fields_provider {
            using root_extensions  = RootExtensions;
            using traits_type      = default_dynamic_traits;
            using string_view_type = traits::string_view<traits_type>;
            using field_type =
              typename root_extensions::template extensie_type<traits_type, request_header_field_descriptor>;
            using name_type   = typename field_type::string_type;
            using value_type  = typename field_type::string_type;
            using fields_type = stl::span<stl::add_const_t<field_type>>;

          private:
            fields_type view;

          public:
            template <HTTPRequestViewifiable ReqType>
            constexpr dynamic_header_fields_provider(ReqType& inp_req) noexcept
              : dynamic_header_fields_provider{inp_req.headers.template as_view<field_type>()} {}

            constexpr dynamic_header_fields_provider(fields_type inp_fields) noexcept : view{inp_fields} {}

            [[nodiscard]] constexpr auto begin() const noexcept {
                return view.begin();
            }

            [[nodiscard]] constexpr auto end() const noexcept {
                return view.begin();
            }
        };

    } // namespace details

    /**
     * A dynamic request; this is what the developers need to use if they want to have a dynamic request type.
     */
    struct basic_request_view final {
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using root_extensions  = empty_extension_pack;
        using fields_provider  = details::dynamic_header_fields_provider<root_extensions>;
        using headers_type     = simple_request_headers<traits_type, root_extensions, fields_provider>;

      private:
        using interface_ptr = details::request_view_interface const*;
        interface_ptr req   = nullptr;

      public:
        headers_type headers;

        // An HTTP Request is passed down
        template <details::HTTPRequestViewifiable ReqType>
        basic_request_view(ReqType const& inp_req) noexcept
          : req{static_cast<interface_ptr>(&inp_req)},
            headers{inp_req} {}

        basic_request_view(basic_request_view const&) noexcept            = default;
        basic_request_view(basic_request_view&&) noexcept                 = default;
        basic_request_view& operator=(basic_request_view const&) noexcept = default;
        basic_request_view& operator=(basic_request_view&&) noexcept      = default;
        ~basic_request_view()                                             = default;

        // An HTTP Request is passed down
        template <details::HTTPRequestViewifiable ReqType>
        basic_request_view& operator=(ReqType const& inp_req) noexcept {
            req     = static_cast<interface_ptr>(&inp_req);
            headers = inp_req.headers.template as_view<traits_type>();
            return *this;
        }

        // Get the raw requested URI
        // This value is not checked for security; this is raw
        [[nodiscard]] string_type uri() const {
            return req->get_uri();
        }

        // Get the request METHOD (GET/PUT/POST/...)
        // This is unfiltered user input; don't store this value anywhere if you haven't checked the
        // correctness of its value
        [[nodiscard]] string_type method() const {
            return req->get_method();
        }

        // Get the HTTP version of the request
        [[nodiscard]] http::version version() const noexcept {
            return req->get_version();
        }
    };



    using request_view = basic_request_view;


} // namespace webpp::http

#endif // WEBPP_REQUEST_VIEW_HPP
