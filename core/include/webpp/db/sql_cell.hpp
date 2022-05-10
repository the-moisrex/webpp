#ifndef WEBPP_DATABASE_SQL_CELL_HPP
#define WEBPP_DATABASE_SQL_CELL_HPP

#include "../std/string.hpp"
#include "sql_concepts.hpp"

namespace webpp::sql {

    template <SQLStatement SQLStmtType>
    struct sql_cell {
        using statement_type = SQLStmtType;

      private:
        statement_type* stmt;

      public:
        template <istl::String StrT = stl::string>
        constexpr auto as_string() const {
            auto str = object::make_general<StrT>(*this);
            stmt.as_string(str);
            return str;
        }


        template <istl::String T>
        constexpr operator T() const {
            return as_string<T>();
        }



        template <stl::integral T>
        constexpr operator T() const {
            return static_cast<T>(as_number());
        }
    };
} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_CELL_HPP
