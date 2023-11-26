#ifndef WEBPP_SDK_CMDS_CREATE_PROJECT
#define WEBPP_SDK_CMDS_CREATE_PROJECT

#include "command.hpp"

#include <webpp/std/span.hpp>
#include <webpp/std/string_view.hpp>

namespace webpp::sdk {

    /**
     * Create a new project
     *
     * Syntax:
     *   wpp new project MySite
     *
     *   MySite is the project name
     */
    struct create_project : command {
        create_project()                                     = default;
        create_project(create_project&&) noexcept            = default;
        create_project(create_project const&)                = delete;
        create_project& operator=(create_project&&) noexcept = default;
        create_project& operator=(create_project const&)     = delete;
        ~create_project() override                           = default;

        [[nodiscard]] stl::string_view name() const noexcept override {
            return {"project"};
        }

        [[nodiscard]] stl::string_view desc() const noexcept override;

        // wpp new ....
        command_status start(command_options) override;

      private:
        // wpp new project ....
        int handle_project(stl::span<stl::string>);
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CREATE_PROJECT
