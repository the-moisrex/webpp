#ifndef WEBPP_HEADERS_HPP
#define WEBPP_HEADERS_HPP

#include "../std/string_view.h"
#include "cookies.h"
#include <map>
#include <string>

namespace webpp {

    /**
     * The classes header and body are the "owners of data".
     * That means these classes will own (use std::string/... and not references
     * and pointers) the data they have.
     *
     * And they also use other "views of data" classes like cookie classes so
     * they can provide shaped and meaningful data to the developers.
     *
     * This class will only contain what's the same in both request and response
     * classes; the rest, is up to those classes.
     */
    template <typename Interface>
    class headers {
      private:
        std::shared_ptr<Interface> interface;
        mutable std::multimap<std::string, std::string> data;
        mutable webpp::cookie_jar _cookies;

        /**
         * @brief this method will reload the cookies's cache.
         */
        void reload_cookies() const noexcept {
            // TODO
        }

      public:
        headers(std::shared_ptr<Interface> _interface)
            : interface(_interface) {}

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
            for (auto it = data.begin(); it != data.end();) {
                if (it->first == "set-cookie" || it->first == "cookie")
                    it = data.erase(it);
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
