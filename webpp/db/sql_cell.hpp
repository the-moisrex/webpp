#ifndef WEBPP_DATABASE_SQL_CELL_HPP
#define WEBPP_DATABASE_SQL_CELL_HPP

#include "../common/meta.hpp"
#include "../convert/casts.hpp"
#include "../convert/lexical_cast.hpp"
#include "../std/string.hpp"
#include "sql_column.hpp"
#include "sql_concepts.hpp"

#include <compare>

namespace webpp::sql {

    template <typename SQLStmtType>
    struct sql_cell {
        using statement_type    = SQLStmtType;
        using size_type         = typename statement_type::size_type;
        using local_string_type = typename statement_type::local_string_type;
        using char_type         = typename statement_type::char_type;
        using string_type       = typename statement_type::string_type;

        static constexpr auto CELL_CAT = "SqlCell";

      private:
        statement_type* stmt;
        size_type       index;

      public:
        explicit sql_cell(statement_type& stmt_ref, size_type cell_index = 0) noexcept
          : stmt(stl::addressof(stmt_ref)),
            index(cell_index) {}

        // define operators
        // todo: replace this with spaceship op, it's not implemented as of this time.
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(op)                                                                                       \
    template <typename T>                                                                                   \
    inline bool operator op(T&& val) const {                                                                \
        switch (category()) {                                                                               \
            case column_category::string: {                                                                 \
                auto const str     = as_string<local_string_type>();                                        \
                auto const val_str = lexical::cast<local_string_type>(stl::forward<T>(val),                 \
                                                                      general_allocator<char_type>(*stmt)); \
                                                                                                            \
                return str op val_str;                                                                      \
            }                                                                                               \
            case column_category::number: {                                                                 \
                if constexpr (stl::integral<T>) {                                                           \
                    return val op as_number<T>();                                                           \
                } else {                                                                                    \
                    /* todo: add debug info                                                                 \
                     * todo: should we use long double?                                                     \
                     */                                                                                     \
                    return lexical::cast<double>(stl::forward<T>(val)) op as_number<double>();              \
                }                                                                                           \
            }                                                                                               \
            case column_category::blob:                                                                     \
            case column_category::unknown:                                                                  \
            default: return false;                                                                          \
        }                                                                                                   \
    }

        webpp_def(>)
        webpp_def(<)
        webpp_def(==)
        webpp_def(!=)
        webpp_def(>=)
        webpp_def(<=)
#undef webpp_def

        // NOLINTEND(*-macro-usage)

        template <istl::String StrT = stl::string>
        [[nodiscard]] inline StrT as_string() const {
            auto str = object::make_general<StrT>(*stmt);
            stmt->as_string(index, str);
            return str;
        }

        template <istl::arithmetic T = size_type>
        [[nodiscard]] inline T as_number() const {
            static constexpr auto t_size = sizeof(T);
            // static constexpr bool supports_int   = requires { stmt->as_int(index); };
            // static constexpr bool supports_int64 = requires { stmt->as_int64(index); };
            T res;

            if constexpr (stl::is_floating_point_v<T>) {
                // todo: add long double support
                if constexpr (t_size >= sizeof(double) && requires { stmt->as_double(); }) {
                    res = stmt->as_double(index);
                } else if constexpr (t_size == sizeof(float) && requires { stmt->as_float(); }) {
                    res = stmt->as_float(index);
                } else if constexpr (t_size >= sizeof(double)) {
                    res = static_cast<T>(stmt->as_double(index));
                } else if constexpr (t_size <= sizeof(float)) {
                    res = static_cast<T>(stmt->as_float(index));
                } else {
                    static_assert_false(T, "statement type doesn't support specified floating type.");
                }
            } else if constexpr (stl::integral<T>) {
                // todo: add 128 bit support
                // todo: add bool support
                // todo: add short support
                // todo: add unsigned support
                if constexpr (t_size >= sizeof(stl::int64_t) && requires { stmt->as_int64(); }) {
                    res = stmt->as_int64(index);
                } else if constexpr (t_size == sizeof(int) && requires { stmt->as_int(); }) {
                    res = stmt->as_int(index);
                } else if constexpr (t_size >= sizeof(stl::int64_t)) {
                    res = static_cast<T>(stmt->as_int64(index));
                } else if constexpr (t_size <= sizeof(int)) {
                    res = static_cast<T>(stmt->as_int(index));
                } else {
                    static_assert_false(T, "statement type doesn't support specified integral type.");
                }
            } else {
                static_assert_false(T, "statement type doesn't support specified type.");
            }
            return res;
        }

        // get the category type
        [[nodiscard]] inline column_category category() const noexcept {
            if constexpr (requires {
                              {
                                  stmt->column_cat(index)
                              } -> stl::same_as<column_category>;
                          })
            {
                return stmt->column_cat(index);
            } else {
                return column_category::unknown;
            }
        }

        // check if the column type is of type string (or varchar, char, text, or any other similar names)
        [[nodiscard]] inline bool is_string() const noexcept {
            if constexpr (requires {
                              {
                                  stmt->is_string()
                              } -> stl::same_as<bool>;
                          })
            {
                return stmt->is_string();
            } else {
                return category() == column_category::string;
            }
        }

        // check if the column type is of type number (or float, tinyint, integer, double, or any other
        // similar names)
        [[nodiscard]] inline bool is_number() const noexcept {
            if constexpr (requires {
                              {
                                  stmt->is_number()
                              } -> stl::same_as<bool>;
                          })
            {
                return stmt->is_number();
            } else if constexpr (requires {
                                     {
                                         stmt->is_column_integer(index)
                                     } -> stl::same_as<bool>;
                                     {
                                         stmt->is_column_float(index)
                                     } -> stl::same_as<bool>;
                                 })
            {
                // todo: do we need this implemwntation or should we re-order it and use category way first and use this way as fallback?
                return stmt->is_column_integer(index) || stmt->is_column_float(index);
            } else {
                return category() == column_category::number;
            }
        }

        [[nodiscard]] inline bool is_primary_key() const noexcept {
            return stmt->is_primary_key();
        }

        // check if the value is null
        [[nodiscard]] inline bool is_null() const noexcept {
            return stmt->is_column_null(index);
        }

        // support for stmt[index] = value;
        // calls stmt->bind
        template <typename T>
        constexpr sql_cell& operator=(T&& value) {
            stmt->bind(index, stl::forward<T>(value));
            return *this;
        }

        template <typename T>
        [[nodiscard]] inline sql_cell& operator<<(T&& val) noexcept {
            stmt->bind(index++, stl::forward<T>(val));
            return *this;
        }

        template <typename T>
        [[nodiscard]] inline auto as() {
            auto errmsg = object::make_general<string_type>(*stmt);
            if constexpr (stl::is_arithmetic_v<T>) {
                return as_number<T>();
            } else if constexpr (istl::String<T>) {
                return as_string<T>();
            } else {
                static_assert_false(T, "Cannot handle this data type");
                // todo
            }
        }

        template <typename T>
        [[nodiscard]] explicit(!istl::String<T> && !stl::is_arithmetic_v<T>) inline operator T() {
            return as<T>();
        }

        [[nodiscard]] inline bool is_integer() const noexcept {
            // todo: use category and other stuff here too
            return stmt->is_column_integer(index);
        }

        [[nodiscard]] inline bool is_float() const noexcept {
            // todo: use category and other stuff here too
            return stmt->is_column_float(index);
        }

        sql_cell& next_column(size_type val = 1) noexcept {
            index += val;
            return *this;
        }

        sql_cell& prev_column(size_type val = 1) noexcept {
            index -= val;
            return *this;
        }
    };

    template <typename StmtType>
    struct cell_iterator {
        using statement_type        = StmtType;
        using value_type            = typename statement_type::cell_type;
        using difference_type       = typename value_type::size_type;
        using reference             = stl::add_lvalue_reference_t<value_type>;
        using pointer               = typename stl::iterator_traits<value_type>::pointer;
        using iterator_category     = stl::bidirectional_iterator_tag;
        using iterator_concept      = stl::bidirectional_iterator_tag;
        using statement_driver_type = typename statement_type::driver_type;
        using cell_type             = sql_cell<statement_type>;
        // todo: convert cell iterator into a random access iterator

      private:
        cell_type cell; // cell is not a pointer or ref

      public:
        constexpr cell_iterator() noexcept = default;

        explicit constexpr cell_iterator(cell_type initial_cell) noexcept : cell{initial_cell} {}

        constexpr cell_iterator(cell_iterator const&)                = default;
        constexpr cell_iterator(cell_iterator&&) noexcept            = default;
        constexpr cell_iterator& operator=(cell_iterator const&)     = default;
        constexpr cell_iterator& operator=(cell_iterator&&) noexcept = default;

        constexpr ~cell_iterator() noexcept = default;

        constexpr auto operator==(cell_iterator const& rhs) noexcept {
            return cell == rhs.cell;
        }

        constexpr auto operator<=>(cell_iterator const& rhs) noexcept {
            return cell <=> rhs.cell;
        }

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return cell;
        }

        constexpr pointer operator->() const noexcept {
            return &cell;
        }

        constexpr cell_iterator& operator+=(difference_type val) noexcept {
            cell.next_column(val);
            return *this;
        }

        constexpr cell_iterator& operator-=(difference_type val) noexcept {
            cell.prev_column(val);
            return *this;
        }

        constexpr cell_iterator& operator++() noexcept {
            cell.next_column();
            return *this;
        }

        constexpr cell_iterator& operator--() noexcept {
            cell.prev_column();
            return *this;
        }

        constexpr cell_iterator operator++(int) const noexcept {
            return cell_iterator{*this}.operator++();
        }

        constexpr cell_iterator operator--(int) const noexcept {
            return cell_iterator{*this}.operator--();
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_CELL_HPP
