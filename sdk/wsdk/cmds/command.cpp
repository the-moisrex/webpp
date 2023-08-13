#include "./command.hpp"

#include "./create_project.hpp"

#include <boost/program_options.hpp>
#include <functional>
#include <iostream>
#include <tuple>
#include <webpp/strings/join.hpp>


namespace webpp::sdk {

    using namespace boost::program_options;
    using namespace webpp::stl;



    struct command_description {
      public:
        int                 argc;
        char const* const*  argv;
        options_description root_desc{"Program options"};

      public:
        command_description(int input_argc, char const* const* input_argv)
          : argc{input_argc},
            argv{input_argv} {}

        int process_args() {

            root_desc.add_options() // start of options
              ("help,h",
               bool_switch()->default_value(false)->implicit_value(true),
               "print this help") // help
              ("cmd",
               value<string>()->default_value("help")->required(),
               "The command") // command
              ("cmd_opts", value<vector<string>>()->multitoken(), "The command options.");

            positional_options_description pos;
            pos.add("cmd", 1);
            pos.add("cmd_opts", -1);

            variables_map vm;
            store(command_line_parser(argc, argv).options(root_desc).positional(pos).run(), vm);
            notify(vm);

            if (vm.count("new")) {
                auto create_args = vm["cmd_opts"].template as<vector<string>>();
                create_args.erase(create_args.begin()); // remove "new"
                create_project creator{.command_desc = *this};
                return creator.handle(create_args);
            }

            if (vm.count("help")) {
                return print_help();
            }


            // running default action
            cout << "Please specify a command; here's the help:" << endl;
            print_help();
            return 1;
        }

        // this is also the default action
        int print_help() const {
            cout << root_desc << endl;
            return 0;
        }

        void session_manager() {
            // TODO: complete me
            // TODO: clean the sessions
            // TODO: clean session data for a specific user
        }
    };


    int command_manager::run_command(int argc, char const** argv) {
        command_description description{argc, argv};
        return description.process_args();
    }


} // namespace webpp::sdk
