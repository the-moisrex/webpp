#ifndef WEBPP_SDK_CMDS_CREATE
#define WEBPP_SDK_CMDS_CREATE

#include "command.hpp"


namespace webpp::sdk {

    /**
     * Create command
     *
     * Create a new thing
     */
    struct create : public command {
        stl::string_view name() const noexcept override;
        stl::string_view desc() const noexcept override;
        void             handle() override;

      protected:
        // A batch is a series of files and directories that have
        // a knowm name.
        virtual stl::string batch_name() const = 0;
    };

} // namespace webpp::sdk


#endif // WEBPP_SDK_CMDS_CREATE
