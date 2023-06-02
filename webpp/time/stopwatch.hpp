// Created by moisrex on 6/2/23.

#ifndef WEBPP_TIME_STOPWATCH_HPP
#define WEBPP_TIME_STOPWATCH_HPP

#include <chrono>

namespace webpp::time {

    /**
     * A simple facility to measure time intervals with nanoseconds resolution as the default.
     */
    template <typename Duration = std::chrono::nanoseconds,
              typename Clock    = std::chrono::high_resolution_clock>
    struct stopwatch {
        using clock_type = Clock;
        using duration   = Duration;
        using period     = typename duration::period;
        using time_point = typename clock_type::time_point;

        template <typename NDuration>
        constexpr stopwatch(std::chrono::time_point<clock_type, NDuration> time) noexcept
          : _start{std::chrono::time_point_cast<duration>(time)} {}
        constexpr stopwatch() noexcept                            = default;
        constexpr stopwatch(const stopwatch&) noexcept            = default;
        constexpr stopwatch(stopwatch&&) noexcept                 = default;
        constexpr stopwatch& operator=(stopwatch const&) noexcept = default;
        constexpr stopwatch& operator=(stopwatch&&) noexcept      = default;
        constexpr ~stopwatch()                                    = default;

        // Starts (or restarts) the stop watch
        constexpr stopwatch& start() noexcept {
            _start = clock_type::now();
            return *this;
        }

        // Stops or pauses the stopwatch.
        constexpr stopwatch& stop() noexcept {
            clock_type current;
            _elapsed += current - _start;
            return *this;
        }

        // Resets the stopwatch.
        constexpr stopwatch& reset() noexcept {
            _elapsed = 0;
            return *this;
        }

        constexpr stopwatch& reset(time_point time) noexcept {
            _elapsed = 0;
            _start   = time;
            return *this;
        }

        // Resets and starts the stopwatch.
        constexpr stopwatch& restart() noexcept {
            _elapsed = 0;
            _start   = clock_type::now();
            return *this;
        }

        /**
         * Returns the elapsed time since the stopwatch started.
         */
        [[nodiscard]] constexpr duration elapsed() const noexcept {
            return _elapsed + (clock_type::now() - _start);
        }

        /**
         * Returns the number of seconds elapsed since the stopwatch started.
         */
        [[nodiscard]] constexpr auto elapsed_seconds() const noexcept {
            return elapsed() / period::num / period::den;
        }

      private:
        time_point _start = clock_type::now();
        duration   _elapsed{};
    };



} // namespace webpp::time

#endif // WEBPP_TIME_STOPWATCH_HPP
