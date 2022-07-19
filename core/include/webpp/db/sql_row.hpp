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
        using statement_type           = stl::remove_cvref_t<StmtType>;
        using value_type               = sql_row<statement_type>;
        static constexpr bool is_const = stl::is_same_v<statement_type, StmtType>;
        using size_type                = typename statement_type::size_type;
        using difference_type          = size_type;
        using raw_reference =
          stl::add_lvalue_reference_t<value_type>;          // ref type without enforcing the constness
        using raw_pointer = stl::add_pointer_t<value_type>; // pointer type without the constness
        using reference   = stl::conditional_t<is_const, stl::add_const_t<raw_reference>, raw_reference>;
        using pointer     = stl::conditional_t<is_const, stl::add_const_t<raw_pointer>, raw_pointer>;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

      private:
        stl::optional<value_type> row{};

      public:
        constexpr row_iterator() noexcept = default;
        constexpr row_iterator(statement_type* stmt_ptr) noexcept : row{*stmt_ptr} {}
        constexpr row_iterator(row_iterator const&)                = default;
        constexpr row_iterator(row_iterator&&) noexcept            = default;
        constexpr row_iterator& operator=(row_iterator const&)     = default;
        constexpr row_iterator& operator=(row_iterator&&) noexcept = default;

        constexpr bool operator==(const row_iterator& rhs) const noexcept {
            return row == rhs.row;
        }

        constexpr bool operator!=(const row_iterator& rhs) const noexcept {
            return row != rhs.row;
        }

        constexpr auto operator<=>(const row_iterator& rhs) const noexcept {
            return row <=> rhs.row;
        }

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return *row;
        }

        constexpr pointer operator->() const noexcept {
            return &*row;
        }

        constexpr row_iterator& operator++() noexcept {
            assert(row);
            const bool has_next = row->step();
            if (!has_next) {
                row.reset();
            }
            return *this;
        }

        constexpr row_iterator operator++(int) const noexcept {
            return row_iterator{*row}.operator++();
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_ROW_HPP
