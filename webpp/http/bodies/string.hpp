// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_HPP
#define WEBPP_HTTP_BODIES_STRING_HPP

// note: we should not import body itself here, strings are used directly there
#include "../../common/meta.hpp"
#include "../../configs/constants.hpp"
#include "../../memory/object.hpp"
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
     *
     * todo: remove this, extensions are removed
     */
    template <Traits TraitsType, Context ContextType>
    struct string_context_extension : ContextType {
        using context_type  = ContextType;
        using traits_type   = TraitsType;
        using response_type = typename context_type::response_type;
        using body_type     = typename response_type::body_type;
        using string_type   = traits::string<traits_type>;

        using context_type::context_type; // inherit the constructors

        template <typename... Args>
        constexpr HTTPResponse auto string(Args&&... args) const {
            return this->response_body(stl::forward<Args>(args)...);
        }

        template <typename StrT, typename... Args>
        constexpr string_type format(StrT&& format_str, Args&&... args) const {
            // todo: it's possible to optimize this for constant expressions
            // todo: should this function return a HTTPResponse instead of string?
            string_type str{get_alloc_for<string_type>(*this)};
            fmt::vformat_to(stl::back_inserter(str),
                            istl::to_std_string_view(format_str),
                            fmt::make_format_args(stl::forward<Args>(args)...));
            return str;
        }

        // load a file as a string body and return a response
        [[nodiscard]] response_type file(stl::filesystem::path const& filepath) noexcept {
            auto result = object::make_object<string_type>(*this);

            if (file::get_to(filepath, result)) {
                // read the file successfully
                return this->response_body(result);
            }

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
    };

    ////////////////////////////// Body Deserializer ( Body into Object ) //////////////////////////////



    namespace details {
        template <typename T>
            requires(istl::String<T> || istl::StringView<T>)
        constexpr void deserialize_text_body(T& str, TextBasedBodyReader auto const& body) {
            str.append(body.data(), body.size());
        }

        template <typename T, typename BodyType>
            requires(istl::String<T> && CStreamBasedBodyReader<stl::remove_cvref_t<BodyType>>)
        constexpr void deserialize_cstream_body(T& str, BodyType&& body) {
            using body_type     = stl::remove_cvref_t<BodyType>;
            using byte_type     = typename body_type::byte_type;
            auto const str_size = str.size();

            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            if constexpr (SizableBody<body_type>) {
                str.resize(str.size() + body.size());
                if constexpr (requires { body.read(str.data(), default_buffer_size); }) {
                    static_cast<void>(
                      body.read(str.data() + str_size, stl::numeric_limits<stl::streamsize>::max()));
                } else {
                    auto* byte_data = reinterpret_cast<byte_type*>(str.data() + str_size);
                    static_cast<void>(body.read(byte_data, stl::numeric_limits<stl::streamsize>::max()));
                }
            } else {
                constexpr stl::size_t buffer_size = 1024u;

                // CGI supports char type as the "byte type" so it doesn't require casting; even though I
                // don't think this has any impact on the generated assembly

                stl::streamsize read; // NOLINT(cppcoreguidelines-init-variables)
                stl::size_t     read_total = str_size;
                for (;;) {
                    // todo: if the body is empty, we're doing a useless allocation here, but we don't know if it's empty or not
                    str.resize(read_total + buffer_size);
                    if constexpr (requires { body.read(str.data(), default_buffer_size); }) {
                        read = body.read(str.data() + read_total, buffer_size);
                    } else {
                        read = body.read(reinterpret_cast<byte_type*>(str.data() + read_total), buffer_size);
                    }
                    if (read == 0) {
                        break;
                    }
                    read_total += static_cast<stl::size_t>(read);
                }
                str.resize(read_total);
            }
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        template <typename T>
            requires(istl::String<T>)
        constexpr void deserialize_stream_body(T& str, StreamBasedBodyReader auto const& body) {
            body >> str;
        }

        template <typename T, typename BodyType>
            requires(istl::String<T> || istl::StringView<T>)
        constexpr void deserialize_body_impl(T& str, BodyType&& body) {
            using body_type = stl::remove_cvref_t<BodyType>;
            using type      = T;
            if constexpr (istl::String<type>) {
                if constexpr (UnifiedBodyReader<body_type>) {
                    switch (body.which_communicator()) {
                        using enum communicator_type;
                        case nothing: break;
                        case text_based: {
                            deserialize_text_body(str, body);
                            break;
                        }
                        case stream_based: {
                            deserialize_stream_body(str, body);
                            break;
                        }
                        case cstream_based: {
                            deserialize_cstream_body(str, body);
                            break;
                        }
                        default: stl::unreachable();
                    }
                } else if constexpr (TextBasedBodyReader<body_type>) {
                    deserialize_text_body(str, body);
                } else if constexpr (CStreamBasedBodyReader<body_type>) {
                    deserialize_cstream_body(str, body);
                } else if constexpr (StreamBasedBodyReader<body_type>) {
                    deserialize_stream_body(str, body);
                } else {
                    static_assert_false(
                      T,
                      "We're not able to put the body to the string; the body type is unknown to us.");
                }
            } else if constexpr (istl::StringView<T>) {
                if constexpr (TextBasedBodyReader<body_type>) {
                    if constexpr (UnifiedBodyReader<body_type>) {
                        switch (body.which_communicator()) {
                            using enum communicator_type;
                            case nothing: return;
                            case text_based: break;
                            case cstream_based:
                            case stream_based:
                                throw stl::invalid_argument(
                                  "You're asking us to get the data of a body type while the body doesn't "
                                  "contain "
                                  "a string so we can't get its data to put it in a string view.");
                            default: stl::unreachable();
                        }
                    }
                    if constexpr (istl::StringViewifiableOf<type, body_type>) {
                        str = istl::string_viewify_of<type>(body);
                    } else {
                        using char_type = istl::char_type_of_t<type>;
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


    } // namespace details

    // Handle the string literals like `char const*`
    template <typename T, HTTPBody BodyType>
        requires(istl::StringLiteral<T> && TextBasedBodyReader<stl::remove_cvref_t<BodyType>>)
    constexpr auto tag_invoke(deserialize_body_tag, stl::type_identity<T>, BodyType&& body) {
        using type = T;
        if constexpr (stl::same_as<istl::char_type_of_t<type>, istl::char_type_of_t<decltype(body.data())>>) {
            return stl::forward<BodyType>(body).data();
        } else {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<type>(stl::forward<BodyType>(body).data());
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }
    }

    // This function will require a request/response body. The request or response objects should pass their
    // own body to this function.
    template <typename T, HTTPBody BodyType>
        requires(istl::String<T> || istl::StringView<T>)
    constexpr T tag_invoke([[maybe_unused]] deserialize_body_tag  tag,
                           [[maybe_unused]] stl::type_identity<T> type_ident,
                           BodyType&&                             body) {
        using type = T;
        if constexpr (istl::String<type> && EnabledTraits<BodyType> && istl::StringifiableOf<type, BodyType>)
        {
            return istl::stringify_of<type>(stl::forward<BodyType>(body), get_alloc_for<type>(body));
        } else if constexpr (
          istl::String<type> && EnabledTraits<BodyType> && traits::has_alloc_for<BodyType, type>)
        {
            type str{get_alloc_for<type>(body)};
            details::deserialize_body_impl(str, stl::forward<BodyType>(body));
            return str;
        } else if constexpr (istl::String<type> && stl::is_default_constructible_v<type>) {
            type str;
            details::deserialize_body_impl(str, stl::forward<BodyType>(body));
            return str;
        } else if constexpr (istl::StringView<T>) {
            if constexpr (istl::StringViewifiableOf<type, BodyType>) {
                return istl::string_viewify_of<type>(stl::forward<BodyType>(body));
            } else {
                type str;
                details::deserialize_body_impl(str, stl::forward<BodyType>(body));
                return str;
            }
        } else {
            static_assert_false(T, "We don't know how to get the string out of the body.");
            return {}; // just to get rid of the warning
        }
    }

    ////////////////////////////// Body Serializer ( Object into Body ) //////////////////////////////


    namespace details {

        template <istl::StringView T, TextBasedBodyReader BodyType>
        constexpr void serialize_text_body(T str, BodyType& body) {
            body.append(str.data(), str.size());
        }

        template <istl::StringView T, CStreamBasedBodyReader BodyType>
        constexpr void serialize_cstream_body(T str, BodyType& body) {
            using body_type = stl::remove_cvref_t<BodyType>;
            using byte_type = typename body_type::byte_type;
            // CGI supports writing "byte type"s as "char type"s; so we can skip the casting even though that
            // probably won't affect much, but it saves us 2 castings that happen because of this.
            if constexpr (requires(stl::streamsize s) { body.write(str.data(), s); }) {
                auto* byte_data = str.data();
                auto  size      = static_cast<stl::streamsize>(str.size());
                for (;;) {
                    stl::streamsize ret_size  = body.write(byte_data, size);
                    byte_data                += ret_size;
                    size                     -= ret_size;
                    if (size <= 0) {
                        break;
                    }
                }
            } else {
                // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                auto* byte_data = reinterpret_cast<byte_type const*>(str.data());
                auto  size      = static_cast<stl::streamsize>(str.size());
                for (;;) {
                    stl::streamsize ret_size  = body.write(byte_data, size);
                    byte_data                += ret_size;
                    size                     -= ret_size;
                    if (size <= 0) {
                        break;
                    }
                }
                // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            }
        }

        template <istl::StringView T, StreamBasedBodyReader BodyType>
        constexpr void serialize_stream_body(T str, BodyType& body) {
            body << str;
        }

    } // namespace details

    template <istl::StringViewifiable T, HTTPBody BodyType>
    constexpr void tag_invoke(serialize_body_tag, T&& str, BodyType& body) {
        using body_type     = stl::remove_cvref_t<BodyType>;
        auto const str_view = istl::string_viewify(str);
        if constexpr (UnifiedBodyReader<body_type>) {
            switch (body.which_communicator()) {
                using enum communicator_type;
                case nothing: // nothing in the body, we can set a new string there
                case text_based: {
                    details::serialize_text_body(str_view, body);
                    break;
                }
                case cstream_based: {
                    details::serialize_cstream_body(str_view, body);
                    break;
                }
                case stream_based: {
                    details::serialize_stream_body(str_view, body);
                    break;
                }
                default: stl::unreachable();
            }
        } else if constexpr (TextBasedBodyWriter<body_type>) {
            details::serialize_text_body(str_view, body);
        } else if constexpr (CStreamBasedBodyWriter<body_type>) {
            details::serialize_cstream_body(str_view, body);
        } else if constexpr (StreamBasedBodyWriter<body_type>) {
            details::serialize_stream_body(str_view, body);
        } else {
            static_assert_false(body_type, "The body type doesn't support strings.");
        }
    }

    template <istl::StringViewifiable T, HTTPResponse ResponseType>
    constexpr void tag_invoke(serialize_response_body_tag, T&& str, ResponseType& res) {
        res.headers.set("Content-Length", ascii::size(str));
        serialize_body(stl::forward<T>(str), res.body);
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_HPP
