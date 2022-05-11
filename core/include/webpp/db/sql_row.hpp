#ifndef WEBPP_DATABASE_SQL_ROW_HPP
#define WEBPP_DATABASE_SQL_ROW_HPP

#include "sql_cell.hpp"

namespace webpp::sql {

    template <SQLStatement SQLStmtType>
    struct sql_row {
        using statement_type = SQLStmtType;
        using cell_type      = sql_cell<statement_type>;

      private:
      public:
        cell_type operator[]() {
            //
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_ROW_HPP
