#include <webpp/logs/dynamic_logger.hpp>
#include <wsdk/cmds/command.hpp>

inline constexpr auto log_cat = "main";

auto main(int argc, char const** argv) -> int {
    using namespace webpp::sdk;

    webpp::dynamic_logger const logger;
    try {
        command_manager manager{logger};
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
