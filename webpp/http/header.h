#ifndef WEBPP_HEADERS_HPP
#define WEBPP_HEADERS_HPP

#include "cookies.h"
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace webpp {

    /**
     * The classes header and body are the "owners of data".
     * That means these classes will own (use std::string/... and not references
     * and pointers) the data they have.
     *
     * This class will only be used in response and not in requests because it's
     * the owner of its data.
     */
    class headers : public std::multimap<std::string, std::string> {
      private:

        mutable webpp::cookie_jar _cookies;
        unsigned int _status_code = 200;

        /**
         * @brief this method will reload the cookies's cache.
         * This method is used internally in order to make sure the developer is
         * not paying anything to parse cookies while he/she is not using
         * cookies.
         */
        void reload_cookies() const noexcept {
            _cookies.clear();
            for (auto const& head : *this) {
                auto& attr = head.first;
                auto& value = head.second;
                if ("set-cookie" == attr) {
                    _cookies.emplace(value);
                }
            }
        }

      public:
        /**
         * @brief get status code
         */
        auto status_code() const noexcept { return _status_code; }

        /**
         * @brief set status code
         * @param status_code
         */
        void status_code(decltype(_status_code) __status_code) noexcept {
            _status_code = __status_code;
        }

        /**
         * @brief get cookies
         * @return
         */
        auto& cookies() noexcept {
            reload_cookies();
            return _cookies;
        }

        /**
         * @brief get cookies
         * @return
         */
        auto const& cookies() const noexcept {
            reload_cookies();
            return _cookies;
        }

        /**
         * @brief remove cookies in the cookie jar
         */
        void remove_cookies() noexcept {
            _cookies.clear();
            for (auto it = begin(); it != end();) {
                if (it->first == "set-cookie")
                    it = erase(it);
                else
                    ++it;
            }
        }

        /**
         * @brief replace cookies in the cookie_jar
         * @param cookie_jar
         */
        void replace_cookies(webpp::cookie_jar&& __cookies) noexcept {
            remove_cookies();
            _cookies = std::move(__cookies);
        }

        /**
         * @brief replace cookies in the cookie_jar
         * @param cookie_jar
         */
        void replace_cookies(webpp::cookie_jar const& __cookies) noexcept {
            remove_cookies();
            _cookies = __cookies;
        }

    };

} // namespace webpp

#endif // WEBPP_HEADERS_HPP
