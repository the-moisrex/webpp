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

    template <SQLStatement SQLStmtType>
    struct sql_cell {
        using statement_type    = SQLStmtType;
        using size_type         = typename statement_type::size_type;
        using local_string_type = typename statement_type::local_string_type;
        using char_type         = typename statement_type::char_type;

      private:
        statement_type& stmt;
        size_type       index;

      public:
        sql_cell(statement_type& stmt_ref, size_type cell_index = 0) noexcept
          : stmt(stmt_ref),
            index(cell_index) {}

        template <typename T>
        inline stl::partial_ordering operator<=>(T&& val) const {
            switch (category()) {
                case column_category::string: {
                    auto str = object::make_local<T>(stmt);
                    stmt.as_string(str);
                    return str <=> lexical::cast<local_string_type>(
                                     val,
                                     stmt.alloc_pack.template local_allocator<char_type>());
                }
                case column_category::number: {
                    if constexpr (stl::integral<T>) {
                        return val <=> as_number<T>();
                    } else {
                        // todo: add debug info
                        // todo: should we use long double?
                        return to<double>(val) <=> as_number<double>();
                    }
                }
                case column_category::blob: {
                    // todo
                }
            }
            return stl::partial_ordering::unordered;
        }

        template <istl::String StrT = stl::string>
        [[nodiscard]] inline auto as_string() const {
            auto str = object::make_general<StrT>(stmt);
            stmt.as_string(str);
            return str;
        }

        template <istl::arithmetic T = size_type>
        [[nodiscard]] inline T as_number() const {
            static constexpr auto t_size       = sizeof(T);
            static constexpr bool supports_int = requires {
                stmt.as_int(index, errmsg);
            };
            static constexpr bool supports_int64 = requires {
                stmt.as_int64(index, errmsg);
            };

            if constexpr (stl::is_floating_point_v<T>) {
                // todo: add long double support
                if constexpr (t_size >= sizeof(double) && requires { stmt.as_double(); }) {
                    return stmt.as_double(index);
                } else if constexpr (t_size == sizeof(float) && requires { stmt.as_float(); }) {
                    return stmt.as_float(index);
                } else if constexpr (t_size >= sizeof(double)) {
                    return static_cast<T>(stmt.as_double(index));
                } else if constexpr (t_size <= sizeof(float)) {
                    return static_cast<T>(stmt.as_float(index));
                } else {
                    static_assert_false(T, "statement type doesn't support specified floating type.");
                }
            } else if constexpr (stl::integral<T>) {
                // todo: add 128 bit support
                // todo: add bool support
                // todo: add short support
                // todo: add unsigned support
                if constexpr (t_size >= sizeof(stl::int64_t) && requires { stmt.as_int64(); }) {
                    return stmt.as_int64(index);
                } else if constexpr (t_size == sizeof(int) && requires { stmt.as_int(); }) {
                    return stmt.as_int(index);
                } else if constexpr (t_size >= sizeof(stl::int64_t)) {
                    return static_cast<T>(stmt.as_int64(index));
                } else if constexpr (t_size <= sizeof(int)) {
                    return static_cast<T>(stmt.as_int(index));
                } else {
                    static_assert_false(T, "statement type doesn't support specified integral type.");
                }
            } else {
                static_assert_false(T, "statement type doesn't support specified type.");
            }
        }


        // get the category type
        [[nodiscard]] inline column_category category() const noexcept {
            if constexpr (requires {
                              { stmt.column_category(index) } -> stl::same_as<column_category>;
                          }) {
                return stmt.column_category(index);
            } else {
                return column_category::unknown;
            }
        }

        // check if the column type is of type string (or varchar, char, text, or any other similar names)
        [[nodiscard]] inline bool is_string() const noexcept {
            if constexpr (requires {
                              { stmt.is_string() } -> stl::same_as<bool>;
                          }) {
                return stmt.is_string();
            } else {
                return category() == column_category::string;
            }
        }



        // check if the column type is of type number (or float, tinyint, integer, double, or any other
        // similar names)
        [[nodiscard]] inline bool is_number() const noexcept {
            if constexpr (requires {
                              { stmt.is_number() } -> stl::same_as<bool>;
                          }) {
                return stmt.is_number();
            } else {
                return category() == column_category::number;
            }
        }


        [[nodiscard]] inline bool is_primary_key() const noexcept {
            return stmt.is_primary_key();
        }

        // check if the value is null
        [[nodiscard]] inline bool is_null() const noexcept {
            return stmt.is_column_null(index);
        }


        // support for stmt[index] = value;
        // calls stmt.bind
        template <typename T>
        constexpr sql_cell& operator=(T&& value) {
            stmt.bind(index, stl::forward<T>(value));
            return *this;
        }


        template <typename T>
        [[nodiscard]] inline sql_cell& operator<<(T&& val) noexcept {
            stmt.bind(index++, stl::forward<T>(val));
            return *this;
        }


        template <typename T>
        [[nodiscard]] inline auto as() {
            auto                  errmsg = object::make_general<string_type>(stmt);
            static constexpr auto t_size = sizeof(T);
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
        [[nodiscard]] inline operator T() {
            return as<T>();
        }

        [[nodiscard]] inline bool is_integer() const noexcept {
            return stmt.is_column_integer(index);
        }

        [[nodiscard]] inline bool is_float() const noexcept {
            return stmt.is_column_float(index);
        }

        [[nodiscard]] inline bool is_number() const noexcept {
            return stmt.is_column_integer(index) || stmt.is_column_float(index);
        }
    };



    template <SQLStatement SQLStmtType>
    struct cell_iterator {};

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_CELL_HPP
