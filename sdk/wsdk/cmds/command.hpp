#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include "root_commands.hpp"

#include <webpp/std/string_view.hpp>
#include <webpp/std/vector.hpp>


namespace webpp::sdk {

    enum struct command_status {
        // success status:
        success,       // done
        empty_command, // empty command

        // failures:
        unknown_error,
        invalid_command
    };

    /// Get the string message of the command status
    stl::string_view to_string(command_status status) noexcept;
    int              to_exit_status(command_status) noexcept;

    struct command {

        command()                                   = default;
        command(command const&)                     = delete;
        command(command&&) noexcept                 = default;
        command& operator=(command const&) noexcept = delete;
        command& operator=(command&&) noexcept      = default;
        virtual ~command()                          = default;

        /// Get the command's name
        [[nodiscard]] virtual stl::string_view name() const noexcept = 0;

        /// Get the command's description
        [[nodiscard]] virtual stl::string_view desc() const noexcept = 0;


        virtual command_status start(command_options) = 0;
    };

    /**
     * Command Manager
     * This class manages all the commands, you should use this class
     * to run your commands
     */
    struct command_manager {

        // parse the args, and run the command
        command_status run_command(int argc, char const** argv) noexcept;

        // run a command from a string view
        command_status run_command(stl::string_view) noexcept;
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CMD
