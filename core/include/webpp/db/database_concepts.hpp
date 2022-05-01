// Created by moisrex on 7/3/21.

#ifndef WEBPP_DATABASE_CONCEPTS_HPP
#define WEBPP_DATABASE_CONCEPTS_HPP

#include "../std/concepts.hpp"

namespace webpp::sql {

    template <typename T>
    concept Connection = requires(T db) {
        requires stl::default_initializable<T>;

        { db.open() } -> stl::same_as<bool>;
        { db.is_open() } -> stl::same_as<bool>;
        { db.close() } -> stl::same_as<bool>;

        // Other specialized configuration for each database type is goes here as well.
        // For example the SQLite will require a file path
        // And MySQL will require connection settings.
    };

    template <typename T>
    concept SQLGrammer = requires {
        T::insert("", {});
    };


    /**
     * This type holds the main object of a SQL Database.
     * A database is bigger than SQL Database, which may even include services that
     * can store files; because I'm planning on making this project work with other
     * services as well (like for example GraphQL and even Cypher language), I separate
     * the Database and SQLDatabase concepts.
     *
     * A SQL Database may hold:
     *   - Connection
     *   - Query Language
     *
     * A Connection is how the system works, and
     * a Query Language is what the system does.
     */
    template <typename T>
    concept SQLDatabase = Connection<T> && requires {
        requires stl::default_initializable<T>;

        typename T::grammer_type;
        requires SQLGrammer<typename T::grammer_type>;
    };


} // namespace webpp::sql

#endif // WEBPP_DATABASE_CONCEPTS_HPP
