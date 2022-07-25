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
    ASSERT_TRUE(db.execute(R"sql(create table settings(
        id integer primary key,
        name text,
        value text
    ))sql"));

    auto stmt = db.prepare("insert into settings (name, value) values (?, ?)");
    stmt.bind(0, "username");
    stmt.bind(1, "moisrex");
    stmt.step();


    stmt = db.prepare("select * from settings;");
    while (stmt.step()) {
        auto        id    = stmt.column(0);
        stl::string name  = stmt.column(1);
        stl::string value = stmt.column(2);
        EXPECT_TRUE(id.is_number());
        EXPECT_EQ(name, "username");
        EXPECT_EQ(value, "moisrex");
    }

    stmt = db.prepare("select * from settings;");

    for (auto row : stmt.structured<3>()) {
        using row_type = stl::remove_cvref_t<decltype(row)>;
        // EXPECT_EQ(row.size(), 3);
        EXPECT_EQ(stl::tuple_size_v<row_type>, 3);
        auto [id, name, value] = row;
        EXPECT_TRUE(name.is_number());
        EXPECT_TRUE(name.is_string());
        EXPECT_TRUE(value.is_string());
        EXPECT_EQ(name, "username");
        EXPECT_EQ(value, "moisrex");
    }
}
