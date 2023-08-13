#include "create.hpp"

#include "create_project.hpp"

using namespace webpp::sdk;

create_command::create_command() {
    this->add_child<create_project>();
}
