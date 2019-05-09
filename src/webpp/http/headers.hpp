#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <string>

namespace webpp {
    class headers {
      private:
        std::map<std::string, std::string> data;

      public:
        void setHeader(std::string name, std::string value) noexcept;
        std::string getHeader(std::string const& name) const noexcept;
    };
} // namespace webpp

#endif // HEADERS_HPP
