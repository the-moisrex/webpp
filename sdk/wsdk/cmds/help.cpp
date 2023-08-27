// Created by moisrex on 8/27/23.

#include "./help.hpp"

#include <array>
#include <cstdint>
#include <webpp/std/functional.hpp>
#include <webpp/std/utility.hpp>

inline constexpr auto log_cat = "helps";

using namespace webpp::sdk;
std::string_view help_command::name() const noexcept {
    return "help";
}

std::string_view help_command::desc() const noexcept {
    return "Get the help you need.";
}

enum struct available_helps : std::uint16_t {
    none          = 0x0,
    all           = 0xffffu, // print all help commands
    root_commands = 0x00'01u
};

command_status help_command::start(command_options options) {
    using enum available_helps;

    using helps_integer = stl::underlying_type_t<available_helps>;

    helps_integer helps = stl::to_underlying(none);

    while (options.tokenizer().next(WHITESPACES)) {
        auto const help_token = options.tokenizer().token();

        if (help_token == "all") {
            helps |= stl::to_underlying(all);
        } else if (help_token == "root-commands") {
            helps |= stl::to_underlying(root_commands);
        } else {
            options.logger().error(log_cat,
                                   stl::format("We don't have help documentation for '{}'.", help_token));
        }
    }

    try {

        // define each help command and its corresponding function to call
        static constexpr stl::array<stl::pair<available_helps, command_status (*)(command_options&)>, 1>
          actions{
            stl::pair{root_commands, &help_root_commands} // 1
          };
        while (helps != 0x0) {
            // print all of them
            if (helps & stl::to_underlying(all)) {
                return help_all(stl::move(options));
            } else {

                // check each one of them and print their helps
                for (auto&& [help_cmd, action] : actions) {
                    if (helps & stl::to_underlying(help_cmd)) {
                        // run the help action
                        if (auto const res = stl::invoke(action, options); res != command_status::success) {
                            return res;
                        }
                        helps &= stl::to_underlying(help_cmd); // remove it
                    }
                }
            }
        }
    } catch (...) {
        options.logger().error(log_cat,
                               "unexpected error happened while preparing the help documents for you.");
        std::rethrow_exception(std::current_exception());
    }

    return command_status::success;
}

command_status help_command::help_all(command_options options) {
    using enum command_status;
    for (auto cmd_ptr : {&help_command::help_root_commands}) {
        if (auto res = stl::invoke(cmd_ptr, options); res != success) {
            return res;
        }
    }
    return success;
}

command_status help_command::help_root_commands(command_options& options) {
    using enum command_status;
    using namespace std::string_view_literals;

    static constexpr stl::array<stl::pair<stl::string_view, stl::string_view>, 2> root_commands_table{
      stl::pair{"new / create [something]"sv, "create a new [something]"sv}, // row 1
      {"help [something]", "Get the help"}                                   // row 2
    };

    options.output().send_table("Root Commands", row_view{root_commands_table});
    return success;
}
