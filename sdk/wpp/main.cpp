#include <webpp/std/utility.hpp>
#include <wsdk/cmds/command.hpp>

inline constexpr auto log_cat = "main";

int main(int const argc, char const** argv) {
    using webpp::sdk::command_manager;
    using webpp::sdk::output_port;
    using webpp::sdk::stdout_output_port;

    auto const output = std::make_shared<stdout_output_port>();
    try {
        command_manager manager{output};
        auto const      cmd_res = manager.run_command(argc, argv);
        logger.info(log_cat, to_string(cmd_res));
        return to_exit_status(cmd_res);
    } catch (std::exception const& ex) {
        logger.critical(log_cat, "This exception has propagated to the top.", ex);
        return EXIT_FAILURE;
    } catch (...) {
        logger.critical(log_cat, "This unknown error has propagated to the top.");
        return EXIT_FAILURE;
    }
}
