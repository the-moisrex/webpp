#include "./command.hpp"

#include "./create.hpp"
#include "./help.hpp"

#include <iterator>
#include <webpp/strings/join.hpp>

inline constexpr auto log_cat = "command";

using namespace webpp::sdk;
using namespace webpp;

command_manager::command_manager(stl::shared_ptr<output_port> inp_output, dynamic_logger inp_logger)
  : output{stl::move(inp_output)},
    logger{stl::move(inp_logger)} {}

stl::string_view webpp::sdk::to_string(command_status status) noexcept {
    using enum command_status;
    switch (status) {
        // success status:
        case success: return "Command ran successfully.";
        case empty_command:
            return "The specified command was empty; nothing to do.";

            // failures:
        case unknown_error: return "Failed: Unknown error happened while trying to run a command.";
        case invalid_command: return "Failed: the specified command is invalid.";
    }
    stl::unreachable();
}

command_status command_manager::run_command(stl::string_view cmd_str) {
    using enum command_status;

    command_options cmd{cmd_str, output, logger};

    // extract the command from the arguments
    if (cmd.tokenizer().next(WHITESPACES)) {
        auto const root_cmd_str = cmd.tokenizer().token();
        if (root_cmd_str == "create" || root_cmd_str == "new") {
            create_command crt_cmd;
            return crt_cmd.start(stl::move(cmd));
        } else if (root_cmd_str == "help") {
            help_command help_cmd;
            return help_cmd.start(stl::move(cmd));
        } else {
            this->logger.error(
              log_cat,
              stl::format("The string '{}' in the specified command '{}' is not a valid root command.",
                          root_cmd_str,
                          cmd_str));
            return invalid_command;
        }

    } else {
        this->logger.warning(log_cat, "You've tried to run an empty command that does nothing.");
        return empty_command;
    }
}

command_status command_manager::run_command(int argc, char const** argv) {
    using enum command_status;
    if (argc <= 1) {
        return empty_command;
    }

    stl::advance(argv, 1); // skip the first one
    --argc;
    try {
        stl::string command{};

        command += *argv;
        stl::advance(argv, 1); // next argument
        --argc;

        for (; argc != 0 && *argv; --argc) {
            command += ' ';
            command += *argv;
            stl::advance(argv, 1); // next argument
        }

        return run_command(stl::string_view{command.data(), command.size()});
    } catch (...) {
        this->logger.error(log_cat, "Unknown Error while handling the command line arguments.");
        return unknown_error;
    }
}

int webpp::sdk::to_exit_status(command_status status) noexcept {
    using enum command_status;
    switch (status) {
        case success: return EXIT_SUCCESS;
        default: return EXIT_FAILURE;
    }
}
