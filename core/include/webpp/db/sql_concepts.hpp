// Created by moisrex on 7/3/21.

#ifndef WEBPP_DATABASE_CONCEPTS_HPP
#define WEBPP_DATABASE_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::sql {


    /**
     * This concept shows what a SQL Statement is and it's used for preparing a query
     * This statement is what a connection is going to use internally; what the user should be exposed to,
     * is the statement wrapper and not this statement. All the cool and common features should be implemented
     * in the wrapper statement.
     */
    template <typename T>
    concept SQLStatement = requires(T stmt, stl::string& errmsg, stl::string& name) {
        typename T::size_type;
        stl::integral<typename T::size_type>;

        stmt.bind(1, stl::string_view{}, errmsg); // index based set string type
        stmt.bind(1, 1, errmsg);                  // index based set integer type
        stmt.column_name(1, name);
        stmt.column(1);
        { stmt.column_count() } -> stl::integral;
        stmt.step();
        stmt.is_column_null(1);
    };


    /**
     * This concept represents a Connection to a SQL based Database.
     * I specificly chose SQLConnection because a connection to a database could be
     * anything. Some of the features for a SQL Connection is only valid for SQL based
     * databases and not other types of databases like NoSQL or Graph based databases.
     */
    template <typename T>
    concept SQLConnection = requires(T db) {
        typename T::statement_type;
        requires SQLStatement<typename T::statement_type>;

        requires stl::default_initializable<T>;

        { db.open() } -> stl::same_as<bool>;
        { db.is_open() } -> stl::same_as<bool>;
        { db.close() } -> stl::same_as<bool>;
        { db.version() } -> istl::String; // todo: change this to have a "version" struct
        db.exec("");
        db.beign_transaction();
        db.rollback(); // rollback a database transaction
        db.commit();   // commit changes
        {
            db.prepare(requires_arg_cvref(istl::StringViewifiable))
            } -> stl::same_as<typename T::statement_type>;
        { db.query(requires_arg_cvref(istl::StringViewifiable)) } -> stl::same_as<typename T::result_type>;

        // todo: return result is not checked here:
        db.last_insert_id();
        db.last_insert_id(requires_arg_cvref(istl::StringViewifiable));


        // Other specialized configuration for each database type is goes here as well.
        // For example the SQLite will require a file path
        // And MySQL will require connection settings.
    };

    template <typename T>
    concept SQLGrammar = requires {
        T::insert("", {});
    };


    /**
     * This type holds the main object of a SQL Database.
     * A database is bigger than SQL Database, which may even include services that
     * can store files; because I'm planning on making this project work with other
     * services as well (like for example GraphQL and even Cypher language), I separate
     * the Database and SQLDriver concepts.
     *
     * A SQL Database may hold:
     *   - SQLConnection
     *   - Query Language
     *
     * A SQLConnection is how the system works, and
     * a Query Language is what the system does.
     */
    template <typename T>
    concept SQLDriver = SQLConnection<T> && requires {
        requires stl::default_initializable<T>;
        { T::template supports_string_view<std::string_view> } -> stl::same_as<bool>;

        typename T::grammar_type;
        requires SQLGrammar<typename T::grammar_type>;
    };


} // namespace webpp::sql

#endif // WEBPP_DATABASE_CONCEPTS_HPP
