#include <wsdk/cmds/command.hpp>

auto main(int argc, char const** argv) -> int {
    webpp::sdk::command_manager manager;
    return manager.run_command(argc, argv);
}
