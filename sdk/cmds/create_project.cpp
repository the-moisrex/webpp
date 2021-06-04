#include "create_project.hpp"

#include <filesystem>

using namespace webpp;
using namespace webpp::sdk::cmd;

// the struct that creates the project template's object
struct template_manager {
    stl::string_view root_dir; // templates' root directory

    // scan the root directory for the list of available templates
    void scan();

  private:
    static constexpr stl::size_t scan_limit = 10;
    static constexpr stl::string_view tmpl_extension = ".tmpl";
    void deep_scan(stl::size_t index = 0);
    void add_template_file(stl::string_view file_path);
};

// One template
struct project_template {
  private:
    stl::string_view name;     // template name

  public:
    project_template(stl::string_view inp_name) : name(inp_name) {}

};


void tmeplate_manager::deep_scan(stl::size_t index) {
  if (index > scan_limit) 
    return; // don't scan this deep

  for (auto const entry : fs::directory_iterator(root_dir)) {

    // todo: you can optimize this by making it multithreaded
    if (entry.is_directory()) {
      deep_scan(index+1);
    }

    if (entry.is_regular_file() && ext == tmpl_extension) {
      add_template_file(entry.path().string());
    }
  }
}

void template_manager::add_template_file(stl::string_view file) {
  
}

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
