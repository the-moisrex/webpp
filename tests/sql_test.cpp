#include "../core/include/webpp/db/query.hpp"
#include "../core/include/webpp/db/sql_database.hpp"
#include "../core/include/webpp/db/sqlite/sqlite.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::sql;

static_assert(SQLStatement<sqlite_statement>, "sqlite statement is not a statement.");


TEST(Database, SQLiteWrapper) {
    sql_database<sqlite> db; // in memory database
    ASSERT_TRUE(db.is_open());
    ASSERT_TRUE(db.execute(R"sql(create table settings(
        id integer primary key,
        name text,
        value text
    );)sql"));

    auto stmt = db.prepare("insert into settings (name, value) values (?, ?)");
    stmt.bind(1, "username");
    stmt.bind(2, "moisrex");
    stmt.execute();


    stmt = db.prepare("select * from settings;");
    while (stmt.step()) {
        auto        id    = stmt.column(0);
        stl::string name  = stmt.column(1);
        stl::string value = stmt.column(2);
        EXPECT_TRUE(id.is_number());
        EXPECT_EQ(name, "username");
        EXPECT_EQ(value, "moisrex");
    }


    int index = 0;
    for (auto row : stmt.reset().structured<3>()) {
        using row_type = stl::remove_cvref_t<decltype(row)>;
        // EXPECT_EQ(row.size(), 3);
        EXPECT_EQ(stl::tuple_size_v<row_type>, 3);
        auto [id, name, value] = row;
        EXPECT_TRUE(id.is_number()) << "row: " << index;
        EXPECT_TRUE(name.is_string()) << "row: " << index;
        EXPECT_TRUE(value.is_string()) << "row: " << index;
        EXPECT_EQ(name, "username") << "row: " << index;
        EXPECT_EQ(value, "moisrex") << "row: " << index;

        index++;
    }


    stmt = db.prepare("insert into settings (name, value) values (?, ?)");
    stmt.bind(1, "password");
    stmt.bind(2, "123");
    stmt.execute();
    stmt.reset();
    stmt.bind(1, "email");
    stmt.bind(2, "example@example.com");
    stmt.execute();

    stmt      = db.prepare("select count(*) from settings;");
    int count = stmt.first()[0];
    EXPECT_EQ(count, 3);


    stmt             = db.prepare("select value from settings where name = ?;");
    stmt[1]          = "password";
    std::string pass = stmt.first()[0];
    EXPECT_EQ(pass, "123");
}


TEST(Database, QueryBuilderTest) {
    sql_database<sqlite> db; // in memory database

    // query builder is different than schema builder
    ASSERT_TRUE(db.execute(R"sql(create table settings(
        id integer primary key,
        name text,
        value text
    );)sql"));

    auto inserter     = db.table("settings");
    inserter["name"]  = "username";
    inserter["value"] = "moisrex";
    inserter.insert();
    inserter.insert({
      {"name", "password"}, // col 2
      {"value", 123}        // col 3
    });


    auto& query = db.table("settings") //
                    .select("value")
                    .where("name", "username");
    EXPECT_EQ("select value from settings where name = username", query.to_string());
}


TEST(Database, InsertSelectQuery) {
    // A test for https://github.com/the-moisrex/webpp/issues/146

    sql_database<sqlite> db;

    auto query = db.table("employees")
                   .insert(db.table("users")
                             .where("employed", true)
                             .select("firstname as first_name", "lastname as last_name"));

    // the alias is automatically gets added for firstname, but it doesn't get added for lastname because it
    // already has an alias
    EXPECT_EQ(query.to_string(),
              "insert into employees "
              "select firstname as first_name, lastname as last_name from users where employed = 1")
      << query.to_string();
}
