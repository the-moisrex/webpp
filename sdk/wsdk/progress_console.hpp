// Created by moisrex on 8/28/23.

#ifndef WEBPP_PROGRESS_CONSOLE_HPP
#define WEBPP_PROGRESS_CONSOLE_HPP

#include "./progress_indicator.hpp"

namespace webpp::sdk {

    struct console_progress_bar : progress_indicator {
        void update();
    };

} // namespace webpp::sdk

#endif // WEBPP_PROGRESS_CONSOLE_HPP
