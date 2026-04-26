// Created by moisrex on 7/15/20.

#ifndef WEBPP_LOG_CONCEPTS_HPP
#define WEBPP_LOG_CONCEPTS_HPP

#include "../common/meta.hpp"
#include "../std/std.hpp"

#include <concepts>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace webpp {

    enum struct [[nodiscard]] log_level : stl::uint8_t {
        trace    = 0,
        debug    = 1,
        info     = 2,
        warn     = 3,
        warning  = 3,
        err      = 4,
        error    = 4,
        critical = 5,
        off      = 6
    };

    [[nodiscard]] static constexpr stl::string_view to_string(log_level const level) noexcept {
        using enum log_level;
        switch (level) {
            case trace: return {"trace"};
            case debug: return {"debug"};
            case info: return {"info"};
            case warn: return {"warn"};
            case err: return {"err"};
            case critical: return {"critical"};
            case off: return {"off"};
            default: break;
        }
        return {"unknown"};
    }

    /**
     * A simple helper to do the logging only if it's being run on debug build
     */
    static constexpr struct [[nodiscard]] if_debug_tag {
        consteval bool operator()() const noexcept {
            return is_debug_build;
        }
    } if_debug;

    template <typename T>
    concept Logger = stl::movable<stl::remove_cvref_t<T>> && requires(stl::remove_cvref_t<T> logger, log_level level) {
        // logger.log(level, "msg");
        logger.log(level, "category", "msg");
        // logger.log(level, if_debug, "category", "msg");
    };


} // namespace webpp

#endif // WEBPP_LOG_CONCEPTS_HPP
