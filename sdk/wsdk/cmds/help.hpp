// Created by moisrex on 8/27/23.

#ifndef WEBPP_HELP_HPP
#define WEBPP_HELP_HPP

#include "./command.hpp"

namespace webpp::sdk {

    command_status help_cmd(command_options options);
    command_status help_all(command_options options);
    command_status help_root_commands(command_options& options);

} // namespace webpp::sdk

#endif // WEBPP_HELP_HPP
