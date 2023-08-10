// Created by moisrex on 8/9/23.

#ifndef WEBPP_RUN_LOOP_HPP
#define WEBPP_RUN_LOOP_HPP

#include "./async.hpp"

namespace webpp::async {


    struct basic_run_loop {

        /// start running
        constexpr void operator()() noexcept {
            // todo
        }

        template <typename TaskT>
        constexpr basic_run_loop& operator+=(TaskT&& task) noexcept {
            // todo
            return *this;
        }

      private:
    };

} // namespace webpp::async

#endif // WEBPP_RUN_LOOP_HPP
