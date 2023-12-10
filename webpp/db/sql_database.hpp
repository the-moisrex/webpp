#ifndef WEBPP_DATABASE_SQL_DATABASE_HPP
#define WEBPP_DATABASE_SQL_DATABASE_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/enable_traits.hpp"
#include "query_builder.hpp"
#include "sql_concepts.hpp"
#include "sql_statement.hpp"

namespace webpp::sql {

    template <typename CharT = char>
    struct sql_lowercase_keywords {
        static constexpr CharT const* select       = "select";
        static constexpr CharT const* update       = "update";
        static constexpr CharT const* delete_word  = "delete";
        static constexpr CharT const* values       = "values";
        static constexpr CharT const* from         = "from";
        static constexpr CharT const* where        = "where";
        static constexpr CharT const* in           = "in";
        static constexpr CharT const* null         = "null";
        static constexpr CharT const* not_word     = "not";
        static constexpr CharT const* and_word     = "and";
        static constexpr CharT const* or_word      = "or";
        static constexpr CharT const* insert       = "insert";
        static constexpr CharT const* into         = "into";
        static constexpr CharT const* default_word = "default";
        static constexpr CharT const* like         = "like";
        static constexpr CharT const* exists       = "exists";
        static constexpr CharT const* set          = "set";
        static constexpr CharT const* inner        = "inner";
        static constexpr CharT const* left         = "left";
        static constexpr CharT const* right        = "right";
        static constexpr CharT const* join         = "join";
        static constexpr CharT const* cross        = "cross";
        static constexpr CharT const* full         = "full";
        static constexpr CharT const* using_word   = "using";
        static constexpr CharT const* on_word      = "on";
        static constexpr CharT const* is           = "is";
        static constexpr CharT const* distinct     = "distinct";
        static constexpr CharT const* true_word    = "true";
        static constexpr CharT const* false_word   = "false";
    };

    template <typename CharT = char>
    struct sql_uppercase_keywords {
        static constexpr CharT const* select       = "SELECT";
        static constexpr CharT const* update       = "UPDATE";
        static constexpr CharT const* delete_word  = "DELETE";
        static constexpr CharT const* values       = "VALUES";
        static constexpr CharT const* from         = "FROM";
        static constexpr CharT const* where        = "WHERE";
        static constexpr CharT const* in           = "IN";
        static constexpr CharT const* null         = "NULL";
        static constexpr CharT const* not_word     = "NOT";
        static constexpr CharT const* and_word     = "AND";
        static constexpr CharT const* or_word      = "OR";
        static constexpr CharT const* insert       = "INSERT";
        static constexpr CharT const* into         = "INTO";
        static constexpr CharT const* default_word = "DEFAULT";
        static constexpr CharT const* like         = "LIKE";
        static constexpr CharT const* exists       = "EXISTS";
        static constexpr CharT const* set          = "SET";
        static constexpr CharT const* inner        = "INNER";
        static constexpr CharT const* left         = "LEFT";
        static constexpr CharT const* right        = "RIGHT";
        static constexpr CharT const* join         = "JOIN";
        static constexpr CharT const* cross        = "CROSS";
        static constexpr CharT const* full         = "FULL";
        static constexpr CharT const* using_word   = "USING";
        static constexpr CharT const* on_word      = "ON";
        static constexpr CharT const* is           = "IS";
        static constexpr CharT const* distinct     = "DISTINCT";
        static constexpr CharT const* true_word    = "TRUE";
        static constexpr CharT const* false_word   = "FALSE";
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
        using string_type           = traits::string<traits_type>;
        using char_type             = traits::char_type<traits_type>;
        using query_builder_type    = query_builder<basic_sql_database>;
        using connection_type       = typename driver_type::connection_type;
        using grammar_type          = typename driver_type::grammar_type;
        using keywords              = sql_lowercase_keywords<char_type>;

        template <typename T>
        static constexpr bool supports_string_view =
          driver_type::template supports_string_view<stl::remove_cvref_t<T>>;

        inline driver_type& driver() noexcept {
            return *static_cast<driver_type*>(this);
        }

        inline driver_type const& driver() const noexcept {
            return *static_cast<driver_type const*>(this);
        }

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

            constexpr bool supports_our_sview =
              supports_string_view<decltype(istl::string_viewify_of<string_view_type>(stl::forward<T>(str)))>;

            constexpr bool supports_std_sview =
              supports_string_view<decltype(istl::string_viewify(stl::forward<T>(str)))>;

            constexpr bool is_our_sview = istl::StringViewifiableOf<string_view_type, type>;
            constexpr bool is_std_sview = istl::StringViewifiable<type>;

            if constexpr (!supports_our_sview && !supports_std_sview) {
                static_assert_false(T, "Thw driver doesn't support natural string views.");
            }

            if constexpr (is_our_sview && supports_our_sview) {
                return istl::string_viewify_of<string_view_type>(stl::forward<T>(str));
            } else if constexpr (is_std_sview && supports_std_sview) {
                return istl::string_viewify(stl::forward<T>(str));
            } else {
                static_assert_false(T, "We're not able to convert the specified sql query to string view");
            }
        }

        // ctor
        using TraitsEnabler::TraitsEnabler;

        constexpr basic_sql_database() {
            auto errmsg = object::make_object<string_type>(*this);
            driver().open(errmsg);
            log(errmsg);
        }

        /**
         * Prepare a SQL query and return a statement object.
         */
        template <istl::StringViewifiable StrT>
        inline statement_type prepare(StrT&& sql_str) noexcept {
            // the driver's prepare function will throw "driver_statement_type",
            // and the "statement_type" will accept a "driver_statement_type" as its ctor argument.

            auto           errmsg = object::make_object<string_type>(*this);
            statement_type stmt{this->get_traits()};
            driver().prepare(string_viewify(stl::forward<StrT>(sql_str)), stmt, errmsg);
            log(errmsg);
            return stmt;
        }

        template <istl::StringViewifiable StrT>
        inline bool execute(StrT sql_str) noexcept {
            if constexpr (requires(driver_statement_type stmt, string_type errmsg) {
                              stmt.execute(string_viewify(stl::forward<StrT>(sql_str)), errmsg);
                          })
            {
                auto errmsg = object::make_object<string_type>(*this);
                driver().execute(string_viewify(stl::forward<StrT>(sql_str)), errmsg);
                log(errmsg);
                return errmsg.empty();
            } else {
                auto stmt = this->prepare(string_viewify(stl::forward<StrT>(sql_str)));
                return !stmt.step();
            }
        }

        inline query_builder_type sql_builder() noexcept {
            return query_builder_type{*this};
        }

        template <istl::StringViewifiable StrvT>
        inline query_builder_type table(StrvT&& table_name) noexcept {
            return sql_builder().table(stl::forward<StrvT>(table_name));
        }

      private:
        inline void log(string_type& errmsg) noexcept {
            if (errmsg.empty()) {
                return;
            }
            this->logger.error("SQL", errmsg);
        }
    };

    template <SQLDriver SQLDBType, Traits TraitsType = default_traits>
    using sql_database = basic_sql_database<SQLDBType, enable_owner_traits<TraitsType>>;

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_DATABASE_HPP
