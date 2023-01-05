// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../configs/constants.hpp"
#include "../../extensions/extension.hpp"
#include "../../memory/object.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string_view.hpp"
#include "../../storage/file.hpp"
#include "../../strings/istring.hpp"
#include "../../traits/traits.hpp"
#include "../http_concepts.hpp"
#include "../routes/router_concepts.hpp"
#include "../status_code.hpp"

#include <filesystem>
#include <fstream>

namespace webpp::http {


    namespace details {


        template <Traits TraitsType>
        struct string_body_extension {
            using traits_type      = TraitsType;
            using string_view_type = traits::string_view<traits_type>;
            using string_type      = traits::general_string<traits_type>;
            using allocator_type   = typename string_type::allocator_type;
            using char_type        = traits::char_type<traits_type>;

            constexpr string_body_extension() = default;
            // todo: add "replace_format"
            // todo: add istring member functions to it
            // todo: operator >> and <<

            // load a file as a string for the body
            bool load(stl::filesystem::path const& filepath) noexcept {
                auto result = object::make_general<string_type>(*this);

                bool const res = file::get_to(filepath, result);
                if (res) {
                    // read the file successfully
                    *this = stl::move(result);
                }
                return res;
            }
        };


        /**
         * This extension helps the user to create a response with the help of the context
         *
         *   ctx.string_type{"this is a response"}
         *   ctx.str_t{"this is nice"}
         *   ctx.string("hello world")
         *
         * The reason for preferring "string" over "string_type" is that the allocator is handled correctly.
         */
        template <Traits TraitsType, Context ContextType>
        struct string_context_extension : public ContextType {
            using context_type  = ContextType;
            using traits_type   = TraitsType;
            using response_type = typename context_type::response_type;
            using body_type     = typename response_type::body_type;
            using char_type     = traits::char_type<traits_type>;
            using string_type   = typename body_type::string_type;

            using context_type::context_type; // inherit the constructors

            template <typename... Args>
            constexpr HTTPResponse auto string(Args&&... args) const {
                // check if there's an allocator in the args:
                constexpr bool has_allocator = (Allocator<Args> || ...);
                if constexpr (!has_allocator && requires {
                                  response_type::with_body(
                                    stl::forward<Args>(args)...,
                                    this->alloc_pack.template general_allocator<char_type>());
                              }) {
                    return response_type::with_body(stl::forward<Args>(args)...,
                                                    this->alloc_pack.template general_allocator<char_type>());
                } else {
                    return response_type::with_body(stl::forward<Args>(args)...);
                }
            }

            template <typename StrT, typename... Args>
            constexpr string_type format(StrT&& format_str, Args&&... args) const {
                // todo: it's possible to optimize this for constant expressions
                string_type str{alloc::general_alloc_for<string_type>(*this)};
                fmt::vformat_to(stl::back_inserter(str),
                                istl::to_std_string_view(format_str),
                                fmt::make_format_args(stl::forward<Args>(args)...));
                return str;
            }

            // load a file as a string body and return a response
            [[nodiscard]] response_type file(stl::filesystem::path const& filepath) noexcept {
                auto result = object::make_general<string_type>(*this);

                bool const res = file::get_to(filepath, result);
                if (res) {
                    // read the file successfully
                    return response_type{body_type{result}};
                } else {

                    this->logger.error("Response/File",
                                       fmt::format("Cannot load the specified file: {}", filepath.string()));
                    // todo: retry feature
                    if constexpr (context_type::is_debug()) {
                        return this->error(http::status_code::internal_server_error);
                    } else {
                        return this->error(
                          http::status_code::internal_server_error,
                          fmt::format("We're not able to load the specified file: {}", filepath.string()));
                    }
                }
            }
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     */
    struct string_body {

        // implement TextBasedBodyCommunication concept
        using request_body_extensions  = extension_pack<as_extension<details::string_body_extension>>;
        using response_body_extensions = extension_pack<as_extension<details::string_body_extension>>;

        // Add easy to use member functions to use text as a body and generate text-based responses
        using context_extensions = extension_pack<as_extension<details::string_context_extension>>;
    };


    template <typename T, typename BodyType>
        requires(istl::String<T> || istl::StringView<T>)
    constexpr void deserialize_body(T& str, BodyType const& body) {
        using type = stl::remove_cvref_t<T>;
        if constexpr (istl::String<type>) {
            if constexpr (requires {
                              body.data();
                              body.size();
                          }) {
                str.append(body.data(), body.size());
            } else if constexpr (requires(stl::streamsize count) { body.read(str.data(), count); }) {
                auto const str_size = str.size();
                if constexpr (requires {
                                  str.resize(1);
                                  { body.size() } -> stl::same_as<stl::size_t>;
                              }) {
                    str.resize(str.size() + body.size());
                }
                for (;;) {
                    stl::streamsize res = body.read(str.data() + str_size, default_buffer_size);
                    if (res == 0)
                        break;
                }
            } else if constexpr (requires { body.read(str.data()); }) {
                if constexpr (requires {
                                  str.resize(1);
                                  { body.size() } -> stl::same_as<stl::size_t>;
                              }) {
                    str.resize(str.size() + body.size());
                }
                body.read(str.data());
            } else {
                static_assert_false(
                  T,
                  "We're not able to put the body to the string; the body type is unknown to us.");
            }
        } else if constexpr (istl::StringView<T>) {
            if constexpr (istl::StringViewifiableOf<type, BodyType>) {
                str = istl::string_viewify_of<type>(body);
            } else {
                static_assert_false(T,
                                    "We have to read and store the body into a string container, "
                                    "a string view can't allocate memory for the body.");
            }
        } else {
            static_assert_false(
              T,
              "We don't know how to get the string out of the body and append it to the string.");
        }
    }


    // This function will require a request/response body. The request or response objects should pass their
    // own body to this function.
    template <typename T, HTTPBody BodyType>
        requires(istl::String<T> || istl::StringView<T>)
    constexpr stl::remove_cvref_t<T> deserialize_body(BodyType const& body) {
        using type = stl::remove_cvref_t<T>;
        if constexpr (istl::String<type> && EnabledTraits<BodyType> &&
                      istl::StringifiableOf<type, BodyType>) {
            return istl::stringify_of<type>(body, alloc::general_alloc_for<type>(body));
        } else if constexpr (istl::String<type> && EnabledTraits<BodyType>) {
            type str{alloc::general_alloc_for<type>(body)};
            deserialize_body(str, body);
            return str;
        } else if constexpr (istl::String<type> && stl::is_default_constructible_v<type>) {
            type str;
            deserialize_body(str, body);
            return str;
        } else if constexpr (istl::StringView<T>) {
            if constexpr (istl::StringViewifiableOf<type, BodyType>) {
                return istl::string_viewify_of<type>(body);
            } else {
                static_assert_false(T,
                                    "We have to read and store the body into a string container, "
                                    "a string view can't allocate memory for the body.");
            }
        } else {
            static_assert_false(T, "We don't know how to get the string out of the body.");
        }
    }

    template <typename T>
        requires(istl::StringViewifiable<T>)
    constexpr void serialize_body(T&& str, HTTPBody auto& body) {
        using type          = stl::remove_cvref_t<T>;
        auto const str_view = istl::string_viewify(stl::forward<T>(str));
        if constexpr (requires { body.write(str_view.data(), str_view.size()); }) {
            body.write(str_view.data(), static_cast<stl::streamsize>(str_view.size()));
        } else {
            static_assert_false(type, "The body type doesn't support strings.");
        }
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_H
