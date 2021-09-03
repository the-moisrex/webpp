#ifndef WEBPP_DATABASE_SQLITE_CONNECTION_HPP
#define WEBPP_DATABASE_SQLITE_CONNECTION_HPP

#include "../../traits/traits.hpp"

#include <utility>

namespace webpp::database {

    template <Traits TraitsType>
    struct sqlite_connection {
      private:
        sqlite3* handle{nullptr};


      public:
        bool open() {}

        bool close() {
            if (handle) {
                sqlite3_close(handle);
                return true;
            } else {
                return true;
            }
        }

        ~sqlite_connection() {
            close();
        }
    };

} // namespace webpp::database

#endif // WEBPP_DATABASE_SQLITE_CONNECTION_HPP
