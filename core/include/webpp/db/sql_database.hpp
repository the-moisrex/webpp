#ifndef WEBPP_DATABASE_SQL_DATABASE_HPP
#define WEBPP_DATABASE_SQL_DATABASE_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "sql_concepts.hpp"
#include "sql_statement.hpp"

#include <compare>

namespace webpp::sql {


    template <SQLStatement StmtType>
    struct sql_iterator {
        using statement_type    = StmtType;
        using value_type        = statement_type;
        using difference_type   = stl::ptrdiff_t;
        using reference         = stl::add_lvalue_reference_t<value_type>;
        using pointer           = typename stl::iterator_traits<value_type>::pointer;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

      private:
        statement_type* stmt = nullptr;

      public:
        constexpr sql_iterator() noexcept = default;
        constexpr sql_iterator(statement_type* stmt_ptr) noexcept : stmt{stmt_ptr} {}
        constexpr sql_iterator(sql_iterator const&)                = default;
        constexpr sql_iterator(sql_iterator&&) noexcept            = default;
        constexpr sql_iterator& operator=(sql_iterator const&)     = default;
        constexpr sql_iterator& operator=(sql_iterator&&) noexcept = default;

        constexpr auto operator==(const sql_iterator& rhs) noexcept {
            return base() == rhs.base();
        }

        constexpr auto operator<=>(const sql_iterator& rhs) noexcept {
            return base() <=> rhs.base();
        }

        // Forward iterator requirements
        constexpr reference operator*() const noexcept {
            return *stmt;
        }

        constexpr pointer operator->() const noexcept {
            return stmt;
        }

        constexpr sql_iterator& operator++() noexcept {
            const bool has_next = stmt->step();
            if (!has_next) {
                stmt = nullptr;
            }
            return *this;
        }

        constexpr sql_iterator operator++(int) const noexcept {
            return sql_iterator{stmt}.operator++();
        }


        constexpr const pointer& base() const noexcept {
            return stmt;
        }
    };


    /**
     * The name SQL Database and not Database is used because database can mean more and this struct will only
     * support sql or sql-like drivers.
     *
     * This struct will either be used by the user directly or it'll be used by the high level utilities. So
     * this struct needs to be user friendly.
     */
    template <SQLDriver SQLDBType, typename TraitsEnabler = enable_traits<default_traits>>
    struct basic_sql_database : TraitsEnabler, SQLDBType {
        using driver_type           = SQLDBType;
        using etraits               = TraitsEnabler;
        using traits_type           = typename etraits::traits_type;
        using driver_statement_type = typename driver_type::statement_type;
        using statement_type        = sql_statement<traits_type, driver_statement_type>;
        using string_view_type      = traits::string_view<traits_type>;
        using string_type           = traits::general_string<traits_type>;
        using iterator              = sql_iterator<statement_type>;

        template <typename T>
        static constexpr bool supports_string_view = typename driver_type::template supports_string_view<T>;

        /**
         * This method converts the input into string view. String view type is chosen with this priority:
         *  1. Matches the string view type specified by the traits type, if not then
         *  2. the default string view is chosen
         *
         * If the resulted string view is not what the database can accept, then a compile-time error is
         * thrown.
         */
        template <typename T>
        static constexpr decltype(auto) string_viewify(T&& str) noexcept {
            using type = stl::remove_cvref_t<T>;
            if constexpr (istl::StringViewifiableOf<string_view_type, type> &&
                          supports_string_view<decltype(istl::string_viewify_of<string_view_type>(
                            stl::forward<T>(str)))>) {
                return istl::string_viewify_of<string_view_type>(stl::forward<T>(str));
            } else if constexpr (istl::StringViewifiable<type> &&
                                 supports_string_view<decltype(istl::string_viewify(stl::forward<T>(str)))>) {
                return istl::string_viewify(stl::forward<T>(str));
            } else {
                static_assert_false(T, "We're not able to convert the specified sql query to string view");
            }
        }

        // ctor
        using TraitsEnabler::TraitsEnabler;

        /**
         * Prepare a SQL query and return a statement object.
         */
        template <istl::StringViewifiable StrT>
        inline statement_type prepare(StrT&& sql_str) noexcept {
            // the driver's prepare function will throw "driver_statement_type",
            // and the "statement_type" will accept a "driver_statement_type" as its ctor argument.

            string_type    errmsg;
            statement_type stmt{this->prepare(string_viewify(stl::forward<StrT>(sql_str)), errmsg),
                                this->get_traits()};
            log(errmsg);
            return stmt;
        }


        template <istl::StringViewifiable StrT>
        inline basic_sql_database& execute(StrT sql_str) noexcept {
            // should we use local string for this? because the chance of an error message is slim and we
            // don't like to pay for more than needed and having a stack seems more work than needed.
            // todo: the above idea needs benchmarking
            string_type errmsg;
            this->execute(string_viewify(stl::forward<StrT>(sql_str)), errmsg);
            log(errmsg);
        }


        iterator begin() {
            return {this};
        }

        iterator end() {
            return {};
        }

      private:
        inline void log(string_type& errmsg) noexcept {
            if (errmsg.empty())
                return;
            this->logger.error("SQL", errmsg);
        }
    };


    template <SQLDriver SQLDBType, Traits TraitsType = default_traits>
    using sql_database = basic_sql_database<SQLDBType, enable_owner_traits<TraitsType>>;

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_DATABASE_HPP
