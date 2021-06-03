#include "create_project.hpp"

using namespace webpp;
using namespace webpp::sdk::cmd;

struct project_template{
	
};

void create_project::handle() override {
    const stl::string proj_name = project_name();
    if (proj_name.empty()) {
        print_error("Please specify a valid name for the project.");
        return;
    }

    const project_tempplate tmpl = get_project_template();
    if (!tmpl.is_valid()) {
        print_error("The specified template is not valid.");
        return;
    }
}
