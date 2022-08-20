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

    template <typename CharT = char>
    struct sql_lowercase_keywords {
        static constexpr const CharT* select       = "select";
        static constexpr const CharT* update       = "update";
        static constexpr const CharT* delete_word  = "delete";
        static constexpr const CharT* values       = "values";
        static constexpr const CharT* from         = "from";
        static constexpr const CharT* where        = "where";
        static constexpr const CharT* in           = "in";
        static constexpr const CharT* null         = "null";
        static constexpr const CharT* not_word     = "not";
        static constexpr const CharT* and_word     = "and";
        static constexpr const CharT* or_word      = "or";
        static constexpr const CharT* insert_into  = "insert into";
        static constexpr const CharT* default_word = "default";
    };

    template <typename CharT = char>
    struct sql_uppercase_keywords {
        static constexpr const CharT* select       = "SELECT";
        static constexpr const CharT* update       = "UPDATE";
        static constexpr const CharT* delete_word  = "DELETE";
        static constexpr const CharT* values       = "VALUES";
        static constexpr const CharT* from         = "FROM";
        static constexpr const CharT* where        = "WHERE";
        static constexpr const CharT* in           = "IN";
        static constexpr const CharT* null         = "NULL";
        static constexpr const CharT* not_word     = "NOT";
        static constexpr const CharT* and_word     = "AND";
        static constexpr const CharT* or_word      = "OR";
        static constexpr const CharT* insert_into  = "INSERT INTO";
        static constexpr const CharT* default_word = "DEFAULT";
    };


    template <typename DBType>
    struct query_builder;


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
                template <istl::Stringifiable StrvT>
                constexpr query_builder_type& name(StrvT&& in_table_name) noexcept {
                    enclosing().table_name =
                      istl::stringify_of<string_type>(stl::forward<StrvT>(in_table_name),
                                                      alloc::allocator_for<string_type>(enclosing().db));
                    return enclosing();
                }

                template <istl::Stringifiable StrvT>
                constexpr query_builder_type& operator()(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

                template <istl::Stringifiable StrvT>
                constexpr query_builder_type& operator[](StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
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
            if constexpr (stl::integral<key_type>) {
                // key is index
                builder.values[key] = value;
            } else {
                builder.columns.push_back(key);
                builder.values.push_back(key);
            }
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
        // todo: check if it's a good idea to use local allocator here or not.
        // using query_builder_ptr = typename allocator_pack_type::template local_unique_ptr<query_builder>;
        using query_builder_ptr =
          istl::dynamic<query_builder, traits::local_allocator<traits_type, query_builder>>;

        // WHERE Clause type
        struct where_type {
            local_string_type op; // the operator string

            // we can use the local_string_type here because we know it's already localified
            local_string_type value;
        };

        using variable_type        = stl::variant<stl::monostate,
                                           db_float_type,
                                           db_integer_type,
                                           db_string_type,
                                           db_blob_type,
                                           query_builder_ptr>;
        using column_variable_pair = stl::pair<string_type, variable_type>;
        using vector_of_variables =
          stl::vector<variable_type, traits::local_allocator<traits_type, variable_type>>;
        using vector_of_strings =
          stl::vector<local_string_type, traits::local_allocator<traits_type, local_string_type>>;
        using vector_of_wheres = stl::vector<where_type, traits::local_allocator<traits_type, where_type>>;

        static_assert(stl::is_move_assignable_v<query_builder_ptr>, "Variable must be movable.");
        static_assert(stl::is_move_assignable_v<variable_type>, "Variable must be movable.");

        // create query is not included in the query builder class
        enum struct query_method { select, insert, insert_default, update, remove, none };
        enum struct order_by_type { asc, desc };


        database_ref        db;
        query_method        method = query_method::none;
        string_type         table_name;
        vector_of_strings   columns; // insert: col names, update: col names, select: cols, delete: unused
        vector_of_variables values;  // insert: values, update: values, select: unused, delete: unused
        vector_of_wheres    where_clauses;
        order_by_type       order_by_value;


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
            table_name{alloc::allocator_for<string_type>(db)},
            columns{alloc::local_allocator<local_string_type>(db)},
            values{alloc::local_allocator<variable_type>(db)},
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

        // todo: where, where_not, where_in, and_where, and_where_not_null, or_where, or_where_not_null


        template <istl::Stringifiable StrT>
        constexpr query_builder& where(StrT&& condition) noexcept {
            where_clauses.emplace("", condition);
            return *this;
        }

        template <istl::Stringifiable StrT1,
                  typename T,
                  SQLKeywords words = sql_lowercase_keywords<istl::char_type_of<StrT1>>>
        constexpr query_builder& where(StrT1&& col, T&& value) noexcept {
            auto clause = stringify(stl::forward<StrT1>(col));
            if constexpr (requires { stl::size(value); }) {
                clause.reserve(clause.size() + stl::size(value) + 3 + 2 + 1);
            }
            clause.append(" = ");
            stringify_value<words>(clause, variablify(stl::forward<T>(value)));

            // we add empty string as condition but to_string can identify if it needs to add "and" or ""
            where_clauses.push_back(where_type{.op = "", .value = stl::move(clause)});
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
            values.emplace_back(alloc::allocator_for<query_builder_ptr>(db), new_builder);
            return *this;
        }


        constexpr query_builder& insert(query_builder&& new_builder) noexcept {
            if (new_builder.method != query_method::select) {
                db.logger.error(LOG_CAT, "Only select queries are allowed");
                return *this;
            }

            method = query_method::insert;
            values.emplace_back(alloc::allocator_for<query_builder_ptr>(db), stl::move(new_builder));
            return *this;
        }



        constexpr query_builder&
        insert(stl::initializer_list<column_variable_pair> const& input_cols_vals) noexcept {
            return insert<stl::initializer_list<column_variable_pair>>(input_cols_vals);
        }


        // insert a single row
        template <istl::ReadOnlyCollection VecOfColVal = stl::initializer_list<column_variable_pair>>
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
                        const stl::size_t col_size = columns.size();
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
                    values.push_back(variablify(val));
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
                    if (values.empty()) {
                        break;
                    }
                    out.append(words::insert_into);
                    out.push_back(' ');
                    stringify_table_name(out);
                    out.push_back(' ');
                    if (!columns.empty()) {
                        out.push_back('(');
                        strings::join_with(out, columns,
                                           ", "); // todo: column names are not escaped
                        out.push_back(')');
                    }
                    out.append(words::values);

                    // join
                    // example: (1, 2, 3), (1, 2, 3), ...
                    auto       it     = values.begin();
                    const auto it_end = values.end();

                    using diff_type     = typename vector_of_variables::iterator::difference_type;
                    const auto col_size = static_cast<diff_type>(columns.size());
                    out.append(" (");

                    // manual join (code duplication)
                    {
                        auto const it_step_first = it + col_size - 1;
                        for (; it != it_step_first; ++it) {
                            stringify_value<words>(out, *it);
                            out.append(", ");
                        }
                        stringify_value<words>(out, *it);
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
                                stringify_value<words>(out, *it);
                                out.append(", ");
                            }
                            stringify_value<words>(out, *it);
                            ++it;
                        }

                        out.push_back(')');
                    }
                    break;
                }
                case query_method::select: {
                    out.append(words::select);
                    out.push_back(' ');
                    stringify_select_columns(out);
                    out.push_back(' ');
                    out.append(words::from);
                    out.push_back(' ');
                    stringify_table_name(out);
                    stringify_where<words>(out);
                    break;
                }
                case query_method::insert_default: {
                    out.append(words::insert_into);
                    out.push_back(' ');
                    stringify_table_name(out);
                    out.push_back(' ');
                    out.append(words::default_word);
                    out.push_back(' ');
                    out.append(words::values);
                    break;
                }
                case query_method::none: {
                    // the query is empty.
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
        constexpr decltype(auto) variablify(V&& val) const noexcept {
            if constexpr (stl::is_same_v<V, db_float_type> || stl::is_same_v<V, db_integer_type> ||
                          stl::is_same_v<V, db_string_type> || stl::is_same_v<V, db_blob_type> ||
                          stl::is_same_v<V, query_builder_ptr>) {
                return variable_type{stl::forward<V>(val)};
            } else if constexpr (istl::Stringifiable<V>) {
                return variable_type{stringify<V>(stl::forward<V>(val))};
            } else if constexpr (stl::floating_point<V>) {
                return variable_type{static_cast<db_float_type>(val)};
            } else if constexpr (stl::integral<V>) {
                return variable_type{static_cast<db_integer_type>(val)};
            } else if constexpr (stl::same_as<stl::remove_cvref_t<V>, variable_type>) {
                return stl::forward<V>(val);
            } else if constexpr (stl::same_as<stl::remove_cvref_t<V>, std::nullptr_t> ||
                                 stl::same_as<stl::remove_cvref_t<V>, stl::monostate>) {
                return variable_type{stl::monostate{}};
            } else {
                // todo
                static_assert_false(V, "The specified type is not a valid SQL Value.");
            }
        }

        /**
         * This function will stringify the values, if you're looking for the function that handles the
         * prepare statements, this is not going to be used there.
         */
        template <SQLKeywords words, typename StrT>
        constexpr void stringify_value(StrT& out, variable_type const& var) const noexcept {
            if (auto* f = stl::get_if<db_float_type>(&var)) {
                out.append(lexical::cast<string_type>(*f, db));
            } else if (auto* i = stl::get_if<db_integer_type>(&var)) {
                out.append(lexical::cast<string_type>(*i, db));
            } else if (auto* s = stl::get_if<db_string_type>(&var)) {
                out.append(*s);
            } else if (auto* qb = stl::get_if<query_builder_ptr>(&var)) {
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
                if (!*qb) {
                    out.append(words::null);
                } else {
                    (*qb)->template to_string<StrT, words>(out);
                }
            } else if (auto* b = stl::get_if<db_blob_type>(&var)) {
                // todo: append blob
            } else {
                // it's null
                out.append(words::null);
            }
        }

        constexpr void stringify_table_name(auto& out) const noexcept {
            // todo: MS SQL Server adds brackets
            out.append(table_name);
        }

        // select [... this method ...] from table;
        constexpr void stringify_select_columns(auto& out) const noexcept {
            strings::join_with(out, columns, ',');
        }

        template <SQLKeywords words>
        constexpr void stringify_where(auto& out) const noexcept {
            if (where_clauses.empty()) {
                return; // we don't have any "WHERE clauses"
            }
            out.push_back(' ');
            out.append(words::where);

            auto       it        = where_clauses.begin();
            auto const where_end = where_clauses.end();

            // specializing the first one for performance reasons
            {
                auto const& [op, value] = *it;
                // the first one can't include "or" and "and"
                if (op != words::and_word && op != words::or_word) {
                    out.push_back(' ');
                    out.append(op);
                }
                out.push_back(' ');
                out.append(value);
            }
            for (; it != where_end; ++it) {
                auto const& [op, value] = *it;
                out.push_back(' ');
                if (op.empty()) {
                    out.append(words::and_word);
                } else {
                    out.append(op);
                }
                out.push_back(' ');
                out.append(value);
            }
        }
    };



} // namespace webpp::sql


#endif // WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
