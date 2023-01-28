// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_HPP
#define WEBPP_HTTP_BODIES_STRING_HPP

#include "../../common/meta.hpp"
#include "../../configs/constants.hpp"
#include "../../extensions/extension.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../storage/file.hpp"
#include "../../strings/size.hpp"
#include "../http_concepts.hpp"
#include "../routes/router_concepts.hpp"
#include "../status_code.hpp"

#include <filesystem>
#include <fstream>

namespace webpp::http {



    /**
     * This extension helps the user to create a response with the help of the context
     *
     * @code
     *   ctx.string_type{"this is a response"}
     *   ctx.str_t{"this is nice"}
     *   ctx.string("hello world")
     * @endcode
     *
     * The reason for preferring "string" over "string_type" is that the allocator is handled correctly.
     */
    template <Traits TraitsType, Context ContextType>
    struct string_context_extension : public ContextType {
        using context_type  = ContextType;
        using traits_type   = TraitsType;
        using response_type = typename context_type::response_type;
        using body_type     = typename response_type::body_type;
        using string_type   = traits::general_string<traits_type>;

        using context_type::context_type; // inherit the constructors

        template <typename... Args>
        constexpr HTTPResponse auto string(Args&&... args) const {
            return this->response_body(stl::forward<Args>(args)...);
        }

        template <typename StrT, typename... Args>
        constexpr string_type format(StrT&& format_str, Args&&... args) const {
            // todo: it's possible to optimize this for constant expressions
            // todo: should this function return a HTTPResponse instead of string?
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
                return response_type::with_body(result);
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



    /**
     * String Response Extension Pack.
     */
    struct string_body {
        // Add easy to use member functions to use text as a body and generate text-based responses
        using context_extensions = extension_pack<as_extension<string_context_extension>>;
    };


    template <typename T, typename BodyType>
        requires(istl::String<T> || istl::StringView<T>)
    constexpr void deserialize_body(T& str, BodyType const& body) {
        using body_type = stl::remove_cvref_t<BodyType>;
        using byte_type = typename body_type::byte_type;
        using type      = stl::remove_cvref_t<T>;
        if constexpr (istl::String<type>) {
            if constexpr (TextBasedBodyReader<body_type>) {
                str.append(body.data(), body.size());
            } else if constexpr (BlobBasedBodyReader<body_type>) {
                auto const str_size = str.size();
                if constexpr (requires {
                                  str.resize(1);
                                  { body.size() } -> stl::same_as<stl::size_t>;
                              }) {
                    str.resize(str.size() + body.size());
                }
                for (;;) {
                    // CGI supports char type as the "byte type" so it doesn't require casting; even though I
                    // don't think this has any impact on the generated assembly
                    if constexpr (requires { body.read(str.data(), default_buffer_size); }) {
                        stl::streamsize const res = body.read(str.data() + str_size, default_buffer_size);
                        if (res == 0)
                            break;
                    } else {
                        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                        stl::streamsize const res =
                          body.read(reinterpret_cast<byte_type*>(str.data() + str_size), default_buffer_size);
                        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
                        if (res == 0)
                            break;
                    }
                }
            } else if constexpr (requires { body.read(str.data()); }) {
                if constexpr (requires {
                                  str.resize(1);
                                  { body.size() } -> stl::same_as<stl::size_t>;
                              }) {
                    str.resize(str.size() + body.size());
                }
                body.read(str.data());
            } else if constexpr (StreamBasedBodyReader<body_type>) {
                body >> str;
            } else {
                static_assert_false(
                  T,
                  "We're not able to put the body to the string; the body type is unknown to us.");
            }
        } else if constexpr (istl::StringView<T>) {
            if constexpr (TextBasedBodyReader<BodyType>) {
                if constexpr (istl::StringViewifiableOf<type, BodyType>) {
                    str = istl::string_viewify_of<type>(body);
                } else {
                    using char_type = istl::char_type_of<type>;
                    using data_type = char_type const*;

                    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                    str = type{reinterpret_cast<data_type>(body.data()), body.size()};
                    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
                }
            } else {
                static_assert_false(T, "Can't convert non-text-based body types to string view types.");
            }
        } else {
            static_assert_false(
              T,
              "We don't know how to get the string out of the body and append it to the string.");
        }
    }


    // Handle the string literals like `char const*`
    template <typename T, HTTPBody BodyType>
        requires(istl::StringLiteral<T> && TextBasedBodyReader<stl::remove_cvref_t<BodyType>>)
    constexpr stl::remove_cvref_t<T> deserialize_body(BodyType const& body) {
        using type = T;
        if constexpr (stl::same_as<istl::char_type_of<type>, istl::char_type_of<decltype(body.data())>>) {
            return body.data();
        } else {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<type>(body.data());
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }
    }


    // This function will require a request/response body. The request or response objects should pass their
    // own body to this function.
    template <typename T, HTTPBody BodyType>
        requires(istl::String<T> || istl::StringView<T>)
    constexpr stl::remove_cvref_t<T> deserialize_body(BodyType const& body) {
        using type = T;
        if constexpr (istl::String<type> && EnabledTraits<BodyType> &&
                      istl::StringifiableOf<type, BodyType>) {
            return istl::stringify_of<type>(body, alloc::general_alloc_for<type>(body));
        } else if constexpr (istl::String<type> && EnabledTraits<BodyType> &&
                             requires {
                                 requires alloc::HasAllocatorFor<
                                   type,
                                   traits::allocator_pack_type<typename BodyType::traits_type>>;
                             }) {
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
                type str;
                deserialize_body(str, body);
                return str;
            }
        } else {
            static_assert_false(T, "We don't know how to get the string out of the body.");
        }
    }


    template <istl::StringViewifiable T, HTTPBody BodyType>
    constexpr void serialize_body(T&& str, BodyType& body) {
        using body_type     = stl::remove_cvref_t<BodyType>;
        auto const str_view = istl::string_viewify(str);
        if constexpr (TextBasedBodyWriter<body_type>) {
            body.append(str_view.data(), str_view.size());
        } else if constexpr (BlobBasedBodyWriter<body_type>) {
            using byte_type = typename body_type::byte_type;
            // CGI supports writing "byte type"s as "char type"s; so we can skip the casting even though that
            // probably won't affect much, but it saves us 2 castings that happen because of this.
            if constexpr (requires(stl::streamsize s) { body.write(str_view.data(), s); }) {
                body.write(str_view.data(), static_cast<stl::streamsize>(str_view.size()));
            } else {
                // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                body.write(reinterpret_cast<const byte_type*>(str_view.data()),
                           static_cast<stl::streamsize>(str_view.size()));
                // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            }
        } else if constexpr (StreamBasedBodyWriter<body_type>) {
            body << str_view;
        } else {
            static_assert_false(body_type, "The body type doesn't support strings.");
        }
    }

    template <istl::StringViewifiable T, HTTPResponse ResponseType>
    constexpr void serialize_response_body(T&& str, ResponseType& res) {
        res.headers.emplace_back("Content-Type", ascii::size(str));
        serialize_body(stl::forward<T>(str), res.body);
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_HPP
