#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include "command_options.hpp"

#include <vector>
#include <webpp/std/string_view.hpp>

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
    std::string_view to_string(command_status status) noexcept;
    int              to_exit_status(command_status) noexcept;

    struct command {
        command()                                   = default;
        command(command const&)                     = delete;
        command(command&&) noexcept                 = default;
        command& operator=(command const&) noexcept = delete;
        command& operator=(command&&) noexcept      = default;
        virtual ~command()                          = default;

        /// Get the command's name
        [[nodiscard]] virtual std::string_view name() const noexcept = 0;

        /// Get the command's description
        [[nodiscard]] virtual std::string_view desc() const noexcept = 0;


        virtual command_status start(command_options) = 0;
    };

    /**
     * Command Manager
     * This class manages all the commands, you should use this class
     * to run your commands
     */
    struct command_manager {
        command_manager(std::shared_ptr<output_port> inp_output = std::make_shared<stdout_output_port>());
        command_manager(command_manager&&) noexcept            = default;
        command_manager(command_manager const&)                = delete;
        command_manager& operator=(command_manager&&) noexcept = default;
        command_manager& operator=(command_manager const&)     = delete;
        ~command_manager()                                     = default;

        // parse the args, and run the command
        command_status run_command(int argc, char const** argv);

        // run a command from a string view
        command_status run_command(std::string_view);

      private:
        std::shared_ptr<output_port> output;
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CMD
