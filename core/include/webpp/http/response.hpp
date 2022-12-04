#ifndef WEBPP_HTTP_RESPONSE_H
#define WEBPP_HTTP_RESPONSE_H

#include "../convert/casts.hpp"
#include "../strings/append.hpp"
#include "../traits/traits.hpp"
#include "header_fields.hpp"
#include "http_concepts.hpp"
#include "response_body.hpp"
#include "response_headers.hpp"

namespace webpp::http {

    struct response_descriptor;

    /**
     * This class owns its data.
     */
    template <typename EList, typename ResponseHeaderType, typename BodyType>
    struct basic_response : public EList {

        // we're not going to use trait's string type here.
        using body_type           = BodyType;
        using headers_type        = ResponseHeaderType;
        using traits_type         = typename headers_type::traits_type;
        using root_extensions     = typename headers_type::root_extensions;
        using elist_type          = EList;
        using basic_response_type = basic_response<EList, ResponseHeaderType, BodyType>;

        body_type    body{};
        headers_type headers{};

        template <typename Arg1, typename... Args>
        constexpr basic_response([[maybe_unused]] Arg1&& arg1, Args&&... args) noexcept
          requires(!istl::one_of<Arg1, headers_type, http::status_code_type, body_type> &&
                   stl::is_constructible_v<elist_type, Args...>)
          : elist_type{stl::forward<Args>(args)...} {}

        constexpr basic_response() noexcept                          = default;
        constexpr basic_response(basic_response const& res) noexcept = default;
        constexpr basic_response(basic_response&& res) noexcept      = default;

        constexpr basic_response(http::status_code_type err_code) noexcept
          : elist_type{},
            headers{err_code} {}

        constexpr explicit basic_response(body_type const& b) noexcept : elist_type{}, body(b) {}
        constexpr explicit basic_response(body_type&& b) noexcept : elist_type{}, body(stl::move(b)) {}

        constexpr explicit basic_response(headers_type&& e) noexcept : elist_type{}, headers(stl::move(e)) {}
        constexpr explicit basic_response(headers_type const& e) noexcept : elist_type{}, headers(e) {}

        constexpr basic_response& operator=(basic_response const&)         = default;
        constexpr basic_response& operator=(basic_response&& res) noexcept = default;

        [[nodiscard]] bool operator==(basic_response const& res) const noexcept {
            return headers == res.headers && body == res.body;
        }

        [[nodiscard]] bool operator!=(basic_response const& res) const noexcept {
            return headers != res.headers || body != res.body;
        }

        /**
         *
         * The reason why this function exists here and not in "headers" is because this function may need
         * access to the body as well to check the body value's type for example.
         */
        void calculate_default_headers() noexcept {
            using header_field_type = typename headers_type::field_type;
            using str_t             = typename header_field_type::string_type;

            bool has_content_type   = false;
            bool has_content_length = false;

            // todo: use headers facilities and if there aren't any, make them
            for (const auto& header : headers) {
                if (ascii::iequals<ascii::char_case_side::second_lowered>(header.name, "content-type"))
                    has_content_type = true;
                else if (ascii::iequals<ascii::char_case_side::second_lowered>(header.name, "content-length"))
                    has_content_length = true;
            }

            // todo: we can optimize this, pre-calculate the default header fields and copy when needed
            if (!has_content_type) {
                static const header_field_type content_type_field{
                  str_t{"Content-Type", headers.get_allocator()},
                  str_t{"text/html; charset=utf-8", headers.get_allocator()}};
                headers.push_back(content_type_field);
            }

            if (!has_content_length) {
                str_t value{headers.get_allocator()};
                append_to(value, body.str().size() * sizeof(char));
                headers.push_back(
                  header_field_type{str_t{"Content-Length", headers.get_allocator()}, stl::move(value)});
            }
        }
    };




    template <typename EList>
    struct final_response final : public EList {
      private:
        using super = EList;

      public:
        using traits_type     = typename super::traits_type;
        using root_extensions = typename super::root_extensions;
        using response_type   = final_response<EList>;
        using body_type       = typename response_type::body_type;
        using headers_type    = typename response_type::headers_type;

        static_assert(HTTPResponseBody<body_type>, "Body is not a valid body type.");
        static_assert(HTTPResponseHeaders<headers_type>, "Header is not a valid header.");

        using EList::EList;


        template <typename... Args>
        [[nodiscard]] constexpr static auto with_body(Args&&... args) {
            return response_type{body_type{stl::forward<Args>(args)...}};
        }

        template <typename... Args>
        [[nodiscard]] constexpr static auto with_headers(Args&&... args) {
            return response_type{headers_type{stl::forward<Args>(args)...}};
        }

        template <Extension... NewExtensions, typename... Args>
        [[nodiscard]] static constexpr HTTPResponse auto create(Args&&... args) {
            using new_response_type =
              typename response_type::template apply_extensions_type<NewExtensions...>;
            return new_response_type{stl::forward<Args>(args)...};
        }

        /**
         * Generate a response
         */
        template <Extension... NewExtensions, EnabledTraits ET, typename... Args>
        [[nodiscard]] static constexpr HTTPResponse auto create(ET&& et, Args&&... args) {
            using new_response_type =
              typename response_type::template apply_extensions_type<NewExtensions...>;
            // todo: write an auto extension finder based on the Args that get passed


            if constexpr (requires { new_response_type{et, stl::forward<Args>(args)...}; }) {
                // ctx is EnabledTraits type, passing ctx as the first argument will help the extensions to be
                // able to have access to the etraits.
                return new_response_type{et, stl::forward<Args>(args)...};

                // todo: add more ways for passing the allocator too.
            } else if constexpr (requires { new_response_type{stl::forward<Args>(args)..., et}; }) {
                // ctx is EnabledTraits type, passing ctx as the first argument will help the extensions to be
                // able to have access to the etraits.
                return new_response_type{stl::forward<Args>(args)..., et};
            } else {
                return new_response_type{stl::forward<Args>(args)...};
            }
        }


        /**
         * Append some extensions to this context type and get the type back
         */
        template <Extension... E>
        using apply_extensions_type =
          typename istl::unique_parameters<typename root_extensions::template appended<
            stl::remove_cvref_t<E>...>>::template extensie_type<traits_type, response_descriptor>;
    };



    struct response_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::response_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = basic_response<
          EList,
          typename RootExtensions::template extensie_type<TraitsType, response_headers_descriptor>,
          typename RootExtensions::template extensie_type<TraitsType, response_body_descriptor>>;

        // empty final extensie
        template <typename RootExtensions, typename TraitsType, typename EList>
        using final_extensie_type = final_response<EList>;
    };


    template <Traits TraitsType, typename RootExtensions>
    using simple_response_pack =
      typename RootExtensions::template extensie_type<TraitsType, response_descriptor>;

    template <Traits TraitsType, Extension... E>
    using simple_response = simple_response_pack<TraitsType, extension_pack<E...>>;


} // namespace webpp::http
#endif // WEBPP_HTTP_RESPONSE_H
