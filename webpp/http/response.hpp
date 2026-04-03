#ifndef WEBPP_HTTP_RESPONSE_HPP
#define WEBPP_HTTP_RESPONSE_HPP

#include "../strings/append.hpp"
#include "./header_fields.hpp"
#include "./http_concepts.hpp"
#include "./response_body.hpp"
#include "./response_headers.hpp"
#include "./status_code.hpp"

#include <concepts>

namespace webpp::http {


    /**
     * This class owns its data.
     */
    template <typename ResponseHeaderType, typename BodyType>
    struct common_http_response {
        // we're not going to use trait's string type here.
        using body_type           = BodyType;
        using headers_type        = ResponseHeaderType;
        using basic_response_type = common_http_response<ResponseHeaderType, BodyType>;
        using field_type          = typename headers_type::field_type;

        static_assert(HTTPResponseBody<body_type>, "Body is not a valid body type.");
        static_assert(HTTPResponseHeaders<headers_type>, "Header is not a valid header.");

        // NOLINTBEGIN(*-non-private-member-variables-in-classes)
        headers_type headers;
        body_type    body;

        // NOLINTEND(*-non-private-member-variables-in-classes)

        constexpr common_http_response() = default;

        template <typename T>
            requires(stl::constructible_from<body_type, T> && !HTTPResponse<stl::remove_cvref_t<T>> &&
                     !stl::same_as<stl::remove_cvref_t<T>, body_type>)
        explicit constexpr common_http_response(T&& body_obj)
          : headers{},
            body{stl::forward<T>(body_obj)} {}

        explicit constexpr common_http_response(http::status_code code) : headers{code}, body{} {}

        constexpr ~common_http_response()                                        = default;
        constexpr common_http_response(common_http_response const& res) noexcept = default;
        constexpr common_http_response(common_http_response&& res) noexcept      = default;

        constexpr explicit common_http_response(body_type const& inp_body) : headers{}, body(inp_body) {}

        constexpr explicit common_http_response(body_type&& inp_body) : headers{}, body(stl::move(inp_body)) {}

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

        template <typename T>
        [[nodiscard]] static constexpr auto with_body(T&& obj) {
            basic_response_type res{};
            res.set(stl::forward<T>(obj));
            return res;
        }

        // template <typename... Args>
        // [[nodiscard]] constexpr static auto with_headers(Args&&... args) {
        //     return response_type{headers_type{stl::forward<Args>(args)...}};
        // }

        /**
         * Generate a response
         */
        template <typename... Args>
            requires stl::constructible_from<basic_response_type, Args...>
        [[nodiscard]] static constexpr HTTPResponse auto create(Args&&... args) {
            return basic_response_type{stl::forward<Args>(args)...};
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
        explicit constexpr operator T() const {
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

    template <istl::CharType CharT, Allocator AllocT = default_allocator_t<CharT>>
    using simple_response =
      common_http_response<response_headers<header_fields_provider<header_field_of<CharT, AllocT>>>,
                           response_body<CharT, AllocT>>;

    template <istl::CharType CharT, Allocator AllocT = default_allocator_t<CharT>>
    struct basic_response : public simple_response<CharT, AllocT> {
        using common_http_response_type = simple_response<CharT, AllocT>;
        using body_type                 = typename common_http_response_type::body_type;

        using simple_response<CharT, AllocT>::simple_response;
        using simple_response<CharT, AllocT>::operator=;

        constexpr basic_response()                                          = default;
        constexpr basic_response(basic_response const&)                     = default;
        constexpr basic_response(basic_response&&) noexcept                 = default;
        constexpr basic_response& operator=(basic_response const&) noexcept = default;
        constexpr basic_response& operator=(basic_response&&) noexcept      = default;
        constexpr ~basic_response()                                         = default;
    };

    using response = basic_response<char>;

} // namespace webpp::http


#endif // WEBPP_HTTP_RESPONSE_HPP
