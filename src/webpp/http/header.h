#ifndef WEBPP_HEADERS_HPP
#define WEBPP_HEADERS_HPP

#include "../std/string_view.h"
#include "cookies.h"
#include <map>
#include <string>

namespace webpp {

    template <typename Interface>
    class header {
      private:
        int _status_code = 200;
        std::shared_ptr<Interface> interface;
        mutable std::multimap<std::string_view, std::string_view> data;
        mutable webpp::cookie_jar _cookies;

      public:
        header(std::shared_ptr<Interface> _interface) : interface(_interface) {}

        /**
         * @brief get cookies
         * @return
         */
        auto& cookies() noexcept { return _cookies; }

        /**
         * @brief get cookies
         * @return
         */
        auto const& cookies() const noexcept { return _cookies; }

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
    };
} // namespace webpp

#endif // WEBPP_HEADERS_HPP
