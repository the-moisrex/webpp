#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include "../../../core/include/webpp/std/string.hpp"
#include "../../../core/include/webpp/std/string_view.hpp"


namespace webpp::sdk::cmd {

    /**
     * Command abstraction
     *
     * This class is the abstract class and the helper class
     * that all the commands will be extend from.
     */
    struct command {

        // the name of the command
        virtual constexpr stl::string_view name() noexcept const = 0;


        // description of the command
        virtual constexpr stl::string desc() noexcept const = 0;
    };

} // namespace webpp::sdk::cmd


#endif // WEBPP_SDK_CMDS_CMD
