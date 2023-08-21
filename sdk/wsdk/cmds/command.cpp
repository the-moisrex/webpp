#include "./command.hpp"

#include "./create_project.hpp"

#include <iterator>
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
    }
    stl::unreachable();
}

struct command_options {
    stl::uint16_t verbose = 0;
};


struct command_parser {
  private:
    stl::string_view cmd;

  public:
    command_parser(stl::string_view str) noexcept : cmd{str} {}

    int operator()() {
        return 0;
    }
};


command_status command_manager::run_command(stl::string_view) noexcept {
    using enum command_status;
    return success;
}

command_status command_manager::run_command(int argc, char const** argv) noexcept {
    using enum command_status;
    if (argc == 0) {
        return empty_command;
    }

    try {
        stl::string command{};

        command += *stl::prev(argv);
        for (; argc != 0; --argc) {
            command += ' ';
            command += *stl::prev(argv);
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
