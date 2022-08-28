#ifndef WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
#define WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP

#include "../std/collection.hpp"
#include "../std/ranges.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/join.hpp"
#include "../traits/traits.hpp"
#include "sql_concepts.hpp"

#include <variant>

namespace webpp::sql {


    template <typename DBType>
    struct query_builder;

    template <typename DBType>
    struct expression_interface {
        using database_type     = DBType;
        using traits_type       = typename database_type::traits_type;
        using local_string_type = traits::local_string<traits_type>;

        constexpr virtual void to_string(local_string_type& out) const noexcept = 0;
        constexpr virtual ~expression_interface()                               = default;
    };

    namespace details {
#define define_expression(name, ...)                                                                                   \
    template <typename DBType>                                                                                         \
    struct name : expression_interface<DBType> {                                                                       \
        using database_type       = DBType;                                                                            \
        using traits_type         = typename database_type::traits_type;                                               \
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;                                          \
        using string_type         = traits::general_string<traits_type>;                                               \
        using string_view_type    = traits::string_view<traits_type>;                                                  \
        using local_string_type   = traits::local_string<traits_type>;                                                 \
        using database_ref        = stl::add_lvalue_reference_t<database_type>;                                        \
        using size_type           = typename database_type::size_type;                                                 \
        using db_float_type       = typename database_type::float_type;                                                \
        using db_integer_type     = typename database_type::integer_type;                                              \
        using db_string_type      = typename database_type::string_type;                                               \
        using db_blob_type        = typename database_type::blob_type;                                                 \
        using keywords            = typename database_type::keywords;                                                  \
        using expr_type           = istl::dynamic<expression_interface<DBType>,                                        \
                                        traits::local_allocator<traits_type, expression_interface<DBType>>>; \
                                                                                                                       \
        using driver_type     = typename database_type::driver_type;                                                   \
        using grammar_type    = typename database_type::grammar_type;                                                  \
        using connection_type = typename database_type::connection_type;                                               \
                                                                                                                       \
        __VA_ARGS__                                                                                                    \
                                                                                                                       \
        constexpr void to_string(local_string_type& out, database_ref db) const noexcept override;                     \
    };                                                                                                                 \
    template <typename DBType>                                                                                         \
    constexpr void name<DBType>::to_string(typename name<DBType>::local_string_type& out,                              \
                                           typename name<DBType>::database_ref       db) const noexcept


        // literal value
        define_expression(floating_expr, db_float_type val;) {
            out.append(lexical::cast<local_string_type>(val, db));
        }

        define_expression(integer_expr, db_integer_type val;) {
            out.append(lexical::cast<local_string_type>(val, db));
        }

        define_expression(bool_expr, bool val;) {
            out.append(val ? keywords::true_word : keywords::false_word);
        }

        define_expression(null_expr) {
            out.append(keywords::null);
        }

        define_expression(col_name_expr, local_string_type schema_name{}, table_name{}, column_name;) {
            db.quoted_escape(schema_name, out);
            db.quoted_escape(table_name, out);
            db.quoted_escape(column_name, out);
        }

        define_expression(unary_op_expr, enum struct unaries
                          : stl::uint_fast8_t{plus, minus, incr, decr, negate} op;
                          expr_type expr;) {
            constexpr stl::string_view op_strs[]{" + ", " - ", " ++", " --", " !"};
            out.append(op_strs[static_cast<stl::uint_fast8_t>(op)]);
            expr.to_string(out, db);
        }

        define_expression(binary_op_expr, enum struct binaries
                          : stl::uint_fast8_t{add, sub, mul, div} op;
                          expr_type left_operand, right_operand;) {
            constexpr stl::string_view op_strs[]{" + ", " - ", " *  ", " / "};
            left_operand.to_string(out, db);
            out.append(op_strs[static_cast<stl::uint_fast8_t>(op)]);
            right_operand.to_string(out, db);
        }

#undef define_expression
    } // namespace details

    namespace details {
        template <typename DBType>
        struct query_builder_subclasses {
            using database_type      = DBType;
            using traits_type        = typename database_type::traits_type;
            using string_type        = traits::general_string<traits_type>;
            using query_builder_type = query_builder<database_type>;

            // get query_builder reference
#define define_enclosing(obj)                                                                   \
    constexpr inline query_builder_type& enclosing() noexcept {                                 \
        return *reinterpret_cast<query_builder_type*>(reinterpret_cast<char*>(this) -           \
                                                      offsetof(query_builder_subclasses, obj)); \
    }

            [[no_unique_address]] struct table_type {

                define_enclosing(table);

                // set the name
                template <istl::Stringifiable... StrvT>
                constexpr query_builder_type& name(StrvT&&... in_table_name) noexcept {
                    enclosing().from_cols.clear();
                    (enclosing().from_cols.push_back(
                       istl::stringify_of<string_type>(stl::forward<StrvT>(in_table_name),
                                                       alloc::allocator_for<string_type>(enclosing().db))),
                     ...);
                    return enclosing();
                }

                template <istl::Stringifiable... StrvT>
                constexpr query_builder_type& operator()(StrvT&&... in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name)...);
                }

                template <istl::Stringifiable... StrvT>
                constexpr query_builder_type& operator[](StrvT&&... in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name)...);
                }

                template <istl::Stringifiable StrvT>
                constexpr query_builder_type& operator=(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

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
        using database_type      = DBType;
        using query_builder_type = query_builder<database_type>;
        using query_builder_ref  = stl::add_lvalue_reference_t<query_builder_type>;
        using key_type           = KeyType;
        using string_type        = typename query_builder_type::string_type;

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
            builder.values.emplace_back(builder.expressionify(stl::forward<T>(value)));
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
    template <typename DBType>
    struct query_builder : public details::query_builder_subclasses<DBType> {
        using database_type       = DBType;
        using traits_type         = typename database_type::traits_type;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using string_type         = traits::general_string<traits_type>;
        using string_view_type    = traits::string_view<traits_type>;
        using local_string_type   = traits::local_string<traits_type>;
        using database_ref        = stl::add_lvalue_reference_t<database_type>;
        using size_type           = typename database_type::size_type;
        using db_float_type       = typename database_type::float_type;
        using db_integer_type     = typename database_type::integer_type;
        using db_string_type      = typename database_type::string_type;
        using db_blob_type        = typename database_type::blob_type;

        using driver_type     = typename database_type::driver_type;
        using grammar_type    = typename database_type::grammar_type;
        using connection_type = typename database_type::connection_type;


        static constexpr auto LOG_CAT = "SQLBuilder";

        template <typename, typename>
        friend struct column_builder;

        template <typename ADBType>
        friend struct details::query_builder_subclasses;

      private:
        using subquery = istl::dynamic<query_builder, traits::local_allocator<traits_type, query_builder>>;

        using expression = expression_interface<database_type>;
        using col_expr_pair =
          stl::pair<string_type, expression>; // todo: anywhere that this being used should be re-checked
        using expression_vec = stl::vector<expression, traits::local_allocator<traits_type, expression>>;
        using string_vec =
          stl::vector<local_string_type, traits::local_allocator<traits_type, local_string_type>>;

        // https://www.sqlite.org/syntax/table-or-subquery.html
        using table_or_subquery_type = stl::variant<local_string_type, subquery>;


        // create query is not included in the query builder class
        enum struct query_method { select, insert, insert_default, update, remove, none };
        enum struct order_by_type { asc, desc };


        struct join_type {
            enum struct join_cat : stl::uint_fast8_t { inner = 0, left = 1, right = 2, full = 3, cross = 4 };
            enum struct cond_type : stl::uint_fast8_t { using_cond, on_cond, none };

            join_cat  cat;
            cond_type cond = cond_type::none;

            // https://www.sqlite.org/syntax/join-clause.html
            // https://www.sqlite.org/syntax/table-or-subquery.html
            table_or_subquery_type table;     // table or sub-query
            expression             expr;      // for "on"
            string_vec             col_names; // for using
        };
        using join_vec = stl::vector<join_type, traits::local_allocator<traits_type, join_type>>;

        database_ref   db;
        query_method   method = query_method::none;
        string_vec     from_cols;
        string_vec     columns; // insert: col names, update: col names, select: cols, delete: unused
        expression_vec values;  // insert: values, update: values, select: unused, delete: unused
        expression     where_clauses;
        join_vec       joins;
        order_by_type  order_by_value;



        template <typename T>
        static constexpr bool is_stringify = istl::StringifiableOf<local_string_type, T>;

        // helper to convert the input to acceptable string type
        template <typename T>
            requires(is_stringify<T>)
        constexpr auto stringify(T&& str) const noexcept {
            return istl::stringify_of<local_string_type>(stl::forward<T>(str),
                                                         alloc::allocator_for<local_string_type>(db));
        }


      public:
        constexpr query_builder(database_ref input_db) noexcept
          : db{input_db},
            from_cols{alloc::local_allocator<local_string_type>(db)},
            columns{alloc::local_allocator<local_string_type>(db)},
            values{alloc::local_allocator<expression>(db)},
            where_clauses{alloc::local_allocator<where_type>(db)} {}

        constexpr query_builder(query_builder&&) noexcept      = default;
        constexpr query_builder(query_builder const&) noexcept = default;

        constexpr query_builder& operator=(query_builder&&) noexcept      = default;
        constexpr query_builder& operator=(query_builder const&) noexcept = default;

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
         * @param select_query
         * @return
         */
        template <typename Expr1>
        constexpr query_builder& where_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            where_clauses.clear();
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::none,
                         .op    = where_type::op_type::in,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify(select_query),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            where_clauses.clear();
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::not_jt,
                         .op    = where_type::op_type::in,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify(select_query),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& and_where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::and_not_jt,
                         .op    = where_type::op_type::in,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify(select_query),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& or_where_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::or_jt,
                         .op    = where_type::op_type::in,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify(select_query),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }

        template <typename Expr1>
        constexpr query_builder& or_where_not_in(Expr1&& expr1, query_builder const& select_query) noexcept {
            if (select_query.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed inside where_in");
                return *this;
            }
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::or_not_jt,
                         .op    = where_type::op_type::in,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify(select_query),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }

        /**
         * Support for:
         *   select * from table where expr1 in (expr2, exprs...);
         */
        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& where_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_clauses.clear();
            where_type clause{.jt    = where_type::join_type::none,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }

        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& where_not_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_clauses.clear();
            where_type clause{.jt    = where_type::join_type::not_jt,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }

        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& and_where_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_type clause{.jt    = where_type::join_type::and_jt,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }

        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& or_where_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_type clause{.jt    = where_type::join_type::or_jt,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }

        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& or_where_not_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_type clause{.jt    = where_type::join_type::or_not_jt,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }

        template <typename Expr1, typename Expr2, typename... Exprs>
        constexpr query_builder& and_where_not_in(Expr1&& expr1, Expr2&& expr2, Exprs&&... exprs) noexcept {
            where_type clause{.jt    = where_type::join_type::and_not_jt,
                              .op    = where_type::op_type::in,
                              .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                              .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                              .exprs = expression_vec{alloc::local_allocator<expression>(db)}};
            clause.exprs.reserve(sizeof...(exprs));
            (clause.exprs.push_back(expressionify<Exprs>(stl::forward<Exprs>(exprs))), ...);
            where_clauses.push_back(clause);
            return *this;
        }


        template <typename Expr1, typename Expr2>
        constexpr query_builder& where(Expr1&& expr1, Expr2&& expr2) noexcept {
            where_clauses.clear();
            where_clauses.push_back(
              where_type{.jt    = where_type::join_type::none,
                         .op    = where_type::op_type::eq,
                         .expr1 = expressionify<Expr1>(stl::forward<Expr1>(expr1)),
                         .expr2 = expressionify<Expr2>(stl::forward<Expr2>(expr2)),
                         .exprs = expression_vec{alloc::local_allocator<expression>(db)}});
            return *this;
        }


        template <typename ColT, typename Expr>
            requires(istl::StringifiableOf<string_type, ColT>)
        constexpr query_builder& left_join_using(ColT&& col_string, Expr&& using_expr) noexcept {
            joins.push_back(join_type{
              .cat   = join_type::join_cat::left,
              .cond  = join_type::cond_type::using_cond,
              .table = istl::stringify_of<local_string_type>(stl::forward<ColT>(col_string),
                                                             alloc::allocator_for<local_string_type>(db)),
              .expr1 = expressionify(stl::forward<Expr>(using_expr))});
            return *this;
        }

        template <typename Expr>
        constexpr query_builder& left_join_using(query_builder const& sub_query, Expr&& using_expr) noexcept {
            joins.push_back(join_type{.cat   = join_type::join_cat::left,
                                      .cond  = join_type::cond_type::using_cond,
                                      .table = sub_query,
                                      .expr1 = expressionify(stl::forward<Expr>(using_expr))});
            return *this;
        }

        template <typename ColT, typename Expr>
            requires(istl::StringifiableOf<string_type, ColT>)
        constexpr query_builder& right_join_using(ColT&& col_string, Expr&& using_expr) noexcept {
            joins.push_back(join_type{
              .cat   = join_type::join_cat::right,
              .cond  = join_type::cond_type::using_cond,
              .table = istl::stringify_of<local_string_type>(stl::forward<ColT>(col_string),
                                                             alloc::allocator_for<local_string_type>(db)),
              .expr1 = expressionify(stl::forward<Expr>(using_expr))});
            return *this;
        }

        template <typename Expr>
        constexpr query_builder& right_join_using(query_builder const& sub_query,
                                                  Expr&&               using_expr) noexcept {
            joins.push_back(join_type{.cat   = join_type::join_cat::right,
                                      .cond  = join_type::cond_type::using_cond,
                                      .table = sub_query,
                                      .expr1 = expressionify(stl::forward<Expr>(using_expr))});
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

            method = query_method::insert;
            values.emplace_back(subquery{alloc::local_allocator<query_builder>(db), new_builder});
            return *this;
        }


        constexpr query_builder& insert(query_builder&& new_builder) noexcept {
            if (new_builder.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed");
                return *this;
            }

            method = query_method::insert;
            values.emplace_back(subquery{alloc::local_allocator<query_builder>(db), stl::move(new_builder)});
            return *this;
        }



        constexpr query_builder&
        insert(stl::initializer_list<col_expr_pair> const& input_cols_vals) noexcept {
            return insert<stl::initializer_list<col_expr_pair>>(input_cols_vals);
        }


        // insert a single row
        template <istl::ReadOnlyCollection VecOfColVal = stl::initializer_list<col_expr_pair>>
        constexpr query_builder& insert(VecOfColVal&& input_cols_vals) noexcept {
            method = query_method::insert;
            // Steps:
            //   1. merge columns (we might have a new column in the cols_vals that we didn't know before)
            //   2. re-adjust the sizes of the values to match to the new column size; insert null values
            //   3. sort the values based on the columns

            stl::span cols_vals{input_cols_vals.begin(), input_cols_vals.end()};

            const auto values_last = stl::prev(values.end());

            // 1. finding if we have a new column:
            // 3. sorting cols_vals based on the columns
            auto col_it = columns.begin();
            auto it     = cols_vals.begin();
            for (;;) {
                auto const& [col, val] = *it;
                if (col != *col_it) {
                    auto next_it = stl::next(it);
                    if (next_it != cols_vals.end()) {
                        stl::swap(it, next_it);
                    } else {
                        // found a new column
                        using diff_type     = typename string_vec::iterator::difference_type;
                        const auto col_size = static_cast<diff_type>(columns.size());
                        columns.push_back(col);

                        // 2. Adding new and null variables into the values to adjust the values matrix
                        for (auto val_it = values.begin() + col_size; val_it != values_last;
                             val_it += col_size) {
                            // insert a null variable at that position
                            values.emplace(val_it, stl::monostate{});
                        }
                    }
                } else {
                    // found the column
                    // now "it" and "col_it" are in the right order
                    values.push_back(expressionify(val));
                    ++col_it;
                    ++it;
                    if (it == cols_vals.end()) {
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
         * @tparam words Whether or not sql keywords are in lowercase or uppercase
         * @param out output
         */
        template <typename StrT     = string_type,
                  SQLKeywords words = sql_lowercase_keywords<istl::char_type_of<StrT>>>
        constexpr void to_string(StrT& out) const noexcept {
            switch (method) {
                case query_method::insert: {
                    serialize_insert<words>(out);
                    break;
                }
                case query_method::select: {
                    out.append(words::select);
                    out.push_back(' ');
                    serialize_select_columns(out);
                    out.push_back(' ');
                    out.append(words::from);
                    out.push_back(' ');
                    serialize_from(out);
                    serialize_joins<words>(out);
                    serialize_where<words>(out);
                    break;
                }
                case query_method::insert_default: {
                    out.append(words::insert_into);
                    out.push_back(' ');
                    serialize_from(out);
                    out.push_back(' ');
                    out.append(words::default_word);
                    out.push_back(' ');
                    out.append(words::values);
                    break;
                }
                case query_method::update: {
                    serialize_update<words>(out);
                    break;
                }
                case query_method::remove: {
                    serialize_remove<words>(out);
                    break;
                }
                case query_method::none: {
                    // the query is empty.
                    db.logger.warning(
                      LOG_CAT,
                      "Calling to_string on a query builder while you haven't defined the query type has no effect. Did you forget calling one of 'remove', 'update', or 'select' member functions?");
                }
            }
        }

        template <typename StrT     = string_type,
                  SQLKeywords words = sql_lowercase_keywords<istl::char_type_of<StrT>>>
        constexpr StrT to_string() const noexcept {
            auto out = object::make<StrT>(db);
            to_string<StrT, words>(out);
            return out;
        }

      private:
        template <typename V>
        constexpr decltype(auto) expressionify(V&& val) const noexcept {
            if constexpr (stl::is_same_v<V, db_float_type> || stl::is_same_v<V, db_integer_type> ||
                          stl::is_same_v<V, db_string_type> || stl::is_same_v<V, db_blob_type> ||
                          stl::is_same_v<V, subquery>) {
                return expression{stl::forward<V>(val)};
            } else if constexpr (istl::Stringifiable<V>) {
                return expression{stringify<V>(stl::forward<V>(val))};
            } else if constexpr (stl::floating_point<V>) {
                return expression{static_cast<db_float_type>(val)};
            } else if constexpr (stl::integral<V>) {
                return expression{static_cast<db_integer_type>(val)};
            } else if constexpr (stl::same_as<stl::remove_cvref_t<V>, expression>) {
                return stl::forward<V>(val);
            } else if constexpr (stl::same_as<stl::remove_cvref_t<V>, std::nullptr_t> ||
                                 stl::same_as<stl::remove_cvref_t<V>, stl::monostate>) {
                return expression{stl::monostate{}};
            } else {
                // todo
                static_assert_false(V, "The specified type is not a valid SQL Value.");
            }
        }

        /**
         * This function will stringify the values, if you're looking for the function that handles the
         * prepare statements, this is not going to be used there.
         *
         * https://www.sqlite.org/syntax/expr.html
         */
        template <SQLKeywords words, typename StrT>
        constexpr void serialize_expression(StrT& out, expression const& var) const noexcept {
            if (auto* f = stl::get_if<db_float_type>(&var)) {
                out.append(lexical::cast<string_type>(*f, db));
            } else if (auto* i = stl::get_if<db_integer_type>(&var)) {
                out.append(lexical::cast<string_type>(*i, db));
            } else if (auto* s = stl::get_if<db_string_type>(&var)) {
                db.quoted_escape(*s, out);
            } else if (auto* qb = stl::get_if<subquery>(&var)) {
                // good, we don't need to worry about the prepared query builder
                // todo: Fix user's mistakes and tune the select columns to match the insert columns

                // I'm going to modify the query builder in order to tune it to the insert sql query
                //   1. N insert cols == N select cols
                //     1.1. remove the useless select cols
                //     1.2. copy insert cols into the missing select cols
                //   2. Same column names in both cols
                //     2.1. ignore if they're the same
                //     2.2. check if it has an alias
                //     2.3. if it has, and it's the same as the insert col, then ignore
                //     2.4. if it doesn't have, then add an alias that is the same as the insert col
                (*qb)->template to_string<StrT, words>(out);
            } else if (auto* b = stl::get_if<db_blob_type>(&var)) {
                // todo: append blob
                out.append("Blob support is a todo");
            } else {
                // it's null
                out.append(words::null);
            }
        }

        constexpr void serialize_from(auto& out) const noexcept {
            auto       it       = from_cols.begin();
            const auto from_end = from_cols.end();
            if (it == from_end) {
                db.logger.error(
                  LOG_CAT,
                  "You requested a sql query but you didn't provide which table we should put into the sql query; did you miss the table name?");
                ;
                return;
            }
            db.quoted_escape(*it, out);
            ++it;
            while (it != from_end) {
                out.append(", ");
                db.quoted_escape(*it, out);
            }
        }

        constexpr void serialize_single_from(auto& out) const noexcept {
            if (from_cols.empty()) {
                db.logger.error(
                  LOG_CAT,
                  "You requested a sql query but you didn't provide which table we should put into the sql query; did you miss the table name?");
                ;
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

        template <SQLKeywords words>
        constexpr void serialize_where(auto& out) const noexcept {
            if (where_clauses.empty()) {
                return; // we don't have any "WHERE clauses"
            }
            out.push_back(' ');
            out.append(words::where);

            auto       it        = where_clauses.begin();
            auto const where_end = where_clauses.end();

            // specializing the first one for performance reasons
            for (;;) {
                out.push_back(' ');
                // todo: optimize this using jump tables
                switch (it->jt) {
                    case where_type::join_type::none: break;
                    case where_type::join_type::not_jt: {
                        out.append(words::not_word);
                        out.push_back(' ');
                        break;
                    }
                    case where_type::join_type::and_jt: {
                        out.append(words::and_word);
                        out.push_back(' ');
                        break;
                    }
                    case where_type::join_type::or_jt: {
                        out.append(words::or_word);
                        out.push_back(' ');
                        break;
                    }
                    case where_type::join_type::and_not_jt: {
                        out.append(words::and_word);
                        out.push_back(' ');
                        out.append(words::not_word);
                        out.push_back(' ');
                        break;
                    }
                    case where_type::join_type::or_not_jt: {
                        out.append(words::or_word);
                        out.push_back(' ');
                        out.append(words::not_word);
                        out.push_back(' ');
                        break;
                    }
                }

                switch (it->op) {
                    case where_type::op_type::eq: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" = ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::noteq: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" != ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::gt: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" > ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::lt: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" < ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::ge: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" >= ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::le: {
                        serialize_expression<words>(out, it->expr1);
                        out.append(" <= ");
                        serialize_expression<words>(out, it->expr2);
                        break;
                    }
                    case where_type::op_type::in: {
                        serialize_expression<words>(out, it->expr1);
                        out.push_back(' ');
                        out.append(words::in);
                        out.push_back(' ');
                        out.push_back('(');
                        serialize_expression<words>(out, it->expr2);
                        for (auto const& expr : it->exprs) {
                            out.append(", ");
                            serialize_expression<words>(out, expr);
                        }
                        out.push_back(')');
                        break;
                    }
                    case where_type::op_type::exists: {
                        serialize_expression<words>(out, it->expr1);
                        out.push_back(' ');
                        out.append(words::exists);
                        out.push_back(' ');
                        out.push_back('(');
                        serialize_expression<words>(out, it->expr2);
                        out.push_back(')');
                        break;
                    }
                    case where_type::op_type::between: {
                        // todo
                        break;
                    }
                    case where_type::op_type::like: {
                        // todo
                        break;
                    }
                    case where_type::op_type::none: break;
                    default: break;
                }

                ++it;
                if (it == where_end) {
                    break;
                }
            }
        }

        template <SQLKeywords words>
        constexpr void serialize_update(auto& out) const noexcept {
            if (values.empty()) {
                return;
            }
            assert(values.size() == columns.size());

            out.append(words::update);
            out.push_back(' ');
            serialize_single_from(out);
            out.push_back(' ');
            out.append(words::set);
            out.push_back(' ');
            auto       it     = values.begin();
            auto const it_end = values.end();
            auto       cit    = columns.begin();
            for (;;) {
                db.quoted_escape(*cit, out);
                out.append(" = ");
                serialize_expression<words>(out, *it);
                ++it;
                ++cit;
                if (it == it_end) {
                    break;
                }
                out.append(", ");
            }
            serialize_where<words>(out);
        }

        template <SQLKeywords words>
        constexpr void serialize_insert(auto& out) const noexcept {
            if (values.empty()) {
                return;
            }
            out.append(words::insert_into);
            out.push_back(' ');
            serialize_single_from(out);
            out.push_back(' ');
            if (!columns.empty()) {
                out.push_back('(');
                auto const it_end = columns.end();
                auto       it     = columns.begin();
                for (;;) {
                    db.quoted_escape(*it, out);
                    ++it;
                    if (it == it_end) {
                        break;
                    }
                    out.append(", ");
                }
                out.push_back(')');
            }

            // join
            // example: (1, 2, 3), (1, 2, 3), ...
            auto       it     = values.begin();
            const auto it_end = values.end();

            using diff_type     = typename expression_vec::iterator::difference_type;
            const auto col_size = static_cast<diff_type>(columns.size());

            if (values.size() == 1 && stl::holds_alternative<subquery>(*it)) {
                // insert ... select
                // manual join (code duplication)
                serialize_expression<words>(out, *it);
            } else {
                out.append(words::values);
                out.append(" (");

                // manual join (code duplication)
                {
                    auto const it_step_first = it + col_size - 1;
                    for (; it != it_step_first; ++it) {
                        serialize_expression<words>(out, *it);
                        out.append(", ");
                    }
                    serialize_expression<words>(out, *it);
                    ++it;
                }

                out.push_back(')');

                // values and columns should be aligned so don't worry
                for (; it != it_end;) {
                    out.append(", (");

                    // manual join
                    {
                        auto const it_step = it + col_size - 1;
                        for (; it != it_step; ++it) {
                            serialize_expression<words>(out, *it);
                            out.append(", ");
                        }
                        serialize_expression<words>(out, *it);
                        ++it;
                    }

                    out.push_back(')');
                }
            }
        }


        template <SQLKeywords words>
        constexpr void serialize_remove(auto& out) const noexcept {
            if (from_cols.empty()) {
                db.logger.error(
                  LOG_CAT,
                  "Calling to_string on delete sql query requires you to specify the table name.");
                return;
            }
            out.append(words::delete_word);
            out.push_back(' ');
            out.append(words::from);
            out.push_back(' ');
            db.quoted_escape(from_cols.front(), out);
            serialize_where<words>(out);
        }



      private:
        // template <SQLKeywords words>
        // static constexpr string_view_type join_strings[]{{
        //   " "+ words::left + " " + words::join + " ",  // 1
        //   " "+ words::right + " " + words::join + " ", // 2
        //   " "+ words::full + " " + words::join + " ",  // 3
        //   " "+ words::cross + " " + words::join + " "  // 4
        // }};

        // template <SQLKeywords words>
        // static constexpr string_view_type cond_strings[]{{
        //   string_view_type{" "} + words::using_word + " (", // 0
        //   string_view_type{" "} + words::on_word + " "      // 1
        // }};

      public:
        template <SQLKeywords words, typename StrT>
        constexpr void serialize_joins(StrT& out) const noexcept {

            // todo: this is branch-less-able :)
            for (auto const& join : joins) {
                out.push_back(' ');
                switch (join.cat) {
                    case join_type::join_cat::inner: {
                        out.append(words::inner); // todo: do we need this?
                        out.push_back(' ');
                        out.append(words::join);
                        break;
                    }
                    case join_type::join_cat::left: {
                        out.append(words::left);
                        out.push_back(' ');
                        out.append(words::join);
                        break;
                    }
                    case join_type::join_cat::right: {
                        out.append(words::right);
                        out.push_back(' ');
                        out.append(words::join);
                        break;
                    }
                    case join_type::join_cat::full: {
                        out.append(words::full);
                        out.push_back(' ');
                        out.append(words::join);
                        break;
                    }
                    case join_type::join_cat::cross: {
                        out.append(words::cross);
                        out.push_back(' ');
                        out.append(words::join);
                        break;
                    }
                }
                out.push_back(' ');
                if (auto* table_name = stl::get_if<local_string_type>(&join.table)) {
                    db.quoted_escape(*table_name, out);
                } else {
                    auto query = stl::get<subquery>(join.table);
                    query->template to_string<StrT, words>(out);
                }
                out.push_back(' ');
                switch (join.cond) {
                    case join_type::cond_type::none: {
                        break;
                    }
                    case join_type::cond_type::on_cond: {
                        out.append(words::on_word);
                        out.push_back(' ');
                        serialize_expression<words>(out, join.expr1);
                        out.append(" = ");
                        serialize_expression<words>(out, join.expr2);
                        break;
                    }
                    case join_type::cond_type::using_cond: {
                        out.append(words::using_word);
                        out.append(" (");
                        serialize_expression<words>(out, join.expr1);
                        out.push_back(')');
                        break;
                    }
                }
            }
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
