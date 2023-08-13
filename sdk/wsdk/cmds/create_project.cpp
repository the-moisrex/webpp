#include "./create_project.hpp"

#include <boost/program_options.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <webpp/logs/default_logger.hpp>


namespace webpp::sdk {

    using namespace boost::program_options;
    using namespace webpp::stl;

    // One template
    struct project_template {
        stl::string_view name; // template name

        static project_template create_from_file(stl::ifstream file);

        // check if the template is valid
        bool is_valid() const;
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





    int create_project::handle(vector<string> args) {

        options_description create_desc{"Create a new thing"};
        create_desc.add_options() // create options
          ("help,h",
           bool_switch()->default_value(false)->implicit_value(true),
           "print help for create command.") // help
          ("subcommand",
           value<string>()->default_value("help")->required(),
           "The subcommand of 'new' command to run") // sub-command
          ;

        positional_options_description pos;
        pos.add("subcommand", -1);
        pos.add("cmd_opts", -1);

        variables_map vm;
        store(command_line_parser(args).options(create_desc).positional(pos).run(), vm);
        notify(vm);

        if (!vm.count("subcommand"))
            return 1;

        const auto sub_command = vm["subcommand"].as<string>();

        if (sub_command == "help" || vm.count("help")) {
            cout << create_desc << endl;
            return 0;
        }

        if (sub_command == "project") {
            // handle project
            args.erase(args.begin()); // remove the first element because it's "project"
            return handle_project(args);
        }

        cout << create_desc << endl;
        return 0;
    }

    int create_project::handle_project(stl::vector<std::string> args) {
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



} // namespace webpp::sdk
