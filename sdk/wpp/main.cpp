#include <wsdk/cmds/command.hpp>

auto main(int argc, char const** argv) -> int {
    using namespace webpp::sdk;
    command_manager manager;
    return to_exit_status(manager.run_command(argc, argv));
}
