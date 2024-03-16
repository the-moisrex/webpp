#ifndef WEBPP_TIME_CONVERT_HPP
#define WEBPP_TIME_CONVERT_HPP

#include "../common/os.hpp"

#include <chrono>

// todo: test on windows
#ifdef MSVC_COMPILER
#    include <winsock2.h>

#    ifndef _SUSECONDS_T
#        define _SUSECONDS_T
typedef long suseconds_t; // signed # of microseconds in timeval
#    endif                // _SUSECONDS_T

#else
#    include <sys/time.h>
#endif

#include <ctime>

namespace webpp {


    /**
     * Converts a number of microseconds to a relative timeval.
     * @param dur A chrono duration of microseconds.
     */
    [[nodiscard]] static constexpr timeval to_timeval(std::chrono::microseconds const& dur) noexcept {
        using std::chrono::duration_cast;
        using std::chrono::microseconds;
        using std::chrono::seconds;

        seconds const sec = duration_cast<seconds>(dur);

        return timeval{
#ifdef MSVC_COMPILER
          .tv_sec = long{sec.count()},
#else
          .tv_sec = time_t{sec.count()},
#endif
          .tv_usec = suseconds_t{duration_cast<microseconds>(dur - sec).count()}};
    }

    /**
     * Converts a chrono duration to a relative timeval.
     * @param dur A chrono duration.
     * @return A timeval.
     */
    template <typename Rep, typename Period>
    [[nodiscard]] static constexpr timeval to_timeval(
      std::chrono::duration<Rep, Period> const& dur) noexcept {
        return to_timeval(std::chrono::duration_cast<std::chrono::microseconds>(dur));
    }

    /**
     * Converts a relative timeval to a chrono duration.
     * @param tv A timeval.
     * @return A chrono duration.
     */
    template <typename Dur = std::chrono::microseconds>
    [[nodiscard]] static constexpr Dur to_duration(timeval const& tv) noexcept {
        auto const dur = std::chrono::seconds{tv.tv_sec} + std::chrono::microseconds{tv.tv_usec};
        return std::chrono::duration_cast<Dur>(dur);
    }

    /**
     * Converts an absolute timeval to a chrono time_point.
     * @param tv A timeval.
     * @return A chrono time_point.
     */
    template <typename Clock = std::chrono::system_clock>
    [[nodiscard]] static constexpr typename Clock::time_point to_timepoint(timeval const& tv) noexcept {
        return
          typename Clock::time_point{std::chrono::duration_cast<typename Clock::duration>(to_duration(tv))};
    }


} // namespace webpp

#endif // WEBPP_TIME_CONVERT_HPP
