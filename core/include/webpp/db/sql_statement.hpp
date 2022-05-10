#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "sql_concepts.hpp"

namespace webpp::sql {
    template <SQLStatement StmtType>
    struct sql_statement;

    template <SQLStatement StmtType>
    struct sql_statement_binder {
        using statement_type     = StmtType;
        using size_type          = typename statement_type::size_type;
        using sql_statement_type = sql_statement<statement_type>;

      private:
        sql_statement_type& stmt;
        size_type           index = 0; // index of the column

      public:
        constexpr sql_statement_binder(size_type column_index) noexcept : index{column_index} {}

        // support for stmt[index] = value;
        template <typename T>
        constexpr sql_statement_binder& operator=(T&& value) {
            stmt.bind(index, stl::forward<T>(value));
            return *this;
        }

        constexpr auto value() noexcept(noexcept(stmt->column(index))) {
            return stmt->column(index);
        }

        template <typename T>
        constexpr auto as() noexcept(noexcept(as<type>(value()))) {
            return as<type>(value());
        }

#define define_op(type)                                               \
    constexpr operator type() noexcept(noexcept(as<type>(value()))) { \
        return this->template as<type>();                             \
    }
        define_op(int)

#undef define_op
    };

    template <SQLStatement StmtType>
    struct sql_statement : StmtType {


        /**
         * Possible values that get passed down:
         *   - string_view
         *   - integral
         */
        template <typename T>
        sql_statement& bind(int index, T&& val) noexcept {

            return *this;
        }

        template <typename T>
        constexpr T column() const {
            //
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
