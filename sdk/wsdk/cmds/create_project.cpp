#include "./create_project.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <webpp/logs/default_logger.hpp>


using namespace webpp::sdk;
using namespace webpp;

// One template
struct project_template {
    stl::string_view name; // template name

    static project_template create_from_file(stl::ifstream file);

    // check if the template is valid
    bool is_valid() const;
};


// the struct that creates the project template's object
struct template_manager {

    template_manager(stl::string_view root_dir_inp) noexcept : root_dir{root_dir_inp} {}
    template_manager(template_manager const&)                = default;
    template_manager(template_manager&&) noexcept            = default;
    template_manager& operator=(template_manager const&)     = default;
    template_manager& operator=(template_manager&&) noexcept = default;
    ~template_manager()                                      = default;

    // scan the root directory for the list of available templates
    void scan();
    void add_template_file(stl::string_view file_path);

  private:
    stl::string_view              root_dir; // templates' root directory
    stl::vector<project_template> tmpls;    // project templates

    static constexpr stl::string_view tmpl_extension = ".tmpl";
};

void template_manager::scan() {
    namespace fs = std::filesystem;
    for (auto const& entry : fs::recursive_directory_iterator(root_dir)) {
        const auto ext = entry.path().extension();
        if (entry.is_regular_file() && ext == tmpl_extension) {
            add_template_file(entry.path().string());
        }
    }
}

void template_manager::add_template_file(stl::string_view /*file*/) {}





command_status create_project::start(command_options args) {
    using enum command_status;
    return success;
}

int create_project::handle_project(stl::span<std::string> args) {
    webpp::default_logger logger;

    if (args.empty()) {
        logger.error("Please specify a project name.");
        return 1;
    }

    stl::string proj_name{args.front()};

    if (proj_name.empty()) {
        logger.error("Please specify a valid name for the project.");
        return 1;
    }

    const project_template tmpl = get_project_template();
    if (!tmpl.is_valid()) {
        logger.error("The specified template is not valid.");
        return 1;
    }

    // check if the project's directory exists or not:

    // create the project directory:
    const stl::string_view project_dir;

    tmpl.scan(); // scan for files to find templates.
    tmpl.map_files_to_directory(project_dir);

    return 0;
}
stl::string_view create_project::desc() const noexcept {
    return {"Create Project"};
}
