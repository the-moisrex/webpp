#ifndef HEADERS_HPP
#define HEADERS_HPP

#include "../std/string_view.h"
#include "cookies.h"
#include <map>
#include <string>

namespace webpp {
    class headers {
      private:
        std::multimap<std::string_view, std::string_view> data;
        webpp::cookies _cookies;

      public:
        decltype(_cookies) const& cookies() const noexcept { return _cookies; }

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
