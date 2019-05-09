#include <boost/program_options.hpp>
#include <iostream>

auto main(int argc, char const** argv) -> int {

    using namespace boost::program_options;
    using namespace std;

    options_description desc("webpp cli");
    desc.add_options()(
        "help,h", bool_switch()->default_value(false)->implicit_value(true),
        "print this help");

    variables_map wm;
    store(command_line_parser(argc, argv).options(desc).run(), wm);
    notify(wm);

    if (wm.count("help")) {
        cout << desc << endl;
        return EXIT_SUCCESS;
    }

    // default action
    cout << desc << endl;

    return EXIT_SUCCESS;
}
