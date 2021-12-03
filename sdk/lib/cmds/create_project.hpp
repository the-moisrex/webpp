#ifndef WEBPP_SDK_CMDS_CREATE_PROJECT
#define WEBPP_SDK_CMDS_CREATE_PROJECT

#include "create.hpp"


namespace webpp::sdk {



    /**
     * The selected template of a project will be detailed in
     * this struct.
     */
    struct project_template;


    /**
     * Create a new project
     *
     * Syntax:
     *   webpp new project MySite
     */
    struct create_project : public create {
        stl::string_view name() const noexcept override;
        stl::string_view desc() const noexcept override;
        void             handle() override;

      protected:
        // A batch is a series of files and directories that have
        // a knowm name.
        virtual stl::string batch_name() const = 0;

        // Get the selected project name
        // Empty when it's not specified.
        stl::string project_name() const;

        // Get the selected template name
        project_template get_project_template() const;
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CREATE_PROJECT
