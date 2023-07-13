#include "../webpp/db/sql_database.hpp"
#include "../webpp/db/sqlite/sqlite.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "common/tests_common_pch.hpp"


using namespace webpp;
using namespace webpp::sql;

using sql_db = sql_database<sqlite>;

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


    auto query = db.table("settings");
    query.select("value");
    query.where("name", "username");
    EXPECT_EQ("select value from 'settings' where 'name' = 'username'", query.to_string())
      << query.to_string();
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
              "insert into 'employees' "
              "select firstname as first_name, lastname as last_name from 'users' where 'employed' = true")
      << query.to_string();
}

TEST(Database, WhereClause) {
    sql_database<sqlite> db;

    auto q1 = db.table("test").select("one", "two", "three").where("four", "question");

    EXPECT_EQ(q1.to_string(), "select one, two, three from 'test' where 'four' = 'question'")
      << q1.to_string();


    q1.where_in("four", 1, 2, 3, 4, 5, 6, 7, 8);
    EXPECT_EQ(q1.to_string(), "select one, two, three from 'test' where 'four' in (1, 2, 3, 4, 5, 6, 7, 8)")
      << q1.to_string();


    q1.where_in("four", 1, 2, 3, 4, "five", 6, 7, 8);
    EXPECT_EQ(q1.to_string(),
              "select one, two, three from 'test' where 'four' in (1, 2, 3, 4, 'five', 6, 7, 8)")
      << q1.to_string();

    q1.where_not_in("four", 1, 2, 3, 4, "five", 6, 7, 8);
    EXPECT_EQ(q1.to_string(),
              "select one, two, three from 'test' where 'four' not in (1, 2, 3, 4, 'five', 6, 7, 8)")
      << q1.to_string();


    q1.and_where_not_in("six", 1, 2, 3);
    EXPECT_EQ(
      q1.to_string(),
      "select one, two, three from 'test' where 'four' not in (1, 2, 3, 4, 'five', 6, 7, 8) and 'six' not in (1, 2, 3)")
      << q1.to_string();

    q1.where_in("six", 1, 2, 3);
    EXPECT_EQ(q1.to_string(), "select one, two, three from 'test' where 'six' in (1, 2, 3)")
      << q1.to_string();

    q1.or_where_in("seven", 1, 2, 3);
    EXPECT_EQ(q1.to_string(),
              "select one, two, three from 'test' where 'six' in (1, 2, 3) or 'seven' in (1, 2, 3)")
      << q1.to_string();
}


TEST(Database, UpdateQuery) {
    sql_database<sqlite> db;

    auto q        = db.table("test").where("user_id", 12);
    q["username"] = "moisrex";
    q["password"] = "moisrex loves coding";
    q.update();
    EXPECT_EQ(
      q.to_string(),
      "update 'test' set 'username' = 'moisrex', 'password' = 'moisrex loves coding' where 'user_id' = 12")
      << q.to_string();
}

TEST(Database, DeleteQuery) {
    sql_database<sqlite> db;

    auto q = db.table("test").where("user_id", 12);
    q.remove();
    EXPECT_EQ(q.to_string(), "delete from 'test' where 'user_id' = 12") << q.to_string();

    auto q2 = db.table("test").remove();
    EXPECT_EQ(q2.to_string(), "delete from 'test'");
}

TEST(Database, Joins) {
    sql_database<sqlite> db;

    auto q = db.table("test").left_join_using("table", "using_condition").where("user_id", 12).select();
    EXPECT_EQ(q.to_string(),
              "select * from 'test' left join 'table' using ('using_condition') where 'user_id' = 12")
      << q.to_string();


    auto q2 = db.table("test").right_join_using("table", "using_condition").where("user_id", 12).select();
    EXPECT_EQ(q2.to_string(),
              "select * from 'test' right join 'table' using ('using_condition') where 'user_id' = 12")
      << q2.to_string();
}
