#ifndef WEBPP_DATABASE_SQL_STATEMENT_HPP
#define WEBPP_DATABASE_SQL_STATEMENT_HPP

#include "../common/meta.hpp"
#include "../memory/object.hpp"
#include "../std/tuple.hpp"
#include "../traits/enable_traits.hpp"
#include "sql_cell.hpp"
#include "sql_concepts.hpp"
#include "sql_row.hpp"

namespace webpp::sql {


    /**
     * A wrapper for the driver's statement type. This is what the user uses directly.
     */
    template <Traits TraitsType, SQLStatement StmtType>
    struct sql_statement : StmtType, enable_traits<TraitsType> {
        using traits_type       = TraitsType;
        using driver_type       = StmtType;
        using size_type         = typename driver_type::size_type;
        using string_type       = traits::general_string<traits_type>;
        using rows_type         = sql_row<sql_statement>;
        using cell_type         = sql_cell<driver_type>;
        using local_string_type = traits::local_string<traits_type>;
        using char_type         = traits::char_type<traits_type>;
        using string_view_type  = traits::string_view<traits_type>;
        using iterator          = row_iterator<sql_statement>;


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

        inline cell_type column(size_type index) noexcept {
            return {*this, index};
        }


        [[nodiscard]] inline cell_type operator[](size_type index) noexcept {
            return {*this, index};
        }

        template <typename T>
        [[nodiscard]] inline cell_type operator<<(T&& val) noexcept {
            bind(0, stl::forward<T>(val));
            return cell_type{*this, 1};
        }

        [[nodiscard]] inline rows_type rows() const noexcept {
            return {driver()};
        }

        [[nodiscard]] inline driver_type driver() noexcept {
            return *static_cast<driver_type*>(this);
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


        iterator begin() {
            return {this};
        }

        iterator end() {
            return {};
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_STATEMENT_HPP
