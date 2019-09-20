#ifndef WEBPP_HEADERS_HPP
#define WEBPP_HEADERS_HPP

#include "../common/meta.h"
#include "../std/string_view.h"
#include "cookies.h"
#include <map>
#include <string>

namespace webpp {

    enum class header_type { RESPONSE, REQUEST };
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
    template <typename Interface,
              header_type HeaderType = header_type::RESPONSE>
    class headers {
      private:
        using headers_t = std::multimap<std::string, std::string>;

        std::shared_ptr<Interface> interface;
        mutable headers_t data;
        mutable webpp::cookie_jar _cookies;

        /**
         * @brief this method will reload the cookies's cache.
         * This method is used internally in order to make sure the developer is
         * not paying anything to parse cookies while he/she is not using
         * cookies.
         */
        void reload_cookies() const noexcept {

            _cookies.clear();

            if constexpr (HeaderType == header_type::RESPONSE) {
                for (auto const& head : data) {
                    auto attr = head.first;
                    auto value = head.second;
                    if ("set-cookie" == attr) {
                        _cookies.emplace(value);
                    }
                }

            } else {

                for (auto const& head : data) {
                    auto attr = head.first;
                    auto value = head.second;
                    if (attr == "cookie") {
                        _cookies.emplace(value);
                    }
                }
            }
        }

      public:
        // TODO: consider using custom iterators instead of multimap's

        using iterator = headers_t::iterator;
        using const_iterator = headers_t ::const_iterator;
        using reverse_iterator = headers_t::reverse_iterator;
        using const_reverse_iterator = headers_t::const_reverse_iterator;

        explicit headers(std::shared_ptr<Interface> _interface)
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

        iterator begin() noexcept { return data.begin(); }

        const_iterator begin() const noexcept { return data.begin(); }

        const_iterator cbegin() const noexcept { return data.cbegin(); }

        iterator end() noexcept { return data.end(); }

        const_iterator end() const noexcept { return data.end(); }

        const_iterator cend() const noexcept { return data.cend(); }

        reverse_iterator rbegin() noexcept { return data.rbegin(); }

        const_reverse_iterator rbegin() const noexcept { return data.rbegin(); }

        reverse_iterator rend() noexcept { return data.rend(); }

        const_reverse_iterator rend() const noexcept { return data.rend(); }

        const_reverse_iterator crbegin() const noexcept {
            return data.crbegin();
        }

        const_reverse_iterator crend() const noexcept { return data.crend(); }
    };

    template <typename Interface>
    class headers<Interface, header_type::RESPONSE> {
        void reload_cookies() const noexcept;
    };

    template <typename Interface>
    void headers<Interface, header_type::RESPONSE>::reload_cookies() const
        noexcept {}

} // namespace webpp

#endif // WEBPP_HEADERS_HPP
