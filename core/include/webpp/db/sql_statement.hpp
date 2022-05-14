#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "sql_cell.hpp"
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


    /**
     * A wrapper for the driver's statement type. This is what the user uses directly.
     */
    template <SQLStatement StmtType>
    struct sql_statement : StmtType {
        using driver_stmt_type = StmtType;
        using size_type        = typename driver_stmt_type::size_type;
        using cell_type        = sql_cell<driver_stmt_type>;



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
        constexpr T cell() const {
            //
        }

        inline cell_type operator[](size_type index) const noexcept {
            //
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
