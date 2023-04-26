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
        statement_type* stmt;

      public:
        sql_row(statement_type& stmt_ref) noexcept : stmt{*stmt_ref} {}

        bool operator==(sql_row const& rhs) const noexcept {
            return *stmt == *rhs.stmt;
        }

        auto operator<=>(sql_row const& rhs) const noexcept {
            return *stmt <=> *rhs.stmt;
        }


        [[nodiscard]] inline cell_type operator[](size_type index) const noexcept {
            return {*stmt, index};
        }


        [[nodiscard]] inline cell_iterator_type begin() noexcept {
            return {{*stmt, 0}};
        }


        [[nodiscard]] inline cell_iterator_type end() noexcept {
            return {{*stmt, stmt->column_count()}};
        }

        [[nodiscard]] inline cell_iterator_type begin() const noexcept {
            return {{*stmt, 0}};
        }


        [[nodiscard]] inline cell_iterator_type end() const noexcept {
            return {{*stmt, stmt->column_count()}};
        }

        [[nodiscard]] inline size_type size() const noexcept {
            return stmt->column_count();
        }

        statement_type& statement() noexcept {
            return *stmt;
        }

        // return a tuple of N length containing cells
        template <stl::size_t N, template <typename...> typename TupleType = stl::tuple>
        [[nodiscard]] inline auto as_tuple() const noexcept {
            using tuple = istl::repeat_type_t<N, cell_type, TupleType>;
            return ([this]<stl::size_t... I>(stl::index_sequence<I...>) constexpr noexcept {
                return tuple{cell_type{*stmt, I}...};
            })(stl::make_index_sequence<N>{});
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
          stl::add_lvalue_reference_t<value_type>;                // ref type without enforcing the constness
        using raw_pointer       = stl::add_pointer_t<value_type>; // pointer type without the constness
        using const_reference   = stl::add_const_t<raw_reference>;
        using const_pointer     = stl::add_const_t<raw_pointer>;
        using reference         = stl::conditional_t<is_const, const_reference, raw_reference>;
        using pointer           = stl::conditional_t<is_const, const_pointer, raw_pointer>;
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
        constexpr ~row_iterator()                                  = default;

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
        constexpr const_reference operator*() const noexcept {
            return *row;
        }

        constexpr reference operator*() noexcept {
            return *row;
        }

        constexpr const_pointer operator->() const noexcept {
            return &*row;
        }

        constexpr pointer operator->() noexcept {
            return &*row;
        }

        constexpr row_iterator& operator++() noexcept {
            assert(row);
            const bool has_next = row->statement().step();
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

namespace std {

    template <size_t I, class... T>
    struct tuple_element<I, webpp::sql::sql_row<T...>> {
        using type = typename webpp::sql::sql_row<T...>::cell_type;
    };

    template <class... T>
    struct tuple_size<webpp::sql::sql_row<T...>> : integral_constant<size_t, 0> {};

} // namespace std

#endif // WEBPP_DATABASE_SQL_ROW_HPP
