#ifndef WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
#define WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP

#include "../memory/object.hpp"
#include "../std/collection.hpp"
#include "../std/functional.hpp"
#include "../std/memory.hpp"
#include "../std/ranges.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/join.hpp"
#include "../traits/traits.hpp"
#include "sql_concepts.hpp"

#include <array>
#include <variant>

namespace webpp::sql {


    template <typename DBType, Allocator AllocT>
    struct query_builder;

    namespace details {
        // NOLINTBEGIN(*-macro-usage)
#define define_expression(name, ...)                                                                           \
    template <typename DBType, typename AllocT>                                                                \
    struct name {                                                                                              \
        using database_type        = DBType;                                                                   \
        using traits_type          = typename database_type::traits_type;                                      \
        using allocator_type       = AllocT;                                                                   \
        using string_type          = traits::string<traits_type, allocator_type>;                              \
        using string_view_type     = traits::string_view<traits_type>;                                         \
        using database_ref         = stl::add_lvalue_reference_t<database_type>;                               \
        using size_type            = typename database_type::size_type;                                        \
        using db_float_type        = typename database_type::float_type;                                       \
        using db_integer_type      = typename database_type::integer_type;                                     \
        using db_string_type       = typename database_type::string_type;                                      \
        using db_blob_type         = typename database_type::blob_type;                                        \
        using keywords             = typename database_type::keywords;                                         \
        using expression_sig       = void(string_type&, database_ref) const noexcept;                          \
        using alloc_traits         = stl::allocator_traits<allocator_type>;                                    \
        using expression_allocator = typename alloc_traits::template rebind_alloc<stl::byte>;                  \
        using expr_func            = istl::function<expression_sig, expression_allocator>;                     \
        using expr_vec      = stl::vector<expr_func, typename alloc_traits::template rebind_alloc<expr_func>>; \
        using subquery_type = query_builder<DBType, allocator_type>;                                           \
        using subquery_ptr =                                                                                   \
          istl::dynamic<subquery_type, typename alloc_traits::template rebind_alloc<subquery_type>>;           \
                                                                                                               \
        using driver_type     = typename database_type::driver_type;                                           \
        using grammar_type    = typename database_type::grammar_type;                                          \
        using connection_type = typename database_type::connection_type;                                       \
        struct expr_data {                                                                                     \
            __VA_ARGS__                                                                                        \
        };                                                                                                     \
                                                                                                               \
      private:                                                                                                 \
        expr_data m_data;                                                                                      \
                                                                                                               \
      public:                                                                                                  \
        constexpr expr_data& data() noexcept {                                                                 \
            return m_data;                                                                                     \
        }                                                                                                      \
        constexpr expr_data const& data() const noexcept {                                                     \
            return m_data;                                                                                     \
        }                                                                                                      \
                                                                                                               \
        explicit constexpr name(expr_data&& input_data) noexcept : m_data{stl::move(input_data)} {}            \
        constexpr name(name const&)                = default;                                                  \
        constexpr name(name&&) noexcept            = default;                                                  \
        constexpr ~name()                          = default;                                                  \
        constexpr name& operator=(name const&)     = default;                                                  \
        constexpr name& operator=(name&&) noexcept = default;                                                  \
                                                                                                               \
        constexpr void operator()(string_type& out, [[maybe_unused]] database_ref db_ref) const noexcept;      \
    };                                                                                                         \
    template <typename DBType, typename AllocT>                                                                \
    constexpr void name<DBType, AllocT>::operator()(                                                           \
      typename name<DBType, AllocT>::string_type&                  out,                                        \
      [[maybe_unused]] typename name<DBType, AllocT>::database_ref db_ref) const noexcept

        // NOLINTEND(*-macro-usage)

        // literal value
        define_expression(floating_expr, db_float_type val;) {
            out.append(lexical::cast<string_type>(data().val, db_ref));
        }

        define_expression(integer_expr, db_integer_type val;) {
            out.append(lexical::cast<string_type>(data().val, db_ref));
        }

        define_expression(string_expr, string_type val;) {
            db_ref.quoted_escape(data().val, out);
        }

        define_expression(blob_expr, db_blob_type val;) {
            out.append(lexical::cast<string_type>(data().val, db_ref));
        }

        define_expression(bool_expr, bool val;) {
            out.append(data().val ? keywords::true_word : keywords::false_word);
        }

        define_expression(null_expr) {
            out.append(keywords::null);
        }

        define_expression(col_name_expr, string_type schema_name{}, table_name{}, column_name;) {
            db_ref.quoted_escape(data().schema_name, out);
            db_ref.quoted_escape(data().table_name, out);
            db_ref.quoted_escape(data().column_name, out);
        }

        // op expr
        inline constexpr stl::array<stl::string_view, 5>
          unary_op_expr_op_strs{" + ", " - ", " ++", " --", " !"};

        define_expression(
          unary_op_expr, enum struct operation
          : stl::uint_fast8_t{plus, minus, incr, decr, negate, and_op, or_op, and_not, or_not} op;
          expr_func expr;) {
            switch (data().op) {
                case expr_data::operation::and_op: {
                    out.push_back(' ');
                    out.append(keywords::and_word);
                    out.push_back(' ');
                    break;
                }
                case expr_data::operation::or_op: {
                    out.push_back(' ');
                    out.append(keywords::or_word);
                    out.push_back(' ');
                    break;
                }
                case expr_data::operation::and_not: {
                    out.push_back(' ');
                    out.append(keywords::and_word);
                    out.push_back(' ');
                    out.append(keywords::not_word);
                    out.push_back(' ');
                    break;
                }
                case expr_data::operation::or_not: {
                    out.push_back(' ');
                    out.append(keywords::or_word);
                    out.push_back(' ');
                    out.append(keywords::not_word);
                    out.push_back(' ');
                    break;
                }
                default: {
                    out.append(unary_op_expr_op_strs[static_cast<stl::uint_fast8_t>(data().op)]);
                }
            }
            data().expr(out, db_ref);
        }

        // expr op expr
        inline constexpr stl::array<stl::string_view, 11> expr_op_expr_op_strs{
          " + ",
          " - ",
          " * ",
          " / ",
          " % ",
          " = ",
          " != ",
          " > ",
          " < ",
          " >= ",
          " <= "};

        define_expression(expr_op_expr, enum struct operation
                          : stl::uint_fast8_t{add, sub, mul, div, modulo, eq, neq, gt, lt, ge, le} op;
                          expr_func left_expr, right_expr;) {
            data().left_expr(out, db_ref);
            out.append(expr_op_expr_op_strs[static_cast<stl::uint_fast8_t>(data().op)]);
            data().right_expr(out, db_ref);
        }

        // ( expr, expr, expr, ... )
        define_expression(expr_list, expr_vec exprs;) {
            out.push_back('(');
            auto       pos    = data().exprs.begin();
            auto const it_end = data().exprs.end();
            for (;;) {
                (*pos)(out, db_ref);
                ++pos;
                if (pos == it_end) {
                    break;
                }
                out.append(", ");
            }
            out.push_back(')');
        }

        define_expression(expr_is_null, enum struct operation
                          : stl::uint8_t{is_null, not_null} op;
                          expr_func expr;) {
            data().expr(out, db_ref);
            out.push_back(' ');
            switch (data().op) {
                case expr_data::operation::is_null: {
                    out.append(keywords::is);
                    out.push_back(' ');
                    out.append(keywords::null);
                    break;
                }
                case expr_data::operation::not_null: {
                    out.append(keywords::not_word);
                    out.push_back(' ');
                    out.append(keywords::null);
                    break;
                }
                default: stl::unreachable();
            }
        }

        define_expression(expr_is_expr, enum struct operation
                          : stl::uint8_t{is, is_not, is_distinct, is_not_distinct} op;
                          expr_func left_expr, right_expr;) {
            data().left_expr(out, db_ref);
            out.push_back(' ');
            switch (data().op) {
                case expr_data::operation::is: {
                    out.append(keywords::is);
                    break;
                }
                case expr_data::operation::is_not: {
                    out.append(keywords::is);
                    out.push_back(' ');
                    out.append(keywords::not_word);
                    break;
                }
                case expr_data::operation::is_distinct: {
                    out.append(keywords::is);
                    out.push_back(' ');
                    out.append(keywords::distinct);
                    out.push_back(' ');
                    out.append(keywords::from);
                    break;
                }
                case expr_data::operation::is_not_distinct: {
                    out.append(keywords::is);
                    out.push_back(' ');
                    out.append(keywords::not_word);
                    out.push_back(' ');
                    out.append(keywords::distinct);
                    out.push_back(' ');
                    out.append(keywords::from);
                    break;
                }
                default: stl::unreachable();
            }
            out.push_back(' ');
            data().right_expr(out, db_ref);
        }

        // left_expr not in (expr, expr, expr, ...)
        // left_expr in (select-stmt)
        define_expression(
          expr_in_expr, enum struct operation
          : stl::uint8_t{in, not_in} op;
          expr_func    left_expr;
          expr_vec     exprs;
          subquery_ptr select_stmt;) {
            data().left_expr(out, db_ref);
            out.push_back(' ');
            if (data().op == expr_data::operation::not_in) {
                out.append(keywords::not_word);
                out.push_back(' ');
            }
            out.append(keywords::in);
            out.append(" (");
            if (!data().exprs.empty()) {
                auto       pos    = data().exprs.begin();
                auto const it_end = data().exprs.end();
                for (;;) {
                    (*pos)(out, db_ref);
                    ++pos;
                    if (pos == it_end) {
                        break;
                    }
                    out.append(", ");
                }
            } else {
                data().select_stmt->to_string(out);
            }
            out.push_back(')');
        }

#undef define_expression
    } // namespace details

    namespace details {
        template <typename DBType>
        struct query_builder_subclasses {
            using database_type  = DBType;
            using traits_type    = typename database_type::traits_type;
            using string_type    = traits::general_string<traits_type>;
            using allocator_type = traits::general_string_allocator<traits_type>;
            using subquery_type  = query_builder<database_type, allocator_type>;

            [[no_unique_address]] struct table_type {
                inline constexpr subquery_type& enclosing() noexcept {
                    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    return *reinterpret_cast<subquery_type*>(
                      reinterpret_cast<char*>(this) - offsetof(query_builder_subclasses, table));
                    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
                }

                // set the name
                template <istl::Stringifiable... StrvT>
                constexpr subquery_type& name(StrvT&&... in_table_name) noexcept {
                    enclosing().from_cols.clear();
                    (enclosing().from_cols.push_back(istl::stringify_of<string_type>(
                       stl::forward<StrvT>(in_table_name),
                       enclosing().template get_allocator<string_type>())),
                     ...);
                    return enclosing();
                }

                template <istl::Stringifiable... StrvT>
                constexpr subquery_type& operator()(StrvT&&... in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name)...);
                }

                template <istl::Stringifiable... StrvT>
                constexpr subquery_type& operator[](StrvT&&... in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name)...);
                }

                // NOLINTBEGIN(cppcoreguidelines-c-copy-assignment-signature,
                // misc-unconventional-assign-operator)
                template <istl::Stringifiable StrvT>
                constexpr subquery_type& operator=(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

                // NOLINTEND(cppcoreguidelines-c-copy-assignment-signature,
                // misc-unconventional-assign-operator)

            } table;
        };

#undef define_enclosing
    } // namespace details

    /**
     * This class is used in query builder class in order to let the users do this:
     * @code
     *   builder["col_name"] = "value";
     *   builder["col_int"]  = 313;
     *   builder[1]          = 5.2; // bind with index
     * @endcode
     * @tparam DBType
     */
    template <typename DBType, typename KeyType>
    struct column_builder {
        using database_type     = DBType;
        using traits_type       = typename database_type::traits_type;
        using allocator_type    = traits::general_string_allocator<traits_type>;
        using subquery_type     = query_builder<database_type, allocator_type>;
        using query_builder_ref = stl::add_lvalue_reference_t<subquery_type>;
        using key_type          = KeyType;
        using string_type       = typename subquery_type::string_type;

      private:
        query_builder_ref builder;
        key_type          key;


      public:
        constexpr column_builder(query_builder_ref input_builder, key_type&& input_key) noexcept
          : builder{input_builder},
            key{stl::move(input_key)} {}

        constexpr column_builder(query_builder_ref input_builder, key_type const& input_key) noexcept
          : builder{input_builder},
            key{input_key} {}

        // set the value for the specified column
        template <typename T>
        constexpr column_builder& operator=(T&& value) noexcept {
            builder.columns.push_back(key);
            builder.values.push_back(builder.expressionify(stl::forward<T>(value)));
            return *this;
        }

        constexpr column_builder& operator++() noexcept {
            builder.columns.push_back(key);
            // todo
            // builder.values.push_back();
            return *this;
        }
    };

    /**
     * This is a query builder class
     * @tparam DBType Database type
     */
    template <typename DBType,
              Allocator AllocT = traits::general_string_allocator<typename DBType::traits_type>>
    struct query_builder : details::query_builder_subclasses<DBType> {
        using database_type    = DBType;
        using traits_type      = typename database_type::traits_type;
        using allocator_type   = AllocT;
        using string_type      = traits::string<traits_type, allocator_type>;
        using string_view_type = traits::string_view<traits_type>;
        using database_ref     = stl::add_lvalue_reference_t<database_type>;
        using size_type        = typename database_type::size_type;
        using db_float_type    = typename database_type::float_type;
        using db_integer_type  = typename database_type::integer_type;
        using db_string_type   = typename database_type::string_type;
        using db_blob_type     = typename database_type::blob_type;
        using keywords         = typename database_type::keywords;
        using expression_sig   = void(string_type&, database_ref) const noexcept;
        using alloc_traits     = stl::allocator_traits<allocator_type>;
        using expr_func =
          istl::function<expression_sig, typename alloc_traits::template rebind_alloc<stl::byte>>;
        using expression_allocator = typename alloc_traits::template rebind_alloc<expr_func>;
        using expr_vec             = stl::vector<expr_func, expression_allocator>;
        using subquery_type        = query_builder<DBType, allocator_type>;
        using subquery_allocator   = typename alloc_traits::template rebind_alloc<subquery_type>;
        using subquery_ptr         = istl::dynamic<subquery_type, subquery_allocator>;

        using driver_type     = typename database_type::driver_type;
        using grammar_type    = typename database_type::grammar_type;
        using connection_type = typename database_type::connection_type;


        template <typename T>
        static constexpr bool is_expression_v = stl::is_invocable_v<T, string_type&, database_ref>;

        static constexpr auto LOG_CAT = "SQLBuilder";

        template <typename, typename>
        friend struct column_builder;

        template <typename ADBType>
        friend struct details::query_builder_subclasses;

      private:
        using subquery =
          istl::dynamic<query_builder, typename alloc_traits::template rebind_alloc<query_builder>>;

        // todo: should we add subquery here?
        using expr_variant =
          stl::variant<stl::monostate, db_float_type, db_integer_type, db_string_type, db_blob_type>;


        using col_expr_pair =
          stl::pair<string_type, expr_variant>; // todo: anywhere that this being used should be re-checked
        using string_vec =
          stl::vector<string_type, typename alloc_traits::template rebind_alloc<string_type>>;

        // https://www.sqlite.org/syntax/table-or-subquery.html
        using table_or_subquery_type = stl::variant<string_type, subquery>;


        // create query is not included in the query builder class
        enum struct query_method : stl::uint_fast8_t {
            select,
            insert,
            insert_default,
            update,
            remove,
            none
        };
        enum struct order_by_type : stl::uint_fast8_t {
            asc,
            desc
        };

        struct join_type {
            enum struct join_cat : stl::uint_fast8_t {
                inner = 0,
                left  = 1,
                right = 2,
                full  = 3,
                cross = 4
            };
            enum struct cond_type : stl::uint_fast8_t {
                using_cond,
                on_cond,
                none
            };

            join_cat  cat;
            cond_type cond = cond_type::none;

            // https://www.sqlite.org/syntax/join-clause.html
            // https://www.sqlite.org/syntax/table-or-subquery.html
            table_or_subquery_type table; // table or sub-query

            // todo: these two can be combined in a variant
            expr_func  expr;      // for "on"
            string_vec col_names; // for using
        };

        using join_vec = stl::vector<join_type, typename alloc_traits::template rebind_alloc<join_type>>;

        database_ref db;
        query_method method = query_method::none;
        string_vec   from_cols;
        string_vec   columns;     // insert: col names, update: col names, select: cols, delete: unused
        expr_vec     values;      // insert: values, update: values, select: unused, delete: unused
        subquery_ptr select_stmt; // insert
        expr_vec     where_clauses;
        join_vec     joins;
        // order_by_type order_by_value;



        template <typename T>
        static constexpr bool is_stringify = istl::StringifiableOf<string_type, T>;

        // helper to convert the input to acceptable string type
        template <typename T>
            requires(is_stringify<T>)
        constexpr auto stringify(T&& str) const noexcept {
            return istl::stringify_of<string_type>(stl::forward<T>(str), general_alloc_for<string_type>(db));
        }


      public:
        template <typename InpAllocT = allocator_type>
        explicit constexpr query_builder(database_ref input_db, InpAllocT const& alloc = {}) noexcept
          : db{input_db},
            from_cols{alloc},
            columns{alloc},
            values{alloc},
            select_stmt{alloc},
            where_clauses{alloc},
            joins{alloc} {}

        constexpr query_builder(query_builder&&) noexcept      = default;
        constexpr query_builder(query_builder const&) noexcept = default;
        constexpr ~query_builder()                             = default;

        constexpr query_builder& operator=(query_builder&&) noexcept      = default;
        constexpr query_builder& operator=(query_builder const&) noexcept = default;

        template <typename T>
        [[nodiscard]] constexpr auto get_allocator() const noexcept {
            using new_alloc_type = typename alloc_traits::template rebind_alloc<T>;
            return new_alloc_type{from_cols.get_allocator()}; // any of them will do
        }

        /**
         * Set columns to be selected in the sql query.
         */
        template <typename... T>
            requires((istl::StringifiableOf<string_type, T> && ...))
        constexpr query_builder& select(T&&... cols) noexcept {
            method = query_method::select;
            columns.reserve(columns.size() + sizeof...(T));
            (columns.push_back(stringify(stl::forward<T>(cols))), ...);
            return *this;
        }

        template <typename StrT>
            requires(istl::StringifiableOf<string_type, StrT>)
        constexpr column_builder<database_type, string_type> operator[](StrT&& col_name) noexcept {
            return {*this, stringify(stl::forward<StrT>(col_name))};
        }

        constexpr column_builder<database_type, stl::size_t> operator[](stl::size_t col_index) noexcept {
            return {*this, col_index};
        }

        /**
         * SQL Example:
         *   SELECT city
         *    FROM offices
         *    WHERE office_code IN (SELECT office_code
         *      FROM  office_revenue
         *      WHERE revenue < 200000);
         * @param expr1
         * @param select_query
         * @return
         */
        template <typename Expr1>
        constexpr query_builder& where_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            using expr_type = details::expr_in_expr<database_type, allocator_type>;
            where_clauses.clear();
            where_clauses.push_back(expressionify(expr_type{
              {.op          = expr_type::expr_data::operation::in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr(get_allocator<subquery_type>(), select_query)}
            }));
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            using expr_type = details::expr_in_expr<database_type, allocator_type>;
            where_clauses.clear();
            where_clauses.push_back(expressionify(expr_type{
              {.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>(), select_query}}
            }));
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& and_where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            if (where_clauses.empty()) {
                db.logger.error(LOG_CAT,
                                "You're executing and/or_where... member functions while you haven't done a "
                                "where... member functions before it.");
                return *this;
            }

            using expr_type = details::expr_in_expr<database_type, allocator_type>;
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            where_clauses.push_back(expressionify(and_expr{
              {.op   = and_expr::expr_data::operation::and_op,
               .expr = expr_type{{.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = expressionify(select_query)}}}
            }));
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& or_where_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            if (where_clauses.empty()) {
                db.logger.error(LOG_CAT,
                                "You're executing and/or_where... member functions while you haven't done a "
                                "where... member functions before it.");
                return *this;
            }

            using expr_type = details::expr_in_expr<database_type, allocator_type>;
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            where_clauses.push_back(expressionify(and_expr{
              {.op   = and_expr::expr_data::operation::or_op,
               .expr = expr_type{{.op          = expr_type::expr_data::operation::in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = expressionify(select_query)}}}
            }));
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& or_where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            if (where_clauses.empty()) {
                db.logger.error(LOG_CAT,
                                "You're executing and/or_where... member functions while you haven't done a "
                                "where... member functions before it.");
                return *this;
            }

            using expr_type = details::expr_in_expr<database_type, allocator_type>;
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            where_clauses.push_back(expressionify(and_expr{
              {.op = and_expr::expr_data::operation::or_op,
               .expr =
               expr_type{{.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr(get_allocator<subquery_type>(), select_query)}}}
            }));
            return *this;
        }

        /**
         * Support for:
         *   select * from table where expr1 in (expr2, exprs...);
         */
        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& where_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            where_clauses.clear();
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            expr_type clause{
              typename expr_type::expr_data{.op          = expr_type::expr_data::operation::in,
                                            .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                                            .exprs       = expr_vec{get_allocator<expr_func>()},
                                            .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}
            };
            clause.data().exprs.reserve(sizeof...(exprs));
            (clause.data().exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& where_not_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            where_clauses.clear();
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            expr_type clause{
              {.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}
            };
            clause.data().exprs.reserve(sizeof...(exprs));
            (clause.data().exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& and_where_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            and_expr clause{
              {.op   = and_expr::opreation::and_op,
               .expr = expressionify(
               expr_type{{.op          = expr_type::expr_data::operation::in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}})}
            };
            clause.data().expr.template as<expr_type>().data().exprs.reserve(sizeof...(exprs));
            (clause.data().expr.template as<expr_type>().data().exprs.push_back(
               expressionify<Exprs>(stl::forward<Exprs>(exprs))),
             ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& or_where_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            and_expr clause{
              {.op   = and_expr::expr_data::operation::or_op,
               .expr = expressionify(
               expr_type{{.op          = expr_type::expr_data::operation::in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}})}
            };
            clause.data().expr.template as<expr_type>().data().exprs.reserve(sizeof...(exprs));
            (clause.data().expr.template as<expr_type>().data().exprs.push_back(
               expressionify<Exprs>(stl::forward<Exprs>(exprs))),
             ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& or_where_not_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            and_expr clause{
              {.op   = and_expr::opreation::or_op,
               .expr = expressionify(
               expr_type{{.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}})}
            };
            clause.data().expr.template as<expr_type>().data().exprs.reserve(sizeof...(exprs));
            (clause.data().expr.template as<expr_type>().data().exprs.push_back(
               expressionify<Exprs>(stl::forward<Exprs>(exprs))),
             ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename... Exprs>
            requires(sizeof...(Exprs) >= 1)
        constexpr query_builder& and_where_not_in(Expr1&& expr1, Exprs&&... exprs) noexcept {
            using and_expr  = details::unary_op_expr<database_type, allocator_type>;
            using expr_type = details::expr_in_expr<database_type, allocator_type>;

            and_expr clause{
              {.op   = and_expr::expr_data::operation::and_op,
               .expr = expressionify(
               expr_type{{.op          = expr_type::expr_data::operation::not_in,
               .left_expr   = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .exprs       = expr_vec{get_allocator<expr_func>()},
               .select_stmt = subquery_ptr{get_allocator<subquery_type>()}}})}
            };
            clause.data().expr.template as<expr_type>().data().exprs.reserve(sizeof...(exprs));
            (clause.data().expr.template as<expr_type>().data().exprs.push_back(
               expressionify<Exprs>(stl::forward<Exprs>(exprs))),
             ...);
            where_clauses.push_back(expressionify(stl::move(clause)));
            return *this;
        }

        template <typename Expr1, typename Expr2>
        constexpr query_builder& where(Expr1&& expr1, Expr2&& expr2) noexcept {
            using expr_type = details::expr_op_expr<database_type, allocator_type>;
            where_clauses.clear();
            where_clauses.push_back(expressionify(expr_type{
              {.op         = expr_type::expr_data::operation::eq,
               .left_expr  = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
               .right_expr = expressionify<Expr2>(stl::forward<Expr2>(expr2))}
            }));
            return *this;
        }

        template <typename ColT, typename... StrT>
            requires(
              istl::StringifiableOf<string_type, ColT> && (istl::StringifiableOf<string_type, StrT> && ...))
        constexpr query_builder& left_join_using(ColT&& col_string, StrT&&... col_names) noexcept {
            joins.push_back(join_type{.cat       = join_type::join_cat::left,
                                      .cond      = join_type::cond_type::using_cond,
                                      .table     = stringify(stl::forward<ColT>(col_string)),
                                      .expr      = expressionify(nullptr),
                                      .col_names = string_vec{get_allocator<string_type>()}});
            auto& back = joins.back();
            (back.col_names.emplace_back(stringify(stl::forward<StrT>(col_names))), ...);
            return *this;
        }

        template <typename... StrT>
            requires(istl::StringifiableOf<string_type, StrT> && ...)
        constexpr query_builder& left_join_using(query_builder const& sub_query,
                                                 StrT&&... col_names) noexcept {
            joins.push_back(join_type{.cat       = join_type::join_cat::left,
                                      .cond      = join_type::cond_type::using_cond,
                                      .table     = sub_query,
                                      .expr      = expressionify(nullptr),
                                      .col_names = string_vec{get_allocator<string_type>()}});
            auto& back = joins.back();
            (back.col_names.emplace_back(stringify(stl::forward<StrT>(col_names))), ...);
            return *this;
        }

        template <typename ColT, typename... StrT>
            requires(
              istl::StringifiableOf<string_type, ColT> && (istl::StringifiableOf<string_type, StrT> && ...))
        constexpr query_builder& right_join_using(ColT&& col_string, StrT&&... col_names) noexcept {
            joins.push_back(join_type{.cat       = join_type::join_cat::right,
                                      .cond      = join_type::cond_type::using_cond,
                                      .table     = stringify(stl::forward<ColT>(col_string)),
                                      .expr      = expressionify(nullptr),
                                      .col_names = string_vec{get_allocator<string_type>()}});
            auto& back = joins.back();
            (back.col_names.emplace_back(stringify(stl::forward<StrT>(col_names))), ...);
            return *this;
        }

        template <typename... StrT>
            requires(istl::StringifiableOf<string_type, StrT> && ...)
        constexpr query_builder& right_join_using(query_builder const& sub_query,
                                                  StrT&&... col_names) noexcept {
            joins.push_back(join_type{.cat       = join_type::join_cat::right,
                                      .cond      = join_type::cond_type::using_cond,
                                      .table     = sub_query,
                                      .expr      = expressionify(nullptr),
                                      .col_names = string_vec{get_allocator<string_type>()}});
            auto& back = joins.back();
            (back.col_names.emplace_back(stringify(stl::forward<StrT>(col_names))), ...);
            return *this;
        }

        // insert into Col default values;
        constexpr query_builder& insert_default() noexcept {
            method = query_method::insert_default;
            return *this;
        }

        constexpr query_builder& insert() noexcept {
            method = query_method::insert;
            return *this;
        }

        constexpr query_builder& insert(query_builder const& new_builder) noexcept {
            if (new_builder.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed");
                return *this;
            }

            method      = query_method::insert;
            select_stmt = new_builder;
            return *this;
        }

        constexpr query_builder& insert(query_builder&& new_builder) noexcept {
            if (new_builder.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed");
                return *this;
            }

            method      = query_method::insert;
            select_stmt = stl::move(new_builder);
            return *this;
        }

        constexpr query_builder& insert(
          stl::initializer_list<col_expr_pair> const& input_cols_vals) noexcept {
            return insert<stl::initializer_list<col_expr_pair>>(input_cols_vals);
        }

        // insert a single row
        template <istl::ReadOnlyCollection VecOfColVal = stl::initializer_list<col_expr_pair>>
        constexpr query_builder& insert(VecOfColVal const& input_cols_vals) noexcept {
            method = query_method::insert;
            // Steps:
            //   1. merge columns (we might have a new column in the cols_vals that we didn't know
            //   before)
            //   2. re-adjust the sizes of the values to match to the new column size; insert null
            //   values
            //   3. sort the values based on the columns

            stl::span cols_vals{input_cols_vals.begin(), input_cols_vals.end()};

            auto const values_last = stl::prev(values.end());

            // 1. finding if we have a new column:
            // 3. sorting cols_vals based on the columns
            auto col_it = columns.begin();
            auto pos    = cols_vals.begin();
            for (;;) {
                auto const& [col, val] = *pos;
                if (col != *col_it) {
                    auto next_it = stl::next(pos);
                    if (next_it != cols_vals.end()) {
                        stl::swap(pos, next_it);
                    } else {
                        // found a new column
                        using diff_type     = typename string_vec::iterator::difference_type;
                        auto const col_size = static_cast<diff_type>(columns.size());
                        columns.push_back(col);

                        // 2. Adding new and null variables into the values to adjust the values
                        // matrix
                        for (
                          auto val_it = values.begin() + col_size; val_it != values_last; val_it += col_size)
                        {
                            // insert a null variable at that position
                            values.insert(val_it, expressionify(nullptr));
                        }
                    }
                } else {
                    // found the column
                    // now "it" and "col_it" are in the right order
                    values.push_back(expressionify(val));
                    ++col_it;
                    ++pos;
                    if (pos == cols_vals.end()) {
                        break;
                    }
                }
            }

            return *this;
        }

        /**
         * Set the query type as an update query
         */
        constexpr query_builder& update() noexcept {
            method = query_method::update;
            return *this;
        }

        /**
         * Set the query type as delete query
         * The name "remove" has been chosen because "delete" is a reserved keyword.
         */
        constexpr query_builder& remove() noexcept {
            method = query_method::remove;
            return *this;
        }

        /**
         * Build the query and get a string for the query
         *
         * @tparam StrT String type
         * @param out output
         */
        template <typename StrT = string_type>
        constexpr void to_string(StrT& out) const noexcept {
            switch (method) {
                case query_method::insert: {
                    serialize_insert(out);
                    break;
                }
                case query_method::select: {
                    out.append(keywords::select);
                    out.push_back(' ');
                    serialize_select_columns(out);
                    out.push_back(' ');
                    out.append(keywords::from);
                    out.push_back(' ');
                    serialize_from(out);
                    serialize_joins(out);
                    serialize_where(out);
                    break;
                }
                case query_method::insert_default: {
                    // todo: "or abort, or fail, or ignore, or replace, or rollback"
                    // todo: replace into
                    out.append(keywords::insert);
                    out.push_back(' ');
                    out.append(keywords::into);
                    out.push_back(' ');
                    serialize_from(out);
                    out.push_back(' ');
                    out.append(keywords::default_word);
                    out.push_back(' ');
                    out.append(keywords::values);
                    break;
                }
                case query_method::update: {
                    serialize_update(out);
                    break;
                }
                case query_method::remove: {
                    serialize_remove(out);
                    break;
                }
                case query_method::none: {
                    // the query is empty.
                    db.logger.warning(LOG_CAT,
                                      "Calling to_string on a query builder while you haven't defined the "
                                      "query type has no effect. Did you forget calling one of 'remove', "
                                      "'update', or 'select' member functions?");
                }
            }
        }

        template <typename StrT = string_type>
        constexpr StrT to_string() const noexcept {
            auto out = object::make_general<StrT>(db);
            to_string<StrT>(out);
            return out;
        }

      private:
        template <typename V>
        constexpr expr_func expressionify(V&& val) const noexcept {
            using vtype = stl::remove_cvref_t<V>;
            using details::blob_expr;
            using details::bool_expr;
            using details::floating_expr;
            using details::integer_expr;
            using details::null_expr;
            using details::string_expr;
            using stl::same_as;
            if constexpr (same_as<vtype, expr_variant>) {
                if (auto* float_ptr = stl::get_if<db_float_type>(&val)) {
                    return expressionify(*float_ptr);
                }
                if (auto* int_ptr = stl::get_if<db_integer_type>(&val)) {
                    return expressionify(*int_ptr);
                }
                if (auto* str_ptr = stl::get_if<db_string_type>(&val)) {
                    return expressionify(*str_ptr);
                }
                if (auto* blob_ptr = stl::get_if<db_blob_type>(&val)) {
                    return expressionify(*blob_ptr);
                }
                return expressionify(nullptr);
            } else if constexpr (same_as<vtype, expr_func>) {
                return stl::forward<V>(val);
            } else if constexpr (is_expression_v<vtype>) {
                return {stl::forward<V>(val), get_allocator<expr_func>()};
            } else if constexpr (same_as<vtype, db_float_type>) {
                using expr_type = floating_expr<database_type, allocator_type>;
                return {expr_type{{.val = stl::forward<V>(val)}}, get_allocator<expr_func>()};
            } else if constexpr (same_as<vtype, db_integer_type>) {
                using expr_type = integer_expr<database_type, allocator_type>;
                return {expr_type{{.val = stl::forward<V>(val)}}, get_allocator<expr_func>()};
            } else if constexpr (same_as<vtype, string_type>) {
                using expr_type = string_expr<database_type, allocator_type>;
                return {expr_type{{.val = stl::forward<V>(val)}}, get_allocator<expr_func>()};
            } else if constexpr (same_as<vtype, bool>) {
                using expr_type = bool_expr<database_type, allocator_type>;
                return {expr_type{{.val = val}}, get_allocator<expr_func>()};
            } else if constexpr (same_as<vtype, stl::nullptr_t>) {
                using expr_type = null_expr<database_type, allocator_type>;
                return {expr_type{typename expr_type::expr_data{}}, get_allocator<expr_func>()};
            } else if constexpr (stl::floating_point<vtype>) {
                using expr_type = floating_expr<database_type, allocator_type>;
                return {expr_type{{.val = static_cast<db_float_type>(val)}}, get_allocator<expr_func>()};
            } else if constexpr (stl::integral<vtype>) {
                using expr_type = integer_expr<database_type, allocator_type>;
                return {expr_type{{.val = static_cast<db_integer_type>(val)}}, get_allocator<expr_func>()};
            } else if constexpr (istl::StringifiableOf<string_type, V>) {
                using expr_type = string_expr<database_type, allocator_type>;
                return expr_func(expr_type{{.val = stringify(stl::forward<V>(val))}},
                                 get_allocator<expr_func>());
            } else {
                static_assert_false(V, "The specified type is not a valid SQL expression.");
                return {}; // just to get rid of the warning
            }
        }

        /**
         * This function will stringify the values, if you're looking for the function that handles
         * the prepare statements, this is not going to be used there.
         *
         * https://www.sqlite.org/syntax/expr.html
         */
        template <typename StrT>
        constexpr void serialize_expression(StrT& out, expr_func const& expr) const noexcept {
            expr(out, db);
        }

        constexpr void serialize_from(auto& out) const noexcept {
            auto       iter     = from_cols.begin();
            auto const from_end = from_cols.end();
            if (iter == from_end) {
                db.logger.error(LOG_CAT,
                                "You requested a sql query but you didn't provide which table we should put "
                                "into the sql query; did you miss the table name?");
                return;
            }
            db.quoted_escape(*iter, out);
            ++iter;
            while (iter != from_end) {
                out.append(", ");
                db.quoted_escape(*iter, out);
                ++iter;
            }
        }

        constexpr void serialize_single_from(auto& out) const noexcept {
            if (from_cols.empty()) {
                db.logger.error(LOG_CAT,
                                "You requested a sql query but you didn't provide which table we should put "
                                "into the sql query; did you miss the table name?");
                return;
            }
            db.quoted_escape(from_cols.front(), out);
        }

        // select [... this method ...] from table;
        constexpr void serialize_select_columns(auto& out) const noexcept {
            if (columns.empty()) {
                out.push_back('*');
                return;
            }
            // todo: Watch out for SQL Injection here
            strings::join_with(out, columns, ", ");
        }

        constexpr void serialize_where(auto& out) const noexcept {
            if (where_clauses.empty()) {
                return; // we don't have any "WHERE clauses"
            }
            out.push_back(' ');
            out.append(keywords::where);
            out.push_back(' ');
            for (auto const& expr : where_clauses) {
                expr(out, db);
            }
        }

        constexpr void serialize_update(auto& out) const noexcept {
            if (values.empty()) {
                return;
            }
            assert(values.size() == columns.size());

            out.append(keywords::update);
            out.push_back(' ');
            serialize_single_from(out);
            out.push_back(' ');
            out.append(keywords::set);
            out.push_back(' ');
            auto       iter   = values.begin();
            auto const it_end = values.end();
            auto       cit    = columns.begin();
            for (;;) {
                db.quoted_escape(*cit, out);
                out.append(" = ");
                serialize_expression(out, *iter);
                ++iter;
                ++cit;
                if (iter == it_end) {
                    break;
                }
                out.append(", ");
            }
            serialize_where(out);
        }

        constexpr void serialize_insert(auto& out) const noexcept {
            // todo: replace into
            // todo: insert or fail, or ignore, or replace, ... into
            out.append(keywords::insert);
            out.push_back(' ');
            out.append(keywords::into);
            out.push_back(' ');
            serialize_single_from(out);
            out.push_back(' ');
            if (!columns.empty()) {
                out.push_back('(');
                auto const it_end = columns.end();
                auto       pos    = columns.begin();
                for (;;) {
                    db.quoted_escape(*pos, out);
                    ++pos;
                    if (pos == it_end) {
                        break;
                    }
                    out.append(", ");
                }
                out.push_back(')');
            }


            if (select_stmt.valid()) {
                // insert ... select
                // manual join (code duplication)
                select_stmt->to_string(out);
            } else {
                // join
                // example: (1, 2, 3), (1, 2, 3), ...
                auto       pos    = values.begin();
                auto const it_end = values.end();

                using diff_type     = typename expr_vec::iterator::difference_type;
                auto const col_size = static_cast<diff_type>(columns.size());

                out.append(keywords::values);
                out.append(" (");

                // manual join (code duplication)
                {
                    auto const it_step_first = pos + col_size - 1;
                    for (; pos != it_step_first; ++pos) {
                        serialize_expression(out, *pos);
                        out.append(", ");
                    }
                    serialize_expression(out, *pos);
                    ++pos;
                }

                out.push_back(')');

                // values and columns should be aligned so don't worry
                for (; pos != it_end;) {
                    out.append(", (");

                    // manual join
                    {
                        auto const it_step = pos + col_size - 1;
                        for (; pos != it_step; ++pos) {
                            serialize_expression(out, *pos);
                            out.append(", ");
                        }
                        serialize_expression(out, *pos);
                        ++pos;
                    }

                    out.push_back(')');
                }
            }
        }

        constexpr void serialize_remove(auto& out) const noexcept {
            if (from_cols.empty()) {
                db.logger.error(
                  LOG_CAT,
                  "Calling to_string on delete sql query requires you to specify the table name.");
                return;
            }
            out.append(keywords::delete_word);
            out.push_back(' ');
            out.append(keywords::from);
            out.push_back(' ');
            db.quoted_escape(from_cols.front(), out);
            serialize_where(out);
        }

        // template <SQLKeywords words>
        // static constexpr stl::array<string_view_type, 4> join_strings{{
        //   " "+ keywords::left + " " + keywords::join + " ",  // 1
        //   " "+ keywords::right + " " + keywords::join + " ", // 2
        //   " "+ keywords::full + " " + keywords::join + " ",  // 3
        //   " "+ keywords::cross + " " + keywords::join + " "  // 4
        // }};

        // template <SQLKeywords words>
        // static constexpr stl::array<string_view_type, 2> cond_strings{{
        //   string_view_type{" "} + keywords::using_word + " (", // 0
        //   string_view_type{" "} + keywords::on_word + " "      // 1
        // }};

        template <typename StrT>
        constexpr void serialize_joins(StrT& out) const noexcept {
            // todo: this is branch-less-able :)
            for (auto const& join : joins) {
                out.push_back(' ');
                switch (join.cat) {
                    case join_type::join_cat::inner: {
                        out.append(keywords::inner); // todo: do we need this?
                        out.push_back(' ');
                        out.append(keywords::join);
                        break;
                    }
                    case join_type::join_cat::left: {
                        out.append(keywords::left);
                        out.push_back(' ');
                        out.append(keywords::join);
                        break;
                    }
                    case join_type::join_cat::right: {
                        out.append(keywords::right);
                        out.push_back(' ');
                        out.append(keywords::join);
                        break;
                    }
                    case join_type::join_cat::full: {
                        out.append(keywords::full);
                        out.push_back(' ');
                        out.append(keywords::join);
                        break;
                    }
                    case join_type::join_cat::cross: {
                        out.append(keywords::cross);
                        out.push_back(' ');
                        out.append(keywords::join);
                        break;
                    }
                    default: stl::unreachable();
                }
                out.push_back(' ');
                if (auto* table_name = stl::get_if<string_type>(&join.table)) {
                    db.quoted_escape(*table_name, out);
                } else {
                    auto query = stl::get<subquery>(join.table);
                    query->template to_string<StrT>(out);
                }
                out.push_back(' ');
                switch (join.cond) {
                    case join_type::cond_type::none: {
                        break;
                    }
                    case join_type::cond_type::on_cond: {
                        out.append(keywords::on_word);
                        out.push_back(' ');
                        serialize_expression(out, join.expr);
                        break;
                    }
                    case join_type::cond_type::using_cond: {
                        // don't need to check the size, if something happened here, the problem is inside the
                        // left/right_join_using functions.
                        out.append(keywords::using_word);
                        out.append(" (");
                        // don't need to check if col_names are empty or not, they should be non-empty
                        auto       pos    = join.col_names.begin();
                        auto const it_end = join.col_names.end();
                        for (;;) {
                            db.quoted_escape(*pos, out);
                            ++pos;
                            if (pos == it_end) {
                                break;
                            }
                            out.append(", ");
                        }
                        out.push_back(')');
                        break;
                    }
                    default: stl::unreachable();
                }
            }
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
