#ifndef WEBPP_DATABASE_SQL_ROW_HPP
#define WEBPP_DATABASE_SQL_ROW_HPP

#include "sql_cell.hpp"

namespace webpp::sql {

    template <SQLStatement SQLStmtType>
    struct cell_iterator {};


    template <SQLStatement SQLStmtType>
    struct sql_row {
        using statement_type     = SQLStmtType;
        using cell_type          = sql_cell<statement_type>;
        using cell_iterator_type = cell_iterator<statement_type>;

      private:
        statement_type stmt;

      public:
        cell_type operator[]() {
            //
        }


        [[nodiscard]] inline cell_iterator_type begin() const noexcept {
            return {};
        }


        [[nodiscard]] inline cell_iterator_type end() const noexcept {
            return {};
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_ROW_HPP
