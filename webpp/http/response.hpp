#ifndef WEBPP_HTTP_RESPONSE_HPP
#define WEBPP_HTTP_RESPONSE_HPP

#include "../convert/casts.hpp"
#include "../strings/append.hpp"
#include "../traits/traits.hpp"
#include "header_fields.hpp"
#include "http_concepts.hpp"
#include "response_body.hpp"
#include "response_headers.hpp"
#include "status_code.hpp"

namespace webpp::http {


    /**
     * This class owns its data.
     */
    template <typename ResponseHeaderType, typename BodyType>
    struct common_http_response {

        // we're not going to use trait's string type here.
        using body_type           = BodyType;
        using headers_type        = ResponseHeaderType;
        using traits_type         = typename body_type::traits_type;
        using basic_response_type = common_http_response<ResponseHeaderType, BodyType>;
        using field_type          = typename headers_type::field_type;

        static_assert(HTTPResponseBody<body_type>, "Body is not a valid body type.");
        static_assert(HTTPResponseHeaders<headers_type>, "Header is not a valid header.");

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        headers_type headers;
        body_type    body;
        // NOLINTEND(misc-non-private-member-variables-in-classes)



        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, common_http_response>) // It's not a copy/move
        constexpr common_http_response(ET&& et) noexcept(
          stl::is_nothrow_constructible_v<headers_type, ET>&& stl::is_nothrow_constructible_v<body_type, ET>)
          : headers{et},
            body{et} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET, typename T>
        constexpr common_http_response(ET&& et, T&& body_obj)
          : headers{et},
            body{et, stl::forward<T>(body_obj)} {}

        template <EnabledTraits ET>
        constexpr common_http_response(ET&& et, http::status_code code) : headers{et, code},
                                                                          body{et} {}


        constexpr ~common_http_response()                                        = default;
        constexpr common_http_response(common_http_response const& res) noexcept = default;
        constexpr common_http_response(common_http_response&& res) noexcept      = default;

        constexpr explicit common_http_response(body_type const& b) : headers{b.get_traits()}, body(b) {}
        constexpr explicit common_http_response(body_type&& b)
          : headers{b.get_traits()},
            body(stl::move(b)) {}

        constexpr common_http_response& operator=(common_http_response const&)         = default;
        constexpr common_http_response& operator=(common_http_response&& res) noexcept = default;

        // set the status code for the headers
        constexpr common_http_response& operator=(http::status_code code) noexcept {
            headers = code;
            return *this;
        }

        [[nodiscard]] bool operator==(common_http_response const& res) const noexcept {
            return headers == res.headers && body == res.body;
        }

        [[nodiscard]] bool operator!=(common_http_response const& res) const noexcept {
            return headers != res.headers || body != res.body;
        }

        /**
         * The reason why this function exists here and not in "headers" is because this function may need
         * access to the body as well to check the body value's type for example.
         */
        void calculate_default_headers()
            requires(istl::String<typename field_type::string_type>)
        {
            using header_field_type = typename headers_type::field_type;
            using str_t             = typename header_field_type::string_type;

            auto const [has_content_type, has_content_length] = headers.has("content-type", "content-length");

            // todo: use content_type class
            if (!has_content_type) {
                headers.set("Content-Type", "text/html; charset=utf-8");
            }

            if constexpr (SizableBody<body_type>) {
                if (!has_content_length) {
                    str_t value{headers.get_allocator()};
                    append_to(value, body.size() * sizeof(char));
                    headers.set("Content-Length", stl::move(value));
                }
            }
        }


        /**
         * Check if the response is empty or not
         */
        [[nodiscard]] constexpr bool empty() const noexcept {
            return headers.empty() && body.empty();
        }



        template <EnabledTraits T>
        [[nodiscard]] constexpr static auto with_body(T&& obj) {
            return create(obj, obj);
        }

        template <EnabledTraits ET, typename T>
        [[nodiscard]] constexpr static auto with_body(ET&& et, T&& obj) {
            return create(et, obj);
        }

        // template <typename... Args>
        // [[nodiscard]] constexpr static auto with_headers(Args&&... args) {
        //     return response_type{headers_type{stl::forward<Args>(args)...}};
        // }

        /**
         * Generate a response
         */
        template <EnabledTraits ET, typename... Args>
        [[nodiscard]] static constexpr HTTPResponse auto create(ET&& et, Args&&... args) {
            using new_response_type = common_http_response;
            if constexpr (requires { new_response_type{et, stl::forward<Args>(args)...}; }) {
                return new_response_type{et, stl::forward<Args>(args)...};
            } else if constexpr (requires { new_response_type{stl::forward<Args>(args)..., et}; }) {
                return new_response_type{stl::forward<Args>(args)..., et};
            } else {
                return new_response_type{stl::forward<Args>(args)...};
            }
        }


        template <HTTPResponse ResType>
        [[nodiscard]] constexpr bool operator==(ResType const& res) const noexcept {
            return this->headers == res.headers && this->body == res.body;
        }

        template <HTTPResponse ResType>
        [[nodiscard]] constexpr bool operator!=(ResType const& res) const noexcept {
            return !operator==(res);
        }

        // Pass the value to the headers/body depending on the value's type
        template <typename ValueType>
        constexpr common_http_response& operator<<(ValueType&& value) {
            using value_type = stl::remove_cvref_t<ValueType>;
            if constexpr (stl::same_as<value_type, field_type>) {
                // give it to the headers
                this->headers.emplace_back(stl::forward<ValueType>(value));
            } else {
                // give it to the body
                this->body.operator<<(stl::forward<ValueType>(value));
            }
            return *this;
        }


        template <typename T>
        constexpr T as() const {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (DeserializableResponseBody<T, common_http_response>) {
                return deserialize_response_body(stl::type_identity<T>{}, *this);
            } else if constexpr (DeserializableResponseBody<T, body_type>) {
                return deserialize_response_body(stl::type_identity<T>{}, this->body);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                // Calls the `deserialize_body` functions
                return this->body.template as<T>();
            }
        }

        constexpr auto as() const {
            return auto_converter<common_http_response>{*this};
        }

        template <typename T>
            requires(!istl::is_specialization_of_v<stl::remove_cvref_t<T>, common_http_response> &&
                     HTTPConvertibleBody<T, common_http_response, headers_type, body_type>)
        constexpr operator T() const {
            return as<T>();
        }


        template <typename T>
        constexpr common_http_response& add(T&& obj) {
            if constexpr (requires { serialize_response_body(stl::forward<T>(obj), *this); }) {
                serialize_response_body(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_response_body(stl::forward<T>(obj), this->body); }) {
                serialize_response_body(stl::forward<T>(obj), this->body);
            } else {
                // calls `serialize_body` functions
                this->body.template add<T>(stl::forward<T>(obj));
            }
            return *this;
        }

        template <typename T>
        constexpr common_http_response& set(T&& obj) {
            this->body.set(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr common_http_response& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr common_http_response& operator+=(T&& obj) {
            add(stl::forward<T>(obj));
            return *this;
        }
    };





    template <Traits TraitsType>
    using simple_response =
      common_http_response<response_headers<header_fields_provider<header_field_of<TraitsType>>>,
                           response_body<TraitsType>>;



    template <Traits TraitsType = default_dynamic_traits>
    struct basic_response : public simple_response<TraitsType> {
        using common_http_response_type = simple_response<TraitsType>;
        using body_type                 = typename common_http_response_type::body_type;
        using traits_type               = TraitsType;

        using simple_response<TraitsType>::operator=;

        constexpr basic_response(basic_response const&)                     = default;
        constexpr basic_response(basic_response&&) noexcept                 = default;
        constexpr basic_response& operator=(basic_response const&) noexcept = default;
        constexpr basic_response& operator=(basic_response&&) noexcept      = default;
        constexpr ~basic_response()                                         = default;

        template <EnabledTraits ET, typename T>
        constexpr basic_response(ET&& et, T&& body_obj)
          : common_http_response_type{stl::forward<ET>(et), stl::forward<T>(body_obj)} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!HTTPResponse<ET> && !istl::cvref_as<basic_response, ET>)
        explicit constexpr basic_response(ET&& et) : common_http_response_type{stl::forward<ET>(et)} {}


        template <HTTPResponse ResT>
            requires(!istl::cvref_as<basic_response, ResT>)
        explicit constexpr basic_response(ResT&& res) : common_http_response_type{stl::forward<ResT>(res)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr explicit basic_response(body_type const& b) : common_http_response_type{b} {}
    };

    using response = basic_response<default_dynamic_traits>;

} // namespace webpp::http


#endif // WEBPP_HTTP_RESPONSE_HPP
