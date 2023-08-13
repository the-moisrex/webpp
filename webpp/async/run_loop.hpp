// Created by moisrex on 8/9/23.

#ifndef WEBPP_RUN_LOOP_HPP
#define WEBPP_RUN_LOOP_HPP

#include "./async.hpp"

namespace webpp::async {


    /**
     * Run Loop
     * This class stores, and runs the [possibly chained] sub-tasks.
     * This class doesn't know if the tasks have sub-tasks of their own.
     */
    struct basic_run_loop {

        /// start running
        constexpr void operator()() noexcept {
            // todo
        }

        /// Schedule a new task(-chain) to be run
        template <typename TaskT>
        constexpr basic_run_loop& operator+=(TaskT&& task) noexcept {
            // todo
            return *this;
        }

      private:
    };

} // namespace webpp::async

#endif // WEBPP_RUN_LOOP_HPP
