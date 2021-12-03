#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include <webpp/std/string.hpp>
#include <webpp/std/string_view.hpp>


namespace webpp::sdk {

    /**
     * Command abstraction
     *
     * This class is the abstract class and the helper class
     * that all the commands will be extend from.
     */
    struct command {

        // the name of the command
        virtual stl::string_view name() const noexcept = 0;


        // description of the command
        virtual stl::string_view desc() const noexcept = 0;

        // Run the command
        virtual void handle();
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CMD
