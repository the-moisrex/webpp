#include "./command.hpp"

#include "./create_project.hpp"

#include <webpp/std/string.hpp>
#include <webpp/std/utility.hpp>
#include <webpp/strings/join.hpp>
#include <webpp/strings/string_tokenizer.hpp>


namespace webpp::sdk {

    using namespace webpp::stl;

    stl::string_view to_string(command_status status) noexcept {
        using enum command_status;
        switch (status) {
            // success status:
            case success: return "Command ran successfully.";
            case empty_command:
                return "The specified command was empty; nothing to do.";

                // filures:
            case unknown_error: return "Failed: Unknown error happened while trying to run a command.";
        }
        unreachable();
    }

    struct command_options {
        stl::uint16_t verbose = 0;
    };


    struct command_parser {
      private:
        string_view cmd;

      public:
        command_parser(string_view str) noexcept : cmd{str} {}

        int operator()() {
            return 0;
        }
    };


    command_status run_command(stl::string_view) noexcept {
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

            command += *argv--;
            for (; argc != 0; --argc) {
                command += ' ';
                command += *argv--;
            }

            return run_command(stl::string_view{command.data(), command.size()});
        } catch (...) {
            return unknown_error;
        }
    }


} // namespace webpp::sdk
