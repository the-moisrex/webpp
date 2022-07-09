#ifndef WEBPP_DATABASE_SQL_ROW_HPP
#define WEBPP_DATABASE_SQL_ROW_HPP

#include "sql_cell.hpp"

namespace webpp::sql {


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


        [[nodiscard]] inline cell_type operator[](size_type index) const noexcept {
            return {stmt, index};
        }


        [[nodiscard]] inline cell_iterator_type begin() noexcept {
            return {{stmt, 0}};
        }


        [[nodiscard]] inline cell_iterator_type end() noexcept {
            return {{stmt, stmt.column_count()}};
        }

        [[nodiscard]] inline cell_iterator_type begin() const noexcept {
            return {{stmt, 0}};
        }


        [[nodiscard]] inline cell_iterator_type end() const noexcept {
            return {{stmt, stmt.column_count()}};
        }

        [[nodiscard]] inline size_type size() const noexcept {
            return stmt.column_count();
        }
    };

    template <typename StmtType>
    struct row_iterator {
        using statement_type    = StmtType;
        using value_type        = statement_type;
        using size_type         = typename statement_type::size_type;
        using difference_type   = size_type;
        using reference         = stl::add_lvalue_reference_t<value_type>;
        using pointer           = stl::add_pointer_t<value_type>;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

      private:
        statement_type* stmt = nullptr;

      public:
        constexpr row_iterator() noexcept = default;
        constexpr row_iterator(statement_type* stmt_ptr) noexcept : stmt{stmt_ptr} {}
        constexpr row_iterator(row_iterator const&)                = default;
        constexpr row_iterator(row_iterator&&) noexcept            = default;
        constexpr row_iterator& operator=(row_iterator const&)     = default;
        constexpr row_iterator& operator=(row_iterator&&) noexcept = default;

        constexpr bool operator==(const row_iterator& rhs) const noexcept {
            return base() == rhs.base();
        }

        constexpr bool operator!=(const row_iterator& rhs) const noexcept {
            return base() != rhs.base();
        }

        constexpr auto operator<=>(const row_iterator& rhs) const noexcept {
            return base() <=> rhs.base();
        }

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return *stmt;
        }

        constexpr pointer operator->() const noexcept {
            return stmt;
        }

        constexpr row_iterator& operator++() noexcept {
            const bool has_next = stmt->step();
            if (!has_next) {
                stmt = nullptr;
            }
            return *this;
        }

        constexpr row_iterator operator++(int) const noexcept {
            return row_iterator{stmt}.operator++();
        }


        constexpr const pointer& base() const noexcept {
            return stmt;
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_ROW_HPP
