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
        constexpr explicit stopwatch(std::chrono::time_point<clock_type, NDuration> time) noexcept
          : _start{std::chrono::time_point_cast<duration>(time)} {}

        constexpr stopwatch() noexcept                            = default;
        constexpr stopwatch(stopwatch const&) noexcept            = default;
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
            _elapsed += clock_type::now() - _start;
            return *this;
        }

        template <typename NDuration>
        constexpr stopwatch& stop(std::chrono::time_point<clock_type, NDuration> time) noexcept {
            _elapsed += std::chrono::time_point_cast<duration>(time) - _start;
            return *this;
        }

        template <typename NDuration>
        constexpr stopwatch& operator+=(std::chrono::time_point<clock_type, NDuration> time) noexcept {
            _elapsed += std::chrono::time_point_cast<duration>(time) - _start;
            return *this;
        }

        template <typename NDuration = duration>
            requires requires(NDuration n_dur, duration dur) { dur.operator+=(n_dur); }
        constexpr stopwatch& operator+=(NDuration dur) noexcept {
            _elapsed += std::chrono::duration_cast<duration>(dur);
            return *this;
        }

        // Resets the stopwatch.
        constexpr stopwatch& reset() noexcept {
            _elapsed = duration{};
            return *this;
        }

        template <typename NDuration>
        constexpr stopwatch& operator=(std::chrono::time_point<clock_type, NDuration> time) noexcept {
            _elapsed = duration{};
            _start   = std::chrono::time_point_cast<duration>(time);
            return *this;
        }

        template <typename NDuration = duration>
            requires requires(NDuration n_dur, duration dur) { dur.operator+=(n_dur); }
        constexpr stopwatch& operator=(NDuration dur) noexcept {
            _elapsed = std::chrono::duration_cast<duration>(dur);
            return *this;
        }

        template <typename NDuration>
        constexpr stopwatch& reset(std::chrono::time_point<clock_type, NDuration> time) noexcept {
            return operator=(time);
        }

        // Resets and starts the stopwatch.
        constexpr stopwatch& restart() noexcept {
            _elapsed = duration{};
            _start   = clock_type::now();
            return *this;
        }

        /**
         * Returns the elapsed time since the stopwatch started.
         */
        [[nodiscard]] constexpr duration elapse_now() const noexcept {
            return clock_type::now() - _start;
        }

        /**
         * Duration till the specified time
         */
        template <typename NDuration>
        [[nodiscard]] constexpr duration elapse(
          std::chrono::time_point<clock_type, NDuration> new_time) const noexcept {
            return new_time - _start;
        }

        /**
         * Get current elapsed duration
         */
        [[nodiscard]] constexpr duration elapsed() const noexcept {
            return _elapsed;
        }

        /**
         * Returns the number of seconds elapsed since the stopwatch started.
         */
        [[nodiscard]] constexpr std::chrono::seconds elapsed_seconds() const noexcept {
            return std::chrono::duration_cast<std::chrono::seconds>(elapsed());
        }

        /**
         * Elapsed now, seconds.
         */
        [[nodiscard]] constexpr std::chrono::seconds elapse_now_seconds() const noexcept {
            return std::chrono::duration_cast<std::chrono::seconds>(elapse_now());
        }

      private:
        time_point _start = clock_type::now();
        duration   _elapsed{};
    };



} // namespace webpp::time

#endif // WEBPP_TIME_STOPWATCH_HPP
