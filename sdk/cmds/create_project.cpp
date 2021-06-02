#include "create_project.hpp"

using namespace webpp;
using namespace webpp::sdk::cmd;


void create_project::handle() override {

	if (const stl::string proj_name = project_name(); !proj_name.empty()) {
		// the project has a valid name
	} else {
		print_error("Please specify a valid name for the project");
	}

}


