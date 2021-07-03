// Created by moisrex on 7/3/21.

#ifndef WEBPP_DATABASE_CONCEPTS_HPP
#define WEBPP_DATABASE_CONCEPTS_HPP

/**
 * This file is the place to print all the database related C++20 concepts
 */
namespace webpp::database {

    /**
     * Example usages that I hope to implement:
     *
     * @code
     *   auto user = db["users"].where("id", 20).first();
     *   user["age"]++;                            // operator is overloaded, sync is the default.
     *   user.sync["age"] += 20;                   // do it NOW
     *   user.async["username"] = "hello world";   // wait for it
     *   user.commit();                            // run username change NOW
     *
     *   auto usr = user.async;
     *   // we should be able to use this instead of keep getting the .async version
     *
     *   auto posts = db["posts"].where(field("id") > 10);                  // haven't fetched them yet
     *   auto names = db["names"].where("date"_field > (now() - 100_days));
     *   auto pics = db.tables["pictures"]
     *                  .select("src")
     *                  .where(
     *                      func::tolower("title"_field) == "cute" ||
     *                      "id"_field == 200
     *                  );
     *   auto authors = db.tables["authors"]
     *                     .select(
     *                          "username",
     *                          "firstname",
     *                          "lastname",
     *                          field(func::concat("firstname", "lastname")).as("name")
     *                     )
     *                     .where("date"_field > now() - 10_days)
     *                     .and_where("date"_field <= now())
     *                     .get();
     *
     *   using func = database::default_database::functions;
     *   posts.each["title"] = func::tolower(post.title);           // tolower is SQL function not C++
     *   posts.update();
     *
     *   db.transaction([&]() {
     *      // ...
     *   });
     * @endcode
     */


    template <typename T>
    concept Connection = requires (T db) {
        db.execute("");
        // todo
    };

    template <typename T>
    concept Handler = requires (T handler) {
        handler.transaction([](){});
        handler.transaction.start();
        handler.transaction.end();
        handler.transaction.raii(); // [[nodiscard]] requires the user to give it a name
        handler.execute("");
    };
}

#endif // WEBPP_DATABASE_CONCEPTS_HPP
