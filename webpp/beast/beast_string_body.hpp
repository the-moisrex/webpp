// Created by moisrex on 10/16/22.

#ifndef WEBPP_BEAST_STRING_BODY_HPP
#define WEBPP_BEAST_STRING_BODY_HPP

#include "../libs/asio.hpp"
#include "../std/string.hpp"

#include <boost/beast/core/buffers_range.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/message.hpp>

namespace webpp::beast_proto {


    /**
     * A Body using any string type
     *
     * This is the same as boost::beast::http::string_body with the difference that the string type can be
     * specified and you're not bound to using std::basic_string
     */
    template <istl::String StrT>
    struct string_body_of {

        /**
         * The type of container used for the body
         * This determines the type of @ref message::body
         * when this body type is used with a message container.
         */
        using value_type = StrT;

        using char_type       = typename value_type::value_type;
        using pointer         = typename value_type::pointer;
        using const_pointer   = typename value_type::const_pointer;
        using str_traits_type = typename value_type::traits_type; // this is std::char_traits not traits_type

        /**
         * Returns the payload size of the body
         * When this body is used with @ref message::prepare_payload,
         * the Content-Length will be set to the payload size, and
         * any chunked Transfer-Encoding will be removed.
         */
        static std::uint64_t size(value_type const& body) {
            return body.size();
        }

        /**
         * The algorithm for parsing the body
         *
         * Meets the requirements of <em>BodyReader</em>.
         */
        class reader {
            value_type* body_ptr;

          public:
            template <bool isRequest, class Fields>
            explicit reader(boost::beast::http::header<isRequest, Fields>&, value_type& b) : body_ptr(&b) {}

            void init(boost::optional<std::uint64_t> const& length, boost::beast::error_code& ec) {
                if (length) {
                    if (*length > body_ptr->max_size()) {
                        ec = boost::beast::http::error::buffer_overflow;
                        return;
                    }
                    body_ptr->reserve(boost::beast::detail::clamp(*length));
                }
                ec = {};
            }

            template <class ConstBufferSequence>
            std::size_t put(ConstBufferSequence const& buffers, boost::beast::error_code& ec) {
                auto const extra = boost::beast::buffer_bytes(buffers);
                auto const size  = body_ptr->size();
                if (extra > body_ptr->max_size() - size) {
                    ec = boost::beast::http::error::buffer_overflow;
                    return 0;
                }

                body_ptr->resize(size + extra);
                ec           = {};
                pointer dest = &(*body_ptr)[size];
                for (auto b : boost::beast::buffers_range_ref(buffers)) {
                    str_traits_type::copy(dest, static_cast<const_pointer>(b.data()), b.size());
                    dest += b.size();
                }
                return extra;
            }

            void finish(boost::beast::error_code& ec) {
                ec = {};
            }
        };

        /**
         * The algorithm for serializing the body
         *
         * Meets the requirements of <em>BodyWriter</em>.
         */
        class writer {
            value_type const* body_ptr;

          public:
            using const_buffers_type = asio::const_buffer;

            template <bool isRequest, class Fields>
            explicit writer(boost::beast::http::header<isRequest, Fields> const&, value_type const& b)
              : body_ptr(&b) {}

            void init(boost::beast::error_code& ec) {
                ec = {};
            }

            boost::optional<std::pair<const_buffers_type, bool>> get(boost::beast::error_code& ec) {
                ec = {};
                return {{const_buffers_type{body_ptr->data(), body_ptr->size()}, false}};
            }
        };
    };
} // namespace webpp::beast_proto

#endif // WEBPP_BEAST_STRING_BODY_HPP
