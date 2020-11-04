// Created by moisrex on 11/4/20.

#ifndef WEBPP_USER_INFO_STRING_HPP
#define WEBPP_USER_INFO_STRING_HPP

namespace webpp::uri {


    struct user_info_string {

        /**
         * @brief checks if the uri has user info or not
         */
        [[nodiscard]] bool has_user_info() const noexcept {
            parse_user_info();
            return user_info_end != data.size() && authority_start != data.size();
        }

        /**
         * @brief get the user info or an empty value
         */
        [[nodiscard]] str_view_t user_info() const noexcept {
            parse_user_info();
            return (user_info_end == data.size() || authority_start == data.size())
                   ? str_view_t()
                   : substr(authority_start, user_info_end - authority_start);
        }

        /**
         * @brief decode user_info and return it as a string
         */
        [[nodiscard]] auto user_info_decoded() const noexcept {
            return decode_uri_component<traits_type>(user_info(), USER_INFO_NOT_PCT_ENCODED);
        }

        /**
         * @brief set the user info if it's possible
         */
        uri_string& user_info(str_view_t const& info) noexcept {
            parse_user_info();
            str_t encoded_info{this->get_allocator()};
            encode_uri_component(info, encoded_info, USER_INFO_NOT_PCT_ENCODED);
            if (user_info_end == data.size() || authority_start == data.size()) {
                // the URI already has user info, I just have to replace it
                replace_value(authority_start, user_info_end - authority_start, encoded_info);
            } else {
                // I don't know where is it, let's find it

                if (authority_start == data.size()) {
                    // this URI doesn't have authority in it, but I'm going to
                    // insert authority into it because the user is obviously
                    // demanding it; of course after this, the URI won't be a
                    // valid URI until the user sets at lease scheme too.

                    parse_scheme(); // to get "scheme_end"
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, str_t("//") + encoded_info + "@");
                    } else {
                        // there's scheme and we have to put it after that
                        replace_value(scheme_end + 1, 0, str_t("//") + encoded_info + "@");
                    }
                } else {
                    // there's authority start but there's no user_info_end
                    replace_value(authority_start, 0, encoded_info + "@");
                }
            }
            return *this;
        }

        /**
         * @brief clears the user info if exists
         */
        auto& clear_user_info() noexcept {
            return user_info({});
        }


        /**
         * Check if the user info has a username in it or not
         * @return bool
         */
        [[nodiscard]] bool has_username() const noexcept {
            return user_info_end != data.size();
        }

        /**
         * Get the username in the user info if it exists or otherwise an empty
         * string view
         * @return
         */
        [[nodiscard]] str_view_t username() const noexcept {
            auto _userinfo = user_info();
            if (auto colon = _userinfo.find(':'); colon != str_view_t::npos)
                _userinfo.remove_suffix(_userinfo.size() - colon);
            return _userinfo;
        }

        /**
         * Get the decoded version of the username if it exists
         * @return
         */
        [[nodiscard]] str_t username_decoded() const noexcept {
            return decode_uri_component<traits_type>(username(), USER_INFO_NOT_PCT_ENCODED);
        }

        /**
         * An indication of whether or not the user info has a password
         * @return
         */
        [[nodiscard]] bool has_password() const noexcept {
            return user_info().find(':') != str_view_t::npos;
        }

        /**
         * The password in the user info
         * @return
         */
        [[nodiscard]] str_view_t password() const noexcept {
            auto _user_info = user_info();
            if (auto found = _user_info.find(':'); found != str_view_t::npos) {
                return _user_info.substr(found + 1);
            }
            return {};
        }

        /**
         * The decoded version of the password
         * @return
         */
        [[nodiscard]] str_t password_decoded() const noexcept {
            return decode_uri_component<traits_type>(password(), USER_INFO_NOT_PCT_ENCODED);
        }

    };

}

#endif // WEBPP_USER_INFO_STRING_HPP
