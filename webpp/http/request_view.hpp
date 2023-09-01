// Created by moisrex on 10/10/22.

#ifndef WEBPP_REQUEST_VIEW_HPP
#define WEBPP_REQUEST_VIEW_HPP

#include "../std/span.hpp"
#include "../traits/default_traits.hpp"
#include "header_fields.hpp"
#include "http_concepts.hpp"
#include "http_version.hpp"
#include "request_headers.hpp"

#include <variant>


namespace webpp::http {


    template <Traits = default_dynamic_traits>
    struct basic_request_view;

    namespace details {

        /**
         * This is a dynamic server which holds a reference to the servers
         */
        template <Traits TraitsType, typename... ServerTypes>
        struct basic_dynamic_server {
            using servers_variant   = stl::variant<ServerTypes*...>;
            using traits_type       = TraitsType;
            using string_view_type  = traits::string_view<traits_type>;
            using supported_servers = stl::tuple<ServerTypes...>;

          private:
            servers_variant svrvar; // server variant

#define call_svr(mem, ...)                                                \
    stl::visit(                                                           \
      [](auto* svr) constexpr noexcept(noexcept(svr->mem(__VA_ARGS__))) { \
          return svr->mem(__VA_ARGS__);                                   \
      },                                                                  \
      svrvar)

          public:
            template <typename ReqT>
                requires(istl::one_of<ServerTypes..., ReqT>)
            constexpr basic_dynamic_server(ReqT& inp_server) : svrvar{&inp_server} {}

            // Get the server name that's being used
            [[nodiscard]] constexpr string_view_type server_name() const noexcept {
                return call_svr(server_name);
            }

#undef call_svr
        };


        /**
         * This request type can hold other HTTP request types.
         */
        template <typename TraitsType>
        struct request_view_interface {
            using traits_type      = TraitsType;
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


            friend struct basic_request_view<traits_type>;

          public:
            constexpr request_view_interface() noexcept                               = default;
            constexpr request_view_interface(request_view_interface const&) noexcept  = default;
            constexpr request_view_interface(request_view_interface&&) noexcept       = default;
            request_view_interface& operator=(request_view_interface&&) noexcept      = default;
            request_view_interface& operator=(request_view_interface const&) noexcept = default;
            virtual ~request_view_interface()                                         = default;
        };


        /**
         * An HTTPRequest that meets the requirements of a "request view".
         */
        template <typename TraitsType, typename T>
        concept HTTPRequestViewifiable =
          stl::is_base_of_v<request_view_interface<TraitsType>, stl::remove_cvref_t<T>> && HTTPRequest<T>;

        /**
         * Will provide a std::span of the provided parent request header type;
         * The data owner can be "header_fields_provider" but the protocols can have their own providers; but
         * they have to make sure this dynamic provider works for their provider as well.
         */
        template <Traits TraitsType>
        struct dynamic_header_fields_provider {
            using traits_type      = TraitsType;
            using string_view_type = traits::string_view<traits_type>;
            using field_type       = header_field_of<traits_type>;
            using name_type        = typename field_type::string_type;
            using value_type       = typename field_type::string_type;
            using fields_type      = stl::span<stl::add_const_t<field_type>>;

          private:
            fields_type view;

          public:
            constexpr dynamic_header_fields_provider(dynamic_header_fields_provider const&) noexcept =
              default;
            constexpr dynamic_header_fields_provider(dynamic_header_fields_provider&&) noexcept = default;
            constexpr ~dynamic_header_fields_provider() noexcept                                = default;
            constexpr dynamic_header_fields_provider&
            operator=(dynamic_header_fields_provider const&) noexcept = default;
            constexpr dynamic_header_fields_provider&
            operator=(dynamic_header_fields_provider&&) noexcept = default;

            template <typename ReqType>
                requires(HTTPRequestViewifiable<traits_type, ReqType>)
            constexpr dynamic_header_fields_provider(ReqType& inp_req) noexcept
              : dynamic_header_fields_provider{inp_req.headers.as_view()} {}

            constexpr dynamic_header_fields_provider(fields_type inp_fields) noexcept : view{inp_fields} {}

            [[nodiscard]] constexpr auto begin() const noexcept {
                return view.begin();
            }

            [[nodiscard]] constexpr auto end() const noexcept {
                return view.end();
            }

            [[nodiscard]] constexpr auto as_view() const noexcept {
                return view;
            }
        };

    } // namespace details

    /**
     * A dynamic request; this is what the developers need to use if they want to have a dynamic request type.
     */
    template <Traits TraitsType>
    struct basic_request_view {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using fields_provider  = details::dynamic_header_fields_provider<traits_type>;
        using headers_type     = request_headers<fields_provider>;

      private:
        using interface_ptr = details::request_view_interface<traits_type> const*;
        interface_ptr req   = nullptr;

      public:
        // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
        headers_type headers;
        // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)

        // An HTTP Request is passed down
        template <typename ReqType>
            requires(details::HTTPRequestViewifiable<traits_type, ReqType>)
        constexpr basic_request_view(ReqType const& inp_req) noexcept
          : req{static_cast<interface_ptr>(&inp_req)},
            headers{inp_req} {}

        template <typename ReqType>
            requires(details::HTTPRequestViewifiable<traits_type, ReqType>)
        constexpr basic_request_view(ReqType& inp_req) noexcept
          : req{static_cast<interface_ptr>(&inp_req)},
            headers{inp_req} {}

        constexpr basic_request_view(basic_request_view const&) noexcept            = default;
        constexpr basic_request_view(basic_request_view&&) noexcept                 = default;
        constexpr basic_request_view& operator=(basic_request_view const&) noexcept = default;
        constexpr basic_request_view& operator=(basic_request_view&&) noexcept      = default;
        constexpr ~basic_request_view()                                             = default;

        // An HTTP Request is passed down
        template <typename ReqType>
            requires(details::HTTPRequestViewifiable<traits_type, ReqType>)
        constexpr basic_request_view& operator=(ReqType const& inp_req) noexcept {
            req     = static_cast<interface_ptr>(&inp_req);
            headers = inp_req.headers.as_view();
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
        [[nodiscard]] constexpr http::version version() const noexcept {
            return req->get_version();
        }
    };


    using request_view = basic_request_view<>;

} // namespace webpp::http

#endif // WEBPP_REQUEST_VIEW_HPP
