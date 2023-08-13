#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD

#include <webpp/std/string.hpp>
#include <webpp/std/string_view.hpp>
#include <webpp/std/vector.hpp>


namespace webpp::sdk {


    struct command {

        command()                                   = default;
        command(command const&)                     = delete;
        command(command&&) noexcept                 = default;
        command& operator=(command const&) noexcept = delete;
        command& operator=(command&&) noexcept      = default;

        /// Get the command's name
        [[nodiscard]] virtual stl::string_view name() const noexcept = 0;

        /// Get the command's description
        [[nodiscard]] virtual stl::string_view desc() const noexcept = 0;

        /// Add another command as a child of this command
        template <typename CommandType, typename... Args>
            requires stl::derived_from<CommandType, command>
        void add_child(Args&&... args) {
            children.emplace_back(stl::make_unique<CommandType>(stl::forward<Args>(args)...));
        }

      private:
        stl::vector<stl::unique_ptr<command>> children;
    };

    /**
     * @brief Add the list of the commands
     */
    struct command_manager {

        // parse the args, and run the command
        int run_command(int argc, char const** argv);
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CMD
