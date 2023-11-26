#ifndef WEBPP_DATABASE_SQL_COLUMN_HPP
#define WEBPP_DATABASE_SQL_COLUMN_HPP

namespace webpp::sql {

    enum struct column_category {
        unknown,
        string, // could be any type of string
        number,
        blob
    };

} // namespace webpp::sql

#endif
