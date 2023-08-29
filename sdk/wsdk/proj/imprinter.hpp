// Created by moisrex on 8/29/23.

#ifndef WEBPP_IMPRINTER_HPP
#define WEBPP_IMPRINTER_HPP

#include "../progress_indicator.hpp"
#include "./proj_template.hpp"
#include "./project.hpp"

namespace webpp::sdk {


    struct proj_imprinter {
        enum imprint_status {
            success, // done
            unknown_failure
        };

        proj_imprinter(stl::shared_ptr<progress_bars>, stl::shared_ptr<command_options>);

        [[nodiscard]] stl::shared_ptr<project>       get_project() const noexcept;
        [[nodiscard]] stl::shared_ptr<proj_template> get_template() const noexcept;

        /// Set default project configurations
        void set_project(project&&);

        /// Set the template to be imprinted
        void set_template(proj_template&&);

        /// start creating a project
        void start();

      private:
        stl::shared_ptr<project>         proj;
        stl::shared_ptr<proj_template>   tmpl;
        stl::shared_ptr<progress_bars>   bars;
        stl::shared_ptr<command_options> options;
    };

} // namespace webpp::sdk

#endif // WEBPP_IMPRINTER_HPP
