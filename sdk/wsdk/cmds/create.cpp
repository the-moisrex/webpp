#include "create.hpp"

#include "create_project.hpp"

using namespace webpp::sdk;

std::string_view create_command::name() const noexcept {
    return "create";
}
std::string_view create_command::desc() const noexcept {
    return "Create a a new ...";
}
command_status create_command::start(command_options options) {
    return command_status::empty_command;
}
