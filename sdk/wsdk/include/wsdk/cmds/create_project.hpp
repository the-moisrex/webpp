#ifndef WEBPP_SDK_CMDS_CREATE_PROJECT
#define WEBPP_SDK_CMDS_CREATE_PROJECT

#include "command.hpp"


namespace webpp::sdk {


    /**
     * Create a new project
     *
     * Syntax:
     *   wpp new project MySite
     *
     *   MySite is the project name
     */
    struct create_project {
        command_description& command_desc;

        stl::string_view desc() const noexcept;
        int              handle(stl::string);

      private:
        void handle_args();
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CREATE_PROJECT
