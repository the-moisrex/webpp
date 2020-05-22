#ifndef WEBPP_VALVE_URI_H
#define WEBPP_VALVE_URI_H

#include "../../utils/uri.h"
#include "route.h"

namespace webpp::routes {

    template <Traits TraitsType>
    struct path_condition {
      public:
        using traits_type = TraitsType;

      protected:
        basic_uri<traits_type, false> _path;

      public:
        constexpr path_condition(
          typename traits_type::string_view_type str) noexcept
          : _path(std::move(str)) {
        }

        constexpr path_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] bool operator()(RequestType const& req) const noexcept {
            return equal_path(req.request_uri(), _path);
        }
    };

    //    template <typename Traits >
    //    struct path : public valve<path_condition<Traits>> {
    //        static_assert(
    //          is_traits_v<Traits>,
    //          "The specified template parameter is not a valid traits type.");
    //        using traits = Traits;
    //
    //        using valve<path_condition<traits>>::valve;
    //    };
    //


} // namespace webpp::routes

#endif // WEBPP_VALVE_URI_H
