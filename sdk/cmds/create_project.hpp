#ifndef WEBPP_SDK_CMDS_CREATE_PROJECT
#define WEBPP_SDK_CMDS_CREATE_PROJECT

#include "create.hpp"


namespace webpp::sdk::cmd {



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
        stl::string_view name() noexcept const override;
        stl::string_view desc() noexcept const override;
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

} // namespace webpp::sdk::cmd


#endif // WEBPP_SDK_CMDS_CREATE_PROJECT
