#ifndef WEBPP_VALVES_METHODS_H
#define WEBPP_VALVES_METHODS_H

#include "route.hpp"

namespace webpp::http {

    struct method_route_condition : public stl::string_view {
        using stl::string_view::basic_string_view; // ctors

        template <HTTPRequest ReqT>
        [[nodiscard]] constexpr bool operator()(ReqT const& req) const noexcept {
            return req.method() == *this;
        }

        template <istl::String StrT = stl::string>
        void append_name_to(StrT& out) const {
            append_to(out, *this);
        }
    };

    using method = route<method_route_condition>;

    constexpr auto get     = method("GET");
    constexpr auto post    = method("POST");
    constexpr auto head    = method("HEAD");
    constexpr auto put     = method("PUT");
    constexpr auto patch   = method("PATCH");
    constexpr auto del     = method("DELETE");
    constexpr auto options = method("OPTIONS");

    inline namespace literals {
        constexpr auto operator""_method(const char* str, std::size_t len) noexcept {
            return method(str, len);
        }
    } // namespace literals

} // namespace webpp::http


#endif // WEBPP_VALVES_METHODS_H
