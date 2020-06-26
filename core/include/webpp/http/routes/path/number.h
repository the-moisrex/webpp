//
// Created by moisrex on 6/19/20.
//

#ifndef WEBPP_NUMBER_H
#define WEBPP_NUMBER_H

#include "../../../std/optional.h"
#include "../../../std/string_view.h"
#include "../../../validators/validators.h"
#include "../path.h"

#include <type_traits>

namespace webpp::routes {

    struct number {
        stl::string_view variable_name = "";

        [[nodiscard]] bool operator()(Context auto& ctx) const noexcept {
            return is::number(ctx.path.current_segment);
        }

        template <typename T>
        requires(stl::is_integral_v<T> || stl::is_constructible_v<T, long double>)
          [[nodiscard]] stl::optional<T> parse(Context auto const& ctx) const noexcept {
            using context_type     = decltype(ctx);
            using traits_type      = typename context_type::traits_type;
            using string_view_type = typename traits_type::string_view_type;

            auto const str = *ctx.current_segment;

            if constexpr (stl::is_integral_v<T>) {
                return to<T>(str);
            } else if constexpr (stl::is_constructible_v<T, long double>) {
                return T(to<long double>(str));
            }

            return stl::nullopt;
        }
    };

    // todo: other types including:
    // double, float, unsigned, short, ...

} // namespace webpp::routes

#endif // WEBPP_NUMBER_H
