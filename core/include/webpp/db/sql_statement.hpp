#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "sql_concepts.hpp"

namespace webpp::sql {

    template <SQLStatement StmtType>
    struct sql_statement : StmtType {


        template <typename T>
        sql_statement& bind(int index, T&& val) noexcept {

            return *this;
        }

    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
