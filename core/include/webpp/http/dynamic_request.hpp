// Created by moisrex on 10/10/22.

#ifndef WEBPP_DYNAMIC_REQUEST_HPP
#define WEBPP_DYNAMIC_REQUEST_HPP

#include "../traits/default_traits.hpp"
#include "./version.hpp"


namespace webpp::http {


    struct basic_dynamic_headers {
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
    };

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


      public:
        virtual ~basic_dynamic_request() = 0;
    };


    struct dynamic_request : basic_dynamic_request {
        using traits_type      = default_dynamic_traits;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;


        // Get the raw requested URI
        // This value is not checked for security; this is raw
        [[nodiscard]] string_type uri() const {
            return this->get_uri();
        }

        // Get the request METHOD (GET/PUT/POST/...)
        // This is unfiltered user input; don't store this value anywhere if you haven't checked the
        // correctness of its value
        [[nodiscard]] string_type method() const {
            return this->get_method();
        }

        // Get the HTTP version of the request
        [[nodiscard]] http::version version() const noexcept {
            return this->get_version();
        }
    };


} // namespace webpp::http

#endif // WEBPP_DYNAMIC_REQUEST_HPP
