#ifndef HEADERS_HPP
#define HEADERS_HPP

#include "cookies.hpp"
#include <map>
#include <string>

namespace webpp {
    class headers : public std::multimap<std::string, std::string> {
      private:
        webpp::cookies _cookies;

      public:
        inline decltype(_cookies) const& cookies() const noexcept {
            return _cookies;
        }

        void remove_cookies() noexcept {
            _cookies.clear();
            for (auto it = begin(); it != end();) {
                if (it->first == "set-cookie" || it->first == "cookie")
                    it = erase(it);
                else
                    ++it;
            }
        }

        void replace_cookies(webpp::cookies __cookies) noexcept {
            remove_cookies();
            _cookies = std::move(__cookies);
        }
    };
} // namespace webpp

#endif // HEADERS_HPP
