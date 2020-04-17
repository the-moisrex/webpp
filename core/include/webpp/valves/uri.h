#ifndef WEBPP_VALVE_URI_H
#define WEBPP_VALVE_URI_H

#include "../../../include/webpp/utils/uri.h"
#include "valve.h"
#include <cstddef> // for std::size_t
#include <string_view>

namespace webpp::valves {

    struct path_condition {
      protected:
        const_uri _path;

      public:
        constexpr path_condition(std::string_view str) noexcept
            : _path(std::move(str)) {}

        constexpr path_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] bool operator()(RequestType const& req) const noexcept {
            return equal_path(req.request_uri(), _path);
        }
    };

    struct path : public valve<path_condition> {
        using valve<path_condition>::valve;
    };

    constexpr path operator""_path(const char* str, std::size_t len) {
        return path{std::string_view{str, len}};
    }

    // templated path

    /**
     * Check whether or not the specified URI path is a match for the specified
     * template. This function will be used in "tpath_condition". I didn't
     * implement it there because it's a template method and I'd like to
     * abstract away the implementation details.
     * @param templated_path
     * @param _path
     * @return
     */
    [[nodiscard]] bool tpath_check(std::string_view const& templated_path,
                                   std::string_view const& _path) noexcept;

    struct tpath_condition {
      protected:
        std::string_view tpath_str;

      public:
        constexpr tpath_condition(std::string_view str) noexcept
            : tpath_str(str) {}

        constexpr tpath_condition() noexcept = default;

        template <typename RequestType>
        [[nodiscard]] inline bool operator()(RequestType const& req) const
            noexcept {
            return tpath_check(tpath_str, req.request_uri());
        }
    };

    struct tpath : public valve<tpath_condition> {
        using valve<tpath_condition>::valve;
    };

    constexpr tpath operator""_tpath(const char* str, std::size_t len) {
        return tpath{std::string_view{str, len}};
    }

} // namespace webpp::valves

#endif // WEBPP_VALVE_URI_H
