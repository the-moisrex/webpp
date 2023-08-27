// Created by moisrex on 8/27/23.

#ifndef WEBPP_HELP_HPP
#define WEBPP_HELP_HPP

#include "./command.hpp"

namespace webpp::sdk {

    struct help_command final : command {

        help_command()                                        = default;
        help_command(help_command const&)                     = delete;
        help_command(help_command&&) noexcept                 = default;
        help_command& operator=(help_command const&) noexcept = delete;
        help_command& operator=(help_command&&) noexcept      = default;
        ~help_command() override                              = default;

        /// Get the command's name
        [[nodiscard]] stl::string_view name() const noexcept override;

        /// Get the command's description
        [[nodiscard]] stl::string_view desc() const noexcept override;

        // individual helps
        static command_status help_all(command_options);
        static command_status help_root_commands(command_options&);

        command_status start(command_options) override;
    };

} // namespace webpp::sdk

#endif // WEBPP_HELP_HPP
