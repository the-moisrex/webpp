#ifndef HEADERS_HPP
#define HEADERS_HPP

#include "../std/string_view.h"
#include "cookies.h"
#include <map>
#include <string>

namespace webpp {

    template <typename Interface>
    class headers {
      private:
        std::multimap<std::string_view, std::string_view> data;
        webpp::cookies _cookies;

      public:
        auto& cookies() noexcept { return _cookies; }
        auto const& cookies() const noexcept { return _cookies; }

        void remove_cookies() noexcept {
            _cookies.clear();
            for (auto it = data.begin(); it != data.end();) {
                if (it->first == "set-cookie" || it->first == "cookie")
                    it = data.erase(it);
                else
                    ++it;
            }
        }

        void replace_cookies(webpp::cookies __cookies) noexcept {
            remove_cookies();
            _cookies = std::move(__cookies);
        }

        int status_code() const noexcept {}
    };
} // namespace webpp

#endif // HEADERS_HPP
