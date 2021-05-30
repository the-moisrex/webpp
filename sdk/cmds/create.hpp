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

        // the name of the command
        virtual constexpr stl::string_view name() noexcept const {
		return "create";
	}


        // description of the command
        virtual constexpr stl::string desc() noexcept const = 0;
    };

} // namespace webpp::sdk::cmd


#endif // WEBPP_SDK_CMDS_CREATE
