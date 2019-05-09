#include <boost/program_options.hpp>
#include <functional>
#include <iostream>
#include <tuple>

void check_args(
    const int argc, char const* const* const argv,
    std::vector<std::pair<
        std::string,
        std::function<
            void(boost::program_options::options_description const&)>>> const&
        actions,
    std::function<void(boost::program_options::options_description)> const&
        default_action) {
    using namespace boost::program_options;

    options_description desc("Program options");
    desc.add_options()(
        "help,h", bool_switch()->default_value(false)->implicit_value(true),
        "print this help");

    variables_map wm;
    store(command_line_parser(argc, argv).options(desc).run(), wm);
    notify(wm);

    for (auto const& action : actions) {
        if (wm.count(action.first)) {
            action.second(desc);
            return;
        }
    }

    // running default action
    default_action(desc);
}

void print_help(boost::program_options::options_description const& desc) {
    using namespace std;
    cout << desc << endl;
}

void create_template(boost::program_options::options_description const& desc) {}

auto main(int argc, char const** argv) -> int {
    using namespace std;

    check_args(argc, argv, {{"help", print_help}, {"create", create_template}},
               print_help);

    return EXIT_SUCCESS;
}
