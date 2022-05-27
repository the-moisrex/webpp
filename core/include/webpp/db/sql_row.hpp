#ifndef WEBPP_DATABASE_SQL_ROW_HPP
#define WEBPP_DATABASE_SQL_ROW_HPP

#include "sql_cell.hpp"

namespace webpp::sql {

    template <SQLStatement SQLStmtType>
    struct cell_iterator {};


    template <typename SQLStmtType>
    struct sql_row {
        using statement_type     = SQLStmtType;
        using cell_type          = sql_cell<statement_type>;
        using cell_iterator_type = cell_iterator<statement_type>;
        using size_type          = typename statement_type::size_type;

      private:
        statement_type& stmt;

      public:
        sql_row(statement_type& stmt_ref) noexcept : stmt{stmt_ref} {}


        cell_type operator[](size_type index) const noexcept {
            return {stmt, index};
        }


        [[nodiscard]] inline cell_iterator_type begin() const noexcept {
            return {};
        }


        [[nodiscard]] inline cell_iterator_type end() const noexcept {
            return {};
        }

        [[nodiscard]] inline size_type size() const noexcept {
            return stmt.column_count();
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_ROW_HPP
