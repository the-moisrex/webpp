#include <boost/program_options.hpp>
#include <functional>
#include <iostream>
#include <tuple>
#include <wsdk/cmds/create_project.hpp>

using namespace webpp;

using action_type = std::function<void(boost::program_options::options_description const&,
                                       boost::program_options::variables_map const&)>;
using action_list = std::vector<std::pair<std::string, action_type>>;


void check_args(const int                argc,
                char const* const* const argv,
                action_list const&       actions,
                action_type const&       default_action) {

    using namespace boost::program_options;

    options_description desc("Program options");
    desc.add_options() // start of options
      ("update,u",
       bool_switch()->default_value(false)->implicit_value(true),
       "update the databases") // update
      ("help,h",
       bool_switch()->default_value(false)->implicit_value(true),
       "print this help") // help
      ("cmd",
       value<std::string>()->default_value("help")->required(),
       "The command") // command
      ("cmd_opts", value<std::vector<std::string>>()->multitoken(), "The command options.");

    positional_options_description pos;
    pos.add("cmd", 1);
    pos.add("cmd_opts", -1);

    variables_map vm;
    store(command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
    notify(vm);

    for (auto const& action : actions) {
        if (vm.count(action.first)) {
            action.second(desc, vm);
            return;
        }
    }

    // running default action
    default_action(desc, vm);
}

void print_help(boost::program_options::options_description const& desc,
                boost::program_options::variables_map const& /* vm */) {
    using namespace std;
    cout << desc << endl;
}

void create_template(boost::program_options::options_description const& desc,
                     boost::program_options::variables_map const& /* vm */) {

    // TODO: complete me
}

void update_db(boost::program_options::options_description const& desc,
               boost::program_options::variables_map const& /* vm */) {
    // TODO: complete me
}

void session_manager(boost::program_options::options_description const& desc,
                     boost::program_options::variables_map const&       vm) {
    // TODO: complete me
    // TODO: clean the sessions
    // TODO: clean session data for a specific user

    using namespace std;

    auto cmds = vm["cmd_opts"].as<vector<string>>();
    if (cmds.size() > 0) {
        auto cmd = cmds.at(0);

        // Clean the sessions
        if ("clean" == cmd || "clear" == cmd) {
            if (cmds.size() == 1) { // Clean all the data

            } else {
                // clean based on other values
            }
        }
    } else {
        cerr << "There's nothing to do." << endl;
    }
}

auto main(int argc, char const** argv) -> int {
    using namespace std;

    check_args(argc,
               argv,
               {{"help", print_help},
                {"create", create_template},
                {"session", session_manager},
                {"update", update_db}},
               print_help);

    return EXIT_SUCCESS;
}
