#ifndef WEBPP_SDK_CMDS_CREATE_HPP
#define WEBPP_SDK_CMDS_CREATE_HPP

#include "command.hpp"

namespace webpp::sdk {

    /**
     * wpp new ...
     */
    struct create_command : command {

        [[nodiscard]] stl::string_view name() const noexcept override;
        [[nodiscard]] stl::string_view desc() const noexcept override;
        command_status                 start(command_options options) override;
    };
} // namespace webpp::sdk

#endif // WEBPP_SDK_CMDS_CREATE_HPP
