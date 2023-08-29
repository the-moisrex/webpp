// Created by moisrex on 8/29/23.

#ifndef WEBPP_PROJ_TEMPLATE_HPP
#define WEBPP_PROJ_TEMPLATE_HPP

#include "../cmds/command_options.hpp"

namespace webpp::sdk {

    enum struct tmpl_action_status {
        success         = 0x0, // done
        unknown_failure = 0x1
    };

    struct proj_template {

        proj_template(stl::shared_ptr<command_options>);

        [[nodiscard]] bool operator==(proj_template) const noexcept;

      private:
        class impl;
        impl* tmpl;
    };

} // namespace webpp::sdk

#endif // WEBPP_PROJ_TEMPLATE_HPP
