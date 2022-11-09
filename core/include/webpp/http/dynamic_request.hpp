// Created by moisrex on 10/10/22.

#ifndef WEBPP_DYNAMIC_REQUEST_HPP
#define WEBPP_DYNAMIC_REQUEST_HPP

#include "../traits/default_traits.hpp"
#include "./version.hpp"
#include "http_concepts.hpp"


namespace webpp::http {



    /**
     * Will provide a std::span of the provided parent request header type;
     * The data owner can be "header_fields_provider" but the protocols can have their own providers; but they
     * have to make sure this dynamic provider works for their provider as well.
     */
    template <RootExtensionList RootExtensions = empty_extension_pack>
    struct dynamic_header_fields_provider {
        using root_extensions  = RootExtensions;
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
        using field_type =
          typename root_extensions::template extensie_type<traits_type, request_header_field_descriptor>;
        using name_type   = typename field_type::string_type;
        using value_type  = typename field_type::string_type;
        using fields_type = stl::span<field_type>;

      private:
        fields_type view;

      public:
        constexpr dynamic_header_fields_provider(fields_type inp_fields) noexcept : view{inp_fields} {}

        [[nodiscard]] constexpr auto begin() const noexcept {
            return view.begin();
        }

        [[nodiscard]] constexpr auto end() const noexcept {
            return view.begin();
        }
    };

    struct dynamic_request;

    /**
     * This request type can hold other HTTP request types.
     */
    struct basic_dynamic_request {
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;

      protected:
        template <typename StrT, EnabledTraits ET>
        inline string_type stringify(StrT&& str, ET const& et) const {
            return istl::stringify_of<string_type>(str, alloc::general_alloc_for<string_type>(et));
        }

        [[nodiscard]] virtual string_type   get_uri() const              = 0;
        [[nodiscard]] virtual string_type   get_method() const           = 0;
        [[nodiscard]] virtual http::version get_version() const noexcept = 0;


        friend struct dynamic_request;

      public:
        constexpr basic_dynamic_request() noexcept                              = default;
        constexpr basic_dynamic_request(basic_dynamic_request const&) noexcept  = default;
        constexpr basic_dynamic_request(basic_dynamic_request&&) noexcept       = default;
        basic_dynamic_request& operator=(basic_dynamic_request&&) noexcept      = default;
        basic_dynamic_request& operator=(basic_dynamic_request const&) noexcept = default;
        virtual ~basic_dynamic_request()                                        = 0;
    };


    /**
     * A dynamic request; this is what the developers need to use if they want to have a dynamic request type.
     */
    struct dynamic_request final {
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using root_extensions  = empty_extension_pack;
        using fields_provider  = dynamic_header_fields_provider<root_extensions>;
        using headers_type     = simple_request_headers<traits_type, root_extensions, fields_provider>;

      private:
        basic_dynamic_request* req;

      public:
        dynamic_request(basic_dynamic_request* inp_req) noexcept : req{inp_req} {
            [[assume(inp_req != nullptr)]];
        }
        dynamic_request(basic_dynamic_request& inp_req) noexcept : req{&inp_req} {}
        dynamic_request(stl::nullptr_t)                             = delete;
        dynamic_request(dynamic_request const&) noexcept            = default;
        dynamic_request(dynamic_request&&) noexcept                 = default;
        dynamic_request& operator=(dynamic_request const&) noexcept = default;
        dynamic_request& operator=(dynamic_request&&) noexcept      = default;
        ~dynamic_request()                                          = default;

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



} // namespace webpp::http

#endif // WEBPP_DYNAMIC_REQUEST_HPP
