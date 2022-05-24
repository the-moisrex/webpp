#ifndef WEBPP_DATABASE_SQL_CELL_HPP
#define WEBPP_DATABASE_SQL_CELL_HPP

#include "../common/meta.hpp"
#include "../convert/casts.hpp"
#include "../convert/lexical_cast.hpp"
#include "../std/compare.hpp"
#include "../std/string.hpp"
#include "sql_column.hpp"
#include "sql_concepts.hpp"

namespace webpp::sql {

    template <SQLStatement SQLStmtType>
    struct sql_cell {
        using statement_type = SQLStmtType;
        using size_type      = typename statement_type::size_type;

      private:
        statement_type& stmt;
        size_type       index;

      public:
        sql_cell(statement_type& stmt_ref) noexcept : stmt(stmt_ref) {}

        template <typename T>
        inline stl::strong_ordering operator<=>(T&& val) const {
            switch (category()) {
                case column_category::string: {
                    auto str = object::make_local<StrT>(stmt);
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
            return false;
        }

        template <istl::String StrT = stl::string>
        [[nodiscard]] inline auto as_string() const {
            auto str = object::make_general<StrT>(stmt);
            stmt.as_string(str);
            return str;
        }


        template <typename T>
        operator T() const {
            if constexpr (istl::String<T>) {
                return as_string<T>();
            } else if constexpr (stl::integral<T>) {
                return static_cast<T>(stmt.as_number());
            } else {
                static_assert_false(T, "Cannot handle this data type");
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
    };
} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_CELL_HPP
