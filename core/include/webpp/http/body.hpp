#ifndef WEBPP_HTTP_BODY_HPP
#define WEBPP_HTTP_BODY_HPP

#include "../common/meta.hpp"
#include "../std/concepts.hpp"

namespace webpp::http {

    /**
     * This little helper will help constructing the request body or response body
     */
    template <typename BodyType>
    struct body_factory : BodyType {
        using body_type = BodyType;

      private:
        template <typename T>
        using ref_t = stl::add_lvalue_reference_t<stl::remove_cvref_t<T>>;

      public:
        // server&
        // For request bodies
        template <typename ServerT>
            requires(stl::is_constructible_v<body_type, ref_t<ServerT>>)
        constexpr body_factory(ServerT& server) noexcept : body_type{server} {}


        // The rest
        // For respones bodies
        template <typename... Args>
            requires(stl::is_constructible_v<body_type, Args...>)
        constexpr body_factory(Args&&... args) noexcept(
          noexcept(stl::is_nothrow_constructible_v<body_type, Args...>))
          : body_type{stl::forward<Args>(args)...} {}
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_BODY_HPP
