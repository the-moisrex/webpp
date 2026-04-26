#include "./command.hpp"

#include "./create.hpp"
#include "./help.hpp"

#include <iterator>
#include <webpp/strings/join.hpp>

using std::string;
using std::string_view;
using webpp::sdk::command_manager;
using webpp::sdk::command_status;

inline constexpr auto log_cat = "cmd";

command_manager::command_manager(std::shared_ptr<output_port> inp_output) : output{std::move(inp_output)} {}

string_view webpp::sdk::to_string(command_status status) noexcept {
    using enum command_status;
    switch (status) {
        // success status:
        case success: return "Command ran successfully.";
        case empty_command:
            return "The specified command was empty; nothing to do.";

            // failures:
        case unknown_error: return "Failed: Unknown error happened while trying to run a command.";
        case invalid_command: return "Failed: the specified command is invalid.";
        default: break;
    }
    return "<unknown command status>";
}

command_status command_manager::run_command(string_view cmd_str) {
    using enum command_status;
    using std::array;

    command_options cmd{cmd_str, output};

    // extract the command from the arguments
    if (cmd.tokenizer().next(WHITESPACES)) {
        auto const root_cmd_str = cmd.tokenizer().token();
        if (root_cmd_str == "create" || root_cmd_str == "new") {
            return create_cmd(std::move(cmd));
        }
        if (root_cmd_str == "help" || root_cmd_str == "--help") {
            return help_cmd(std::move(cmd));
        }
        logger.error(
          log_cat,
          format("The string '{}' in the specified command '{}' is not a valid root command.", root_cmd_str, cmd_str));
        return invalid_command;
    }

    logger.warning(log_cat, "You've tried to run an empty command that does nothing.");
    return empty_command;
}

command_status command_manager::run_command(int argc, char const** argv) {
    using enum command_status;
    if (argc <= 1) {
        return empty_command;
    }

    std::advance(argv, 1); // skip the first one
    --argc;
    try {
        string command = *argv;
        std::advance(argv, 1); // next argument
        --argc;

        for (; argc != 0 && *argv; --argc) {
            command += ' ';
            command += *argv;
            std::advance(argv, 1); // next argument
        }

        return run_command(string_view{command.data(), command.size()});
    } catch (...) {
        logger.error(log_cat, "Unknown Error while handling the command line arguments.");
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
