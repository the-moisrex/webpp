#ifndef WEBPP_VALVE_URI_H
#define WEBPP_VALVE_URI_H

#include "../utils/uri.h"
#include "routes.h"

namespace webpp::valves {

    template <typename Traits = std_traits>
    struct path_condition {
      public:
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");
        using traits = Traits;

      protected:
        basic_uri<traits, false> _path;

      public:
        constexpr path_condition(typename traits::string_view_type str) noexcept
          : _path(std::move(str)) {
        }

        constexpr path_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] bool operator()(RequestType const& req) const noexcept {
            return equal_path(req.request_uri(), _path);
        }
    };

    template <typename Traits = std_traits>
    struct path : public valve<path_condition<Traits>> {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");
        using traits = Traits;

        using valve<path_condition<traits>>::valve;
    };



} // namespace webpp::valves

#endif // WEBPP_VALVE_URI_H
