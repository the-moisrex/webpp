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



    ////////////////////////////// Body Deserializer ( Body into Object ) //////////////////////////////



    namespace details {
        template <typename CharT, typename AllocT>
        constexpr void deserialize_text_body(stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& str,
                                             TextBasedBodyReader auto const&                            body) {
            str.append(body.data(), body.size());
        }

        template <typename CharT, typename AllocT, typename BodyType>
            requires(CStreamBasedBodyReader<stl::remove_cvref_t<BodyType>>)
        constexpr void deserialize_cstream_body(stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& str,
                                                BodyType&&                                                 body) {
            using body_type     = stl::remove_cvref_t<BodyType>;
            using byte_type     = typename body_type::byte_type;
            auto const str_size = str.size();

            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            if constexpr (SizableBody<body_type>) {
                str.resize(str.size() + body.size());
                if constexpr (requires { body.read(str.data(), default_buffer_size); }) {
                    static_cast<void>(body.read(str.data() + str_size, stl::numeric_limits<stl::streamsize>::max()));
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

        template <typename CharT, typename AllocT>
        constexpr void deserialize_stream_body(stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& str,
                                               StreamBasedBodyReader auto const&                          body) {
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
                        str = istl::view_of<type>(body);
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
                static_assert_false(T,
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
        if constexpr (istl::String<type> && EnabledTraits<BodyType> && istl::StringifiableOf<type, BodyType>) {
            return istl::stringify_of<type>(stl::forward<BodyType>(body), get_alloc_for<type>(body));
        } else if constexpr (istl::String<type> && EnabledTraits<BodyType> && traits::has_alloc_for<BodyType, type>) {
            type str{get_alloc_for<type>(body)};
            details::deserialize_body_impl(str, stl::forward<BodyType>(body));
            return str;
        } else if constexpr (istl::String<type> && stl::is_default_constructible_v<type>) {
            type str;
            details::deserialize_body_impl(str, stl::forward<BodyType>(body));
            return str;
        } else if constexpr (istl::StringView<T>) {
            if constexpr (istl::StringViewifiableOf<type, BodyType>) {
                return istl::view_of<type>(stl::forward<BodyType>(body));
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

        template <typename CharT, TextBasedBodyReader BodyType>
        constexpr void serialize_text_body(stl::basic_string_view<CharT> const str, BodyType& body) {
            body.append(str.data(), str.size());
        }

        template <typename CharT, CStreamBasedBodyReader BodyType>
        constexpr void serialize_cstream_body(stl::basic_string_view<CharT> const str, BodyType& body) {
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

        template <typename CharT, StreamBasedBodyReader BodyType>
        constexpr void serialize_stream_body(stl::basic_string_view<CharT> const str, BodyType& body) {
            body << str;
        }

    } // namespace details

    template <typename CharT, HTTPBody BodyType>
    constexpr void tag_invoke(serialize_body_tag, stl::basic_string_view<CharT> const str, BodyType& body) {
        using body_type = stl::remove_cvref_t<BodyType>;
        if constexpr (UnifiedBodyReader<body_type>) {
            switch (body.which_communicator()) {
                using enum communicator_type;
                case nothing: // nothing in the body, we can set a new string there
                case text_based: {
                    details::serialize_text_body(str, body);
                    break;
                }
                case cstream_based: {
                    details::serialize_cstream_body(str, body);
                    break;
                }
                case stream_based: {
                    details::serialize_stream_body(str, body);
                    break;
                }
                default: stl::unreachable();
            }
        } else if constexpr (TextBasedBodyWriter<body_type>) {
            details::serialize_text_body(str, body);
        } else if constexpr (CStreamBasedBodyWriter<body_type>) {
            details::serialize_cstream_body(str, body);
        } else if constexpr (StreamBasedBodyWriter<body_type>) {
            details::serialize_stream_body(str, body);
        } else {
            static_assert_false(body_type, "The body type doesn't support strings.");
        }
    }

    template <typename CharT, stl::size_t N, HTTPBody BodyType>
    constexpr void tag_invoke(serialize_body_tag, CharT const (&str)[N], BodyType& body) { // NOLINT(*-c-arrays)
        auto const body_size = N > 0 && str[N - 1] == CharT{} ? N - 1 : N;
        tag_invoke(serialize_body_tag{}, stl::basic_string_view<CharT>{str, body_size}, body);
    }

    template <typename CharT, HTTPResponse ResponseType>
    constexpr void tag_invoke(serialize_response_body_tag, stl::basic_string_view<CharT> const str, ResponseType& res) {
        res.headers.set("Content-Length", ascii::size(str));
        serialize_body(str, res.body);
    }

} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_HPP
