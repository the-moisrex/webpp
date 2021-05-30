#ifndef WEBPP_SDK_CMDS_CMD
#define WEBPP_SDK_CMDS_CMD


namespace webpp::sdk::cmd {

	struct command {

		// the name of the command
		virtual constexpr stl::string_view name() noexcept const = 0;


		// description of the command
		virtual constexpr stl::string desc() noexcept const = 0;
	};

}


#endif // WEBPP_SDK_CMDS_CMD
