// Created by moisrex on 11/5/20.

#ifndef WEBPP_PORT_STRING_HPP
#define WEBPP_PORT_STRING_HPP

namespace webpp::uri {

    struct port_string {



        /**
         * Get the default port for the specified scheme
         * TODO: make this function a free function
         */
        [[nodiscard]] uint16_t default_port() const noexcept {
            auto _scheme = scheme();
            if (_scheme == "http")
                return 80u;
            if (_scheme == "https")
                return 443u;
            if (_scheme == "ftp")
                return 21u;
            if (_scheme == "ssh")
                return 22u;
            if (_scheme == "telnet")
                return 23u;
            if (_scheme == "ftps")
                return 990u;

            // TODO: add more protocols here
            return 0u;
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        [[nodiscard]] uint16_t port_uint16() const noexcept {
            if (has_port()) {
                return static_cast<uint16_t>(to_uint(port()));
            }
            return default_port();
        }

        /**
         * Get the port in a string_view format; there's no default value.
         * @return string_view
         */
        [[nodiscard]] str_view_t port() const noexcept {
            parse_port();

            // there's no port
            if (port_start == data.size())
                return {};

            // don't worry authority_end will be the end of the string anyway
            return substr(port_start + 1, (authority_end == data.size() ? authority_end - 1 : authority_end) -
                                          (port_start + 1));
        }

        /**
         * @brief checks if the uri has explicitly specified a port number or
         * not
         * @return bool
         */
        [[nodiscard]] bool has_port() const noexcept {
            parse_port();
            return port_start != data.size();
        }

        /**
         * @brief set port
         * @param new_port
         */
        auto& port(uint16_t new_port) noexcept {
            return port(to_str_copy<traits_type>(new_port));
        }

        /**
         * Set new port value
         * @param new_port
         */
        uri_string& port(str_view_t new_port) noexcept {
            if (ascii::starts_with(new_port, ':'))
                new_port.remove_prefix(1);
            if (!ascii::is::digit(new_port))
                throw stl::invalid_argument("The specified port is not valid");
            parse_port();
            if (port_start == data.size()) {
                // there's no port, I have to insert it myself:
                parse_host();

                if (authority_end != data.size()) {
                    // found it at the end of the line
                    replace_value(authority_end, 0, ":" + str_t(new_port));
                } else if (user_info_end != data.size()) {
                    // there's authority and there might be a host
                    replace_value(user_info_end + 1, user_info_end + 1, ":" + str_t(new_port));
                } else if (authority_start != data.size()) {
                    // there's a authority_start at least
                    replace_value(authority_start + 1, 0, ":" + str_t(new_port));
                } else {
                    // there's no authority at all.
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, "//:" + str_t(new_port));
                    } else {
                        // there's scheme
                        replace_value(scheme_end + 1, 0, "//:" + str_t(new_port));
                    }
                }
            } else {
                // there's a port and we are going to just replace it
                replace_value(port_start + 1,
                              authority_end == data.size() ? authority_end - 1 : authority_end, new_port);
            }
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri_string& clear_port() noexcept {
            return port({});
        }

    };

}

#endif // WEBPP_PORT_STRING_HPP
