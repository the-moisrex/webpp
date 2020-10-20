// Created by moisrex on 5/31/20.

#ifndef WEBPP_FORMAT_H
#define WEBPP_FORMAT_H

#include "./std.hpp"
#include "../libs/fmt.hpp"

#if __cpp_lib_format
#    include <format>
#    define FMT_COMPILE(formatted_string) (formatted_string)
#elif __has_include(fmt(format))
#    include fmt(format)
#    include fmt(printf)
#    include fmt(chrono)
#    include fmt(compile)
#    define WEBPP_FMT_LIB 1
namespace webpp::stl {

    template <typename... Args>
    inline auto format(Args&&... args) {
        return fmt::format(stl::forward<Args>(args)...);
    }

    template <typename... Args>
    inline auto format_to(Args&&... args) {
        return fmt::format_to(stl::forward<Args>(args)...);
    }

} // namespace webpp::std
#else
#    error "We don't have access to <format> nor {fmt} library."
#endif


namespace webpp::istl {
#if WEBPP_FMT_LIB
    template <typename...Args>
    inline auto safe_localtime(Args&& ...args) {
        return ::fmt::localtime(stl::forward<Args>(args)...);
    }

#else

    // A fallback for when the fmt library is not available

namespace details {

// Prevents expansion of a preceding token as a function-style macro.
// Usage: f FMT_NOMACRO()
#define WEBPP_FMT_NOMACRO

    template <typename T = void> struct null {};
    inline null<> localtime_r WEBPP_FMT_NOMACRO(...) { return null<>(); }
    inline null<> localtime_s(...) { return null<>(); }

}
// Thread-safe replacement for std::localtime
inline std::tm safe_localtime(std::time_t time) {
  struct dispatcher {
    std::time_t time_;
    std::tm tm_;

    dispatcher(std::time_t t) : time_(t) {}

    bool run() {
      using namespace details;
      return handle(localtime_r(&time_, &tm_));
    }

    bool handle(std::tm* tm) { return tm != nullptr; }

    bool handle(details::null<>) {
      using namespace details;
      return fallback(localtime_s(&tm_, &time_));
    }

    bool fallback(int res) { return res == 0; }

#if !_MSC_VER
    bool fallback(details::null<>) {
      using namespace fmt::detail;
      std::tm* tm = std::localtime(&time_);
      if (tm) tm_ = *tm;
      return tm != nullptr;
    }
#endif
  };
  dispatcher lt(time);
  // Too big time values may be unsupported.
  if (!lt.run()) throw stl::invalid_argument("time_t value out of range");
  return lt.tm_;
}

inline std::tm safe_localtime(
    std::chrono::time_point<std::chrono::system_clock> time_point) {
  return localtime(std::chrono::system_clock::to_time_t(time_point));
}
#endif
}


#endif // WEBPP_FORMAT_H
