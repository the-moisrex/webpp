#include "create_project.hpp"

using namespace webpp;
using namespace webpp::sdk::cmd;

struct project_template {
  private:
    stl::string_view name; // template name

  public:
    project_template(stl::string_view inp_name) : name(inp_name) {}

    // check if the project template is a valid template or not
    bool is_valid() const {}
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

    // check if the project's directory exists or not:

    // create the project directory:
    const stl::string_view project_dir;

    tmpl.load_files();
    tmpl.map_files_to_directory(project_dir);
}
