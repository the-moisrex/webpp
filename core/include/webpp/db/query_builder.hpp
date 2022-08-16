#ifndef WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
#define WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP

#include "../std/collection.hpp"
#include "../std/ranges.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/join.hpp"
#include "../traits/traits.hpp"
#include "sql_concepts.hpp"

namespace webpp::sql {

    template <typename CharT = char>
    struct sql_lowercase_keywords {
        static constexpr const CharT* select      = "select";
        static constexpr const CharT* update      = "update";
        static constexpr const CharT* delete_word = "delete";
        static constexpr const CharT* values      = "values";
        static constexpr const CharT* from        = "from";
        static constexpr const CharT* where       = "where";
        static constexpr const CharT* in          = "in";
        static constexpr const CharT* null        = "null";
        static constexpr const CharT* not_word    = "not";
        static constexpr const CharT* and_word    = "and";
        static constexpr const CharT* or_word     = "or";
        static constexpr const CharT* insert_into = "insert into";
    };

    template <typename CharT = char>
    struct sql_uppercase_keywords {
        static constexpr const CharT* select      = "SELECT";
        static constexpr const CharT* update      = "UPDATE";
        static constexpr const CharT* delete_word = "DELETE";
        static constexpr const CharT* values      = "VALUES";
        static constexpr const CharT* from        = "FROM";
        static constexpr const CharT* where       = "WHERE";
        static constexpr const CharT* in          = "IN";
        static constexpr const CharT* null        = "NULL";
        static constexpr const CharT* not_word    = "NOT";
        static constexpr const CharT* and_word    = "AND";
        static constexpr const CharT* or_word     = "OR";
        static constexpr const CharT* insert_into = "INSERT INTO";
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
                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& name(StrvT&& in_table_name) noexcept {
                    enclosing().table_name = istl::stringify_of<string_type>(
                      stl::forward<StrvT>(in_table_name),
                      alloc::allocator_for<string_type>(enclosing().db.alloc_pack));
                    return enclosing();
                }

                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& operator()(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& operator[](StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

                template <istl::StringViewifiable StrvT>
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
        using database_type     = DBType;
        using traits_type       = typename database_type::traits_type;
        using string_type       = traits::general_string<traits_type>;
        using local_string_type = traits::local_string<traits_type>;
        using database_ref      = stl::add_lvalue_reference_t<database_type>;
        using size_type         = typename database_type::size_type;
        using db_float_type     = typename database_type::float_type;
        using db_integer_type   = typename database_type::integer_type;
        using db_string_type    = typename database_type::string_type;
        using db_blob_type      = typename database_type::blob_type;

        using driver_type     = typename database_type::driver_type;
        using grammar_type    = typename database_type::grammar_type;
        using connection_type = typename database_type::connection_type;

        template <typename, typename>
        friend struct column_builder;

        template <typename ADBType>
        friend struct details::query_builder_subclasses;

      private:
        // WHERE Clause type
        struct where_type {
            enum where_op {
                NONE, // where
                AND,  // and_where
                OR,   // or_where
                IN    // where_in
            };

            where_op op = NONE;

            // we can use the local_string_type here because we know it's already localified
            local_string_type value;
        };

        using variable_type = stl::variant<db_float_type, db_integer_type, db_string_type, db_blob_type>;
        using vector_of_variables = traits::localify_allocators<traits_type, stl::vector<variable_type>>;
        using vector_of_strings   = traits::localify_allocators<traits_type, stl::vector<local_string_type>>;
        using vector_of_wheres    = traits::localify_allocators<traits_type, stl::vector<where_type>>;

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
            return istl::stringify_of<local_string_type>(stl::forward<T>(str));
        }

      public:
        template <EnabledTraits ET>
            requires(!stl::same_as<ET, query_builder>)
        constexpr query_builder(ET&& et, database_ref input_db) noexcept
          : db{input_db},
            table_name{alloc::allocator_for<string_type>(et)},
            columns{alloc::local_allocator<local_string_type>(et)},
            where_clauses{alloc::local_allocator<where_type>(et)} {}

        /**
         * Set columns to be selected in the sql query.
         */
        template <typename T>
        constexpr query_builder& select(T&& col) noexcept {
            if constexpr (istl::Stringifiable<T>) {
                col.push_back(stringify(stl::forward<T>(col)));
            } else if constexpr (istl::ReadOnlyCollection<T>) {
            }
            return *this;
        }

        template <istl::StringViewifiable StrvT>
        constexpr column_builder<database_type, StrvT> operator[](StrvT&& col_name) const noexcept {
            return {db, stl::forward<StrvT>(col_name)};
        }

        constexpr column_builder<database_type, stl::size_t>
        operator[](stl::size_t col_index) const noexcept {
            return {db, col_index};
        }

        // todo: where, where_not, where_in, and_where, and_where_not_null, or_where, or_where_not_null


        // insert into Col default values;
        constexpr query_builder& insert_default() noexcept {
            method = query_method::insert_default;
            return *this;
        }

        constexpr query_builder& insert() noexcept {
            method = query_method::insert;
            return *this;
        }

        // insert a single row
        template <istl::ReadOnlyCollection VecOfColVal = vector_of_variables>
        constexpr query_builder& insert(VecOfColVal&& input_cols_vals) noexcept {
            method = query_method::insert;
            // Steps:
            //   1. merge columns (we might have a new column in the cols_vals that we didn't know before)
            //   2. re-adjust the sizes of the values to match to the new column size; insert null values
            //   3. sort the values based on the columns

            stl::span cols_vals{input_cols_vals.begin(), input_cols_vals.end()};

            const auto        values_last = stl::prev(values.end());
            const stl::size_t values_size = values.size();

            // 1. finding if we have a new column:
            // 3. sorting cols_vals based on the columns
            auto col_it = columns.begin();
            for (auto it = cols_vals.begin();;) {
                auto const& [col, val] = *it;
                if (col != *col_it) {
                    auto next_it = stl::next(it);
                    if (next_it != cols_vals.end()) {
                        stl::iter_swap(it, next_it);
                    } else {
                        // found a new column
                        const stl::size_t col_size = columns.size();
                        columns.push_back(col);

                        // 2. Adding new and null variables into the values to adjust the values matrix
                        for (auto val_it = values.begin() + col_size; val_it != values_last;
                             val_it += col_size) {
                            values.emplace(val_it); // insert a null variable at that position
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
                    out.append(' ');
                    stringify_table_name(out);
                    out.append(' ');
                    if (!columns.empty()) {
                        out.append('(');
                        strings::join_with(out, columns, ", "); // todo: column names are not escaped
                        out.append(')');
                    }
                    out.append(words::values);

                    // join
                    // example: (1, 2, 3), (1, 2, 3), ...
                    auto              it       = values.begin();
                    const auto        it_end   = values.end();
                    const stl::size_t col_size = columns.size();
                    out.append(" (");
                    strings::join_with(out,
                                       stl::span{it, col_size} |
                                         stl::views::transform([this](auto& val) constexpr noexcept {
                                             return stringify_value<words>(val);
                                         }),
                                       ", ");
                    out.append(')');

                    // values and columns should be aligned so don't worry
                    for (; it != it_end; it += col_size) {
                        out.append(", (");
                        strings::join_with(out,
                                           stl::span{it, col_size} |
                                             stl::views::transform([this](auto& val) constexpr noexcept {
                                                 return stringify_value<words>(val);
                                             }),
                                           ", ");
                        out.append(')');
                    }
                    break;
                }
                case query_method::select: {
                    out.append(words::select);
                    out.append(' ');
                    stringify_select(out);
                    out.append(' ');
                    out.append(words::from);
                    out.append(' ');
                    stringify_table_name(out);
                    stringify_where<words>(out);
                    break;
                }
                case query_method::insert_default: {
                    out.append(words::insert_into);
                    out.append(' ');
                    stringify_table_name(out);
                    out.append(' ');
                    out.append(words::default_word);
                    out.append(' ');
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
        static constexpr variable_type variablify(V&& val) noexcept {
            if constexpr (stl::is_same_v<V, db_float_type> || stl::is_same_v<V, db_integer_type> ||
                          stl::is_same_v<V, db_string_type> || stl::is_same_v<V, db_blob_type>) {
                return stl::forward<V>(val);
            } else {
                // todo
            }
        }

        template <SQLKeywords words>
        constexpr void stringify_value(auto& out, variable_type const& var) const noexcept {
            if (db_float_type f = stl::get_if<db_float_type>(var)) {
                out.append(lexical::cast<string_type>(f, db));
            } else if (db_integer_type i = stl::get_if<db_integer_type>(var)) {
                out.append(lexical::cast<string_type>(i, db));
            } else if (db_string_type s = stl::get_if<db_string_type>(var)) {
                out.append(s);
            } else if (db_blob_type b = stl::get_if<db_blob_type>(var)) {
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
        constexpr void stringify_select(auto& out) const noexcept {
            strings::join_with(out, columns, ',');
        }

        template <SQLKeywords words>
        constexpr void stringify_where(auto& out) const noexcept {
            if (where_clauses.empty()) {
                return; // we don't have any "WHERE clauses"
            }
            out.append(' ');
            out.append(words::where);
            for (where_type& clause : where_clauses) {
                out.append(' ');
                switch (clause.op) {
                    case where_type::NONE: break;
                    case where_type::AND:
                        out.append(' ');
                        out.append(words::and_word);
                        out.append(' ');
                        break;
                    case where_type::OR:
                        out.append(' ');
                        out.append(words::or_word);
                        out.append(' ');
                        break;
                    case where_type::IN:
                        out.append(' ');
                        out.append(words::in);
                        out.append(' ');
                        // todo
                        continue;
                }
                out.append(clause.value);
            }
        }
    };



} // namespace webpp::sql


#endif // WEBPP_DATABASE_SQL_QUERY_BUILDER_HPP
