#ifndef URI_H
#define URI_H

#include <string>
#include <vector>

namespace webpp {

    class path {
      private:
        std::vector<std::string> slugs;

      public:
        path(std::string const& str);
    };

} // namespace webpp

#endif // URI_H
