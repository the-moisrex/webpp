#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "../common/meta.hpp"
#include "../memory/object.hpp"
#include "../std/tuple.hpp"
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
        using string_type    = typename statement_type::string_type;

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


        template <typename T>
        [[nodiscard]] inline auto as() {
            auto                  errmsg       = object::make_general<string_type>(stmt);
            static constexpr auto t_size       = sizeof(T);
            static constexpr bool supports_int = requires {
                stmt.as_int(index, errmsg);
            };
            static constexpr bool supports_int64 = requires {
                stmt.as_int64(index, errmsg);
            };
            if constexpr (stl::is_arithmetic_v<T>) {
                if constexpr (stl::integral<T>) {
                    if constexpr (supports_int64 && t_size >= 64) {
                        return stmt.as_int64();
                    } else if constexpr (supports_int) {
                        return stmt.as_int();
                    } else {
                        static_assert_false(T, "The specified type is unknown to the database's driver.");
                    }
                } else if constexpr (stl::is_floating_point_v<T>) {
                    // todo
                } else {
                    static_assert_false(T,
                                        "How can a type be arithmetic but not integral nor floating point?");
                }
            } else if constexpr (istl::String<T>) {
                auto str = object::make_general<T>(stmt);
                stmt.as_string(index, str);
                return str;
            } else {
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
        using string_view_type  = traits::string_view<traits_type>;


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
                this->bind(index, istl::string_viewify_of<string_view_type>(stl::forward<T>(val)), errmsg);
            } else {
                static_assert_false(T, "Don't know how to bind the value, unknown type specified.");
            }
            if (!errmsg.empty()) {
                this->logger.error("SQLStmt", errmsg);
            }
            return *this;
        }

        inline bool step() noexcept {
            auto       errmsg          = object::make_general<string_type>(*this);
            const bool continue_or_not = this->step(errmsg);
            if (!errmsg.empty()) {
                this->logger.error("SQLStmt", errmsg);
                return false;
            }
            return continue_or_not;
        }

        inline binder_type column(size_type index) noexcept {
            return {*this, index};
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


        template <stl::size_t N>
        [[nodiscard]] inline auto structured() noexcept {
            return istl::ituple_iterable<sql_statement>{*this};
        }


        template <stl::size_t N>
        [[nodiscard]] inline auto structured() const noexcept {
            return istl::ituple_iterable<sql_statement>{*this};
        }


        template <stl::size_t N>
        [[nodiscard]] inline auto&& structured() const&& noexcept {
            return istl::ituple_iterable<sql_statement>{stl::move(*this)};
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
