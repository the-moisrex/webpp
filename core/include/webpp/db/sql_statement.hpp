#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "../common/meta.hpp"
#include "../memory/object.hpp"
#include "../traits/enable_traits.hpp"
#include "sql_concepts.hpp"
#include "sql_row.hpp"

namespace webpp::sql {

    /**
     * This class is the thing that gets returned by the statement's [] operator.
     */
    template <typename StmtType>
    struct sql_statement_binder {
        using statement_type = StmtType;
        using size_type      = typename statement_type::size_type;

      private:
        statement_type& stmt;
        size_type       index = 0; // index of the column

      public:
        constexpr sql_statement_binder(statement_type& stmt_ref, size_type column_index) noexcept
          : stmt{stmt_ref},
            index{column_index} {}

        // support for stmt[index] = value;
        template <typename T>
        constexpr sql_statement_binder& operator=(T&& value) {
            stmt.bind(index, stl::forward<T>(value));
            return *this;
        }


        template <typename T>
        [[nodiscard]] inline sql_statement_binder& operator<<(T&& val) noexcept {
            stmt.bind(index++, stl::forward<T>(val));
            return *this;
        }
    };


    /**
     * A wrapper for the driver's statement type. This is what the user uses directly.
     */
    template <Traits TraitsType, SQLStatement StmtType>
    struct sql_statement : StmtType, enable_traits<TraitsType> {
        using traits_type       = TraitsType;
        using driver_stmt_type  = StmtType;
        using size_type         = typename driver_stmt_type::size_type;
        using binder_type       = sql_statement_binder<sql_statement>;
        using string_type       = traits::general_string<traits_type>;
        using rows_type         = sql_row<sql_statement>;
        using local_string_type = traits::local_string<traits_type>;
        using char_type         = traits::char_type<traits_type>;


        /**
         * Possible values that get passed down:
         *   - string_view
         *   - integral
         */
        template <typename T>
        sql_statement& bind(size_type index, T&& val) noexcept {
            auto errmsg = object::make_general<string_type>(*this);
            if constexpr (requires { this->bind(index, stl::forward<T>(val), errmsg); }) {
                this->bind(index, stl::forward<T>(val), errmsg);
            } else if constexpr (istl::StringViewifiable<T>) {
                //
            } else {
                static_assert_false(T, "Don't know how to bind the value, unknown type specified.");
            }
            if (!errmsg.empty()) {
                this->logger.error("SQLStmt", errmsg);
            }
            return *this;
        }


        [[nodiscard]] inline binder_type operator[](size_type index) noexcept {
            return {*this, index};
        }

        template <typename T>
        [[nodiscard]] inline binder_type operator<<(T&& val) noexcept {
            bind(0, stl::forward<T>(val));
            return binder_type{*this, 1};
        }

        [[nodiscard]] inline rows_type rows() const noexcept {
            return {driver()};
        }

        [[nodiscard]] inline driver_stmt_type driver() noexcept {
            return *static_cast<driver_stmt_type*>(this);
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
