#ifndef WEBPP_VALVE_URI_H
#define WEBPP_VALVE_URI_H

#include "valve.h"

namespace webpp::valves {

    struct path_condition {
      protected:
        std::string_view path_str;

      public:
        constexpr path_condition(std::string_view str) noexcept
            : path_str(std::move(str)) {}

        constexpr path_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] bool operator()(RequestType const& req) const noexcept {
            return req.request_uri() == path_str;
        }
    };

    struct path : public valve<path_condition> {
        using valve<path_condition>::valve;
    };

} // namespace webpp::valves

#endif // WEBPP_VALVE_URI_H
