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

        template <HTTPResponseBody NewBodyType>
        using rebind_basic_response_type = basic_response<EList, ResponseHeaderType, NewBodyType>;


        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        headers_type headers{};
        body_type    body{};
        // NOLINTEND(misc-non-private-member-variables-in-classes)


        template <typename Arg1, typename... Args>
        constexpr basic_response([[maybe_unused]] Arg1&& arg1, Args&&... args) noexcept
          requires(!istl::part_of<Arg1, headers_type, http::status_code_type, body_type> &&
                   stl::is_constructible_v<elist_type, Args...>)
          : elist_type{stl::forward<Args>(args)...} {}


        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, basic_response> && // It's not a copy/move
                     stl::is_constructible_v<elist_type, ET>)                  // can we give it to elist
        constexpr basic_response(ET&& et) noexcept(stl::is_nothrow_constructible_v<elist_type, ET>&&
                                                       stl::is_nothrow_constructible_v<headers_type, ET>&&
                                                       stl::is_nothrow_constructible_v<body_type, ET>)
          : elist_type{et},
            headers{et},
            body{et} {}


        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, basic_response>) // It's not a copy/move
        constexpr basic_response(ET&& et) noexcept(stl::is_nothrow_default_constructible_v<elist_type>&&
                                                       stl::is_nothrow_constructible_v<headers_type, ET>&&
                                                       stl::is_nothrow_constructible_v<body_type, ET>)
          : elist_type{},
            headers{et},
            body{et} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr basic_response() noexcept                          = default;
        constexpr ~basic_response()                                  = default;
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

        // set the status code for the headers
        constexpr basic_response& operator=(http::status_code code) noexcept {
            headers = code;
            return *this;
        }

        [[nodiscard]] bool operator==(basic_response const& res) const noexcept {
            return headers == res.headers && body == res.body;
        }

        [[nodiscard]] bool operator!=(basic_response const& res) const noexcept {
            return headers != res.headers || body != res.body;
        }

        /**
         * The reason why this function exists here and not in "headers" is because this function may need
         * access to the body as well to check the body value's type for example.
         */
        void calculate_default_headers() noexcept {
            using header_field_type = typename headers_type::field_type;
            using str_t             = typename header_field_type::string_type;

            auto const [has_content_type, has_content_length] = headers.has("content-type", "content-length");

            // todo: we can optimize this, pre-calculate the default header fields and copy when needed
            // todo: use content_type class
            if (!has_content_type) {
                static const header_field_type content_type_field{
                  str_t{"Content-Type", headers.get_allocator()},
                  str_t{"text/html; charset=utf-8", headers.get_allocator()}};
                headers.emplace_back(content_type_field);
            }

            if (!has_content_length) {
                str_t value{headers.get_allocator()};
                append_to(value, body.string().size() * sizeof(char));
                headers.emplace_back(
                  header_field_type{str_t{"Content-Length", headers.get_allocator()}, stl::move(value)});
            }
        }
    };




    template <typename EList>
    struct final_response final : public EList {
      private:
        using elist_type = EList;

      public:
        using traits_type     = typename elist_type::traits_type;
        using root_extensions = typename elist_type::root_extensions;
        using response_type   = final_response;
        using body_type       = typename elist_type::body_type;
        using headers_type    = typename elist_type::headers_type;
        using field_type      = typename headers_type::field_type;
        using string_type     = traits::general_string<traits_type>;


        template <HTTPResponseBody NewBodyType>
        using rebind_response_type =
          final_response<typename elist_type::template rebind_basic_response_type<NewBodyType>>;


        template <HTTPResponseBodyCommunicator NewBodyCommunicator>
        using rebind_response_body_communicator_type = rebind_response_type<
          typename body_type::template rebind_body_communicator_type<NewBodyCommunicator>>;

        using string_response_type = rebind_response_body_communicator_type<string_type>;
        // todo: add stream and blob response types


        static_assert(HTTPResponseBody<body_type>, "Body is not a valid body type.");
        static_assert(HTTPResponseHeaders<headers_type>, "Header is not a valid header.");


        using EList::EList;


        template <typename T>
        [[nodiscard]] constexpr static auto with_body(T&& obj) {
            response_type res = create();
            res               = stl::forward<T>(obj);
            return res;
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


        template <HTTPResponse ResType>
        [[nodiscard]] constexpr bool operator==(ResType const& res) const noexcept {
            return this->headers == res.headers && this->body == this->body;
        }

        template <HTTPResponse ResType>
        [[nodiscard]] constexpr bool operator!=(ResType const& res) const noexcept {
            return !operator==(res);
        }

        // Pass the value to the headers/body depending on the value's type
        template <typename ValueType>
        constexpr response_type& operator<<(ValueType&& value) {
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
            if constexpr (requires {
                              { deserialize_response_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_response_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_response_body<T>(this->body) } -> stl::same_as<T>;
                                 }) {
                return deserialize_response_body<T>(this->body);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(this->body) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(this->body);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        constexpr auto as() const {
            return auto_converter<final_response>{.obj = *this};
        }

        template <typename T>
        constexpr operator T() const {
            return as<T>();
        }


        template <typename T>
        constexpr final_response& set(T&& obj) {
            if constexpr (requires { elist_type::template set<T>(stl::forward<T>(obj)); }) {
                elist_type::template set<T>(stl::forward<T>(obj));
            } else if constexpr (requires { elist_type::template operator=<T>(stl::forward<T>(obj)); }) {
                elist_type::template operator=<T>(stl::forward<T>(obj));
            } else if constexpr (requires { serialize_response_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_response_body<T>(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_response_body<T>(stl::forward<T>(obj), this->body); }) {
                serialize_response_body<T>(stl::forward<T>(obj), this->body);
            } else if constexpr (requires { serialize_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_body<T>(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_body<T>(stl::forward<T>(obj), this->body); }) {
                serialize_body<T>(stl::forward<T>(obj), this->body);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the specified object to a response."
                                    " Did you import the right header?"
                                    " You can always write your own custom body serializer functions.");
            }
            return *this;
        }

        template <typename T>
        constexpr final_response& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }



        template <HTTPResponseBodyCommunicator CommunicatorType, typename T>
        constexpr auto rebind_body(T&& obj) const {
            using new_response_type = rebind_response_body_communicator_type<CommunicatorType>;
            if constexpr (EnabledTraits<final_response>) {
                new_response_type res{this->get_traits()};
                res = stl::forward<T>(obj);
                return res;
            } else {
                new_response_type res;
                res = stl::forward<T>(obj);
                return res;
            }
        }
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
    using simple_response = typename RootExtensions::template extensie_type<TraitsType, response_descriptor>;


} // namespace webpp::http


#endif // WEBPP_HTTP_RESPONSE_HPP
