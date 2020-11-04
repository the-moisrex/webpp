// Created by moisrex on 11/4/20.

#ifndef WEBPP_USER_INFO_HPP
#define WEBPP_USER_INFO_HPP

#include "../std/string.hpp"
#include "./encoding.hpp"
#include "./constants.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_user_info {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type = typename string_type::value_type;


        string_type username{};
        string_type password{};

        /**
         * Get the string in the encoded shape
         */
        void append_to(istl::String auto& out) const {
            if (!username.empty()) {
                encode_uri_component(username, out, USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
            if (!password.empty()) {
                out.reserve(password.size() + 1); // much better chance of removing one memory allocation
                out += '@';
                encode_uri_component(password, out, USER_INFO_NOT_PCT_ENCODED<char_type>);
            }
        }

        string_type to_string() const {
            string_type res{username.get_allocator()};
            append_to(res);
            return res;
        }

        void append_raw_to(istl::String &out) const {
            const bool user_empty = !username.empty();
            const bool pass_empty = !password.empty();
            if (user_empty && pass_empty) {
                out.resize(out.size() + username.size() + password.size() + 1);
                out += username;
                out += '@';
                out += password;
            } else if (pass_empty) {
                out += username;
            } else if (user_empty) {
                out.resize(out.size() + password.size() + 1);
                out += '@';
                out += password;
            }
        }

        /**
         * Convert to string without encoding it
         */
        string_type to_raw_string() const {
            string_type res{username.get_allocator()};
            append_raw_to(res);
            return res;
        }

    };


}

#endif // WEBPP_USER_INFO_HPP
