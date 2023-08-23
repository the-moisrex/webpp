#include "./command.hpp"

#include "./create.hpp"

#include <iterator>
#include <webpp/std/memory.hpp>
#include <webpp/std/string.hpp>
#include <webpp/std/utility.hpp>
#include <webpp/strings/join.hpp>
#include <webpp/strings/string_tokenizer.hpp>


using namespace webpp::sdk;
using namespace webpp;

stl::string_view to_string(command_status status) noexcept {
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


struct command_parser {
  private:
    stl::string_view cmd;

  public:
    command_parser(stl::string_view str) noexcept : cmd{str} {}

    int operator()() {
        return 0;
    }
};


command_status command_manager::run_command(stl::string_view cmd_str) {
    using enum command_status;

    command_options cmd{cmd_str};

    // extract the command from the arguments
    if (cmd.tokenizer().next(WHITESPACES)) {

        auto const root_cmd_str = cmd.tokenizer().token();
        if (root_cmd_str == "create" || root_cmd_str == "new") {
            create_command crt_cmd;
            return crt_cmd.start(stl::move(cmd));
        } else {
            return invalid_command;
        }

    } else {
        return empty_command;
    }
}

command_status command_manager::run_command(int argc, char const** argv) {
    using enum command_status;
    if (argc == 0) {
        return empty_command;
    }

    stl::advance(argv, 1); // skip the first one
    try {
        stl::string command{};

        if (*argv) {
            command += *argv;
            for (; argc != 0 && *argv; --argc) {
                command += ' ';
                command += *argv;
                stl::advance(argv, 1); // next argument
            }
        }

        return run_command(stl::string_view{command.data(), command.size()});
    } catch (...) {
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
