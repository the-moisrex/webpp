// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_COOKIE_JAR_HPP
#define WEBPP_REQUEST_COOKIE_JAR_HPP

#include "../../strings/parser_utils.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "cookie_jar.hpp"
#include "request_cookie.hpp"

namespace webpp::http {


    template <typename Allocator              = std::allocator<request_cookie<stl::string_view>>,
              istl::StringView StringViewType = stl::string_view>
    struct request_cookie_jar
      : public basic_cookie_jar<
          request_cookie<StringViewType>,
          typename stl::allocator_traits<Allocator>::template rebind_alloc<request_cookie<StringViewType>>> {
        using cookie_type      = request_cookie<StringViewType>;
        using allocator_type   = Allocator;
        using super            = basic_cookie_jar<cookie_type, allocator_type>;
        using string_view_type = StringViewType;
        using char_type        = typename string_view_type::value_type;

        template <typename... Args>
        constexpr request_cookie_jar(Args&&... args) noexcept(noexcept(super(stl::forward<Args>(args)...)))
          : super{stl::forward<Args>(args)...} {}

        /**
         * Parse Cookie header value
         */
        explicit request_cookie_jar(string_view_type source) noexcept {
            parse(source);
        }

        bool parse(string_view_type source) noexcept {
            do {
                string_view_type name;
                string_view_type value;
                bool             is_valid;
                details::parse_SE_value(source, name, value, is_valid);
                if (!is_valid) {
                    return false;
                }
                this->emplace_back(name, value);
                ascii::lremove(source, charset(';'));
            } while (!source.empty());
            return true;
        }
    };


} // namespace webpp::http

#endif // WEBPP_REQUEST_COOKIE_JAR_HPP
