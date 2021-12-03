#include "create_project.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

using namespace webpp;
using namespace webpp::sdk;

// One template
struct project_template {
    stl::string_view name; // template name

    static create_from_file(stl::ifatream file);
};


// the struct that creates the project template's object
struct template_manager {
    stl::string_view              root_dir; // templates' root directory
    stl::vector<project_template> tmpls;    // project templates

    // scan the root directory for the list of available templates
    void scan();
    void add_template_file(stl::string_view file_path);

  private:
    static constexpr stl::string_view tmpl_extension = ".tmpl";
};

void tmeplate_manager::scan() {
    using fs = stl::filesystem;
    for (auto const entry : fs::recursive_directory_iterator(root_dir)) {
        const auto ext = entry.path().extension();
        if (entry.is_regular_file() && ext == tmpl_extension) {
            add_template_file(entry.path().string());
        }
    }
}

void template_manager::add_template_file(stl::string_view file) {}

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

    tmpl.scan(); // scan for files to find templates.
    tmpl.map_files_to_directory(project_dir);
}
