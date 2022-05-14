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
        constexpr sql_statement_binder(sql_statement_type& stmt_ref, size_type column_index) noexcept
          : stmt{stmt_ref},
            index{column_index} {}

        // support for stmt[index] = value;
        template <typename T>
        constexpr sql_statement_binder& operator=(T&& value) {
            stmt.bind(index, stl::forward<T>(value));
            return *this;
        }
    };


    /**
     * A wrapper for the driver's statement type. This is what the user uses directly.
     */
    template <SQLStatement StmtType>
    struct sql_statement : StmtType {
        using driver_stmt_type = StmtType;
        using size_type        = typename driver_stmt_type::size_type;
        using binder_type      = sql_statement_binder<driver_stmt_type>;

      private:
      public:
        /**
         * Possible values that get passed down:
         *   - string_view
         *   - integral
         */
        template <typename T>
        sql_statement& bind(size_type index, T&& val) noexcept {
            if constexpr (requires { stmt->bind(index, stl::forward<T>(val)); }) {
                stmt->bind(index, stl::forward<T>(val));
            } else if constexpr (istl::StringViewifiable<T>) {
                //
            } else {
                static_assert_false(T, "Don't know how to bind the value, unknown type specified.");
            }
            return *this;
        }


        inline binder_type operator[](size_type index) noexcept {
            return {*this, index};
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
