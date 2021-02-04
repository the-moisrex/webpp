// Created by moisrex on 2/4/21.

#ifndef WEBPP_HEADER_EXTENSIONS_HPP
#define WEBPP_HEADER_EXTENSIONS_HPP

#include "../../extensions/extension.hpp"

namespace webpp::http {

    namespace details {
        template <typename RequestHeaders>
        struct request_headers_child_extension : public RequestHeaders {
            using RequestHeaders::RequestHeaders;

            constexpr stl::size_t content_length() const noexcept {
                // todo
            }
        };
    } // namespace details

    struct request_headers_extensions {

        template <Traits TraitsType, typename Parent>
        using request_headers_extensions =
          webpp::extension_pack<details::request_headers_child_extension<Parent>>;
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_EXTENSIONS_HPP
