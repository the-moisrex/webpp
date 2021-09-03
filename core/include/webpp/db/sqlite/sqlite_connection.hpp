#ifndef WEBPP_DATABASE_SQLITE_CONNECTION_HPP
#define WEBPP_DATABASE_SQLITE_CONNECTION_HPP

#include "../../traits/traits.hpp"

namespace webpp::database {

    template <Traits TraitsType>
    struct sqlite_connection {


        bool open() {}

        bool close() {}
    };

} // namespace webpp::database

#endif // WEBPP_DATABASE_SQLITE_CONNECTION_HPP
