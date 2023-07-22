// Created by moisrex on 6/19/20.

#ifndef WEBPP_HTTP_ROUTES_NUMBER_HPP
#define WEBPP_HTTP_ROUTES_NUMBER_HPP

#include "../../std/string_view.hpp"
#include "../../strings/charset.hpp"
#include "context.hpp"

namespace webpp::http {

    template <typename T = long double>
        requires(stl::is_integral_v<T> || stl::regular<T>)
    struct number {
        template <typename TraitsType>
        [[nodiscard]] bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            using char_type = traits::char_type<TraitsType>;
            // todo: -+ are okay too
            return DIGIT<char_type>.contains(*ctx.path_traverser());
        }

        [[nodiscard]] stl::optional<T> parse(Context auto const& ctx) const noexcept {
            using context_type     = decltype(ctx);
            using traits_type      = typename context_type::traits_type;
            using string_view_type = traits::string_view<traits_type>;

            auto const str = *ctx.current_segment;

            if constexpr (stl::is_integral_v<T>) {
                return to<T>(str);
            } else if constexpr (stl::is_constructible_v<T, long double>) {
                return T(to<long double>(str));
            } else {
                return stl::nullopt;
            }
        }
    };

    // todo: other types including:
    // double, float, unsigned, short, ...

} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTES_NUMBER_HPP
