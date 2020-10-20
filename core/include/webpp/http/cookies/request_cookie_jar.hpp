// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_COOKIE_JAR_HPP
#define WEBPP_REQUEST_COOKIE_JAR_HPP

#include "./cookie_jar.hpp"
#include "./request_cookie.hpp"

namespace webpp {


    template <typename Allocator              = std::allocator<request_cookie<stl::string_view>>,
              istl::StringView StringViewType = stl::string_view>
    struct request_cookie_jar
      : public basic_cookie_jar<
          request_cookie<StringViewType>,
          typename stl::allocator_traits<Allocator>::template rebind_alloc<request_cookie<StringViewType>>> {
        using cookie_type    = request_cookie<StringViewType>;
        using allocator_type = Allocator;
        using super          = basic_cookie_jar<cookie_type, allocator_type>;


        template <typename... Args>
        constexpr request_cookie_jar(Args&&... args) noexcept(noexcept(super(stl::forward<Args>(args)...)))
          : super{stl::forward<Args>(args)...} {}

        /**
         * Parse Cookie header value
         */
        explicit request_cookie_jar(istl::StringViewfiable auto&& source) noexcept {
            // todo
        }
    };


} // namespace webpp

#endif // WEBPP_REQUEST_COOKIE_JAR_HPP
