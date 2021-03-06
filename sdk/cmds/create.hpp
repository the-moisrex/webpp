#ifndef WEBPP_SDK_CMDS_CREATE
#define WEBPP_SDK_CMDS_CREATE

#include "command.hpp"


namespace webpp::sdk::cmd {

    /**
     * Create command
     *
     * Create a new thing
     */
    struct create : public command {
        stl::string_view name() noexcept const override;
        stl::string_view desc() noexcept const override;
        void             handle() override;

      protected:
        // A batch is a series of files and directories that have
        // a knowm name.
        virtual stl::string batch_name() const = 0;
    };

} // namespace webpp::sdk::cmd


#endif // WEBPP_SDK_CMDS_CREATE
