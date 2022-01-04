#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include <webpp/std/string.hpp>
#include <webpp/std/string_view.hpp>
#include <webpp/std/vector.hpp>


namespace webpp::sdk {

    struct command_description;


    /**
     * @brief Add the list of the commands
     */
    struct command_manager {

        // parse the args, and run the command
        int run_command(int argc, char const** argv);
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CMD
