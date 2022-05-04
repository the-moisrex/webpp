#include "../core/include/webpp/db/query.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::sql;


TEST(Database, SQLiteWrapper) {
    database<sqlite> db; // in memory database
    db.execute(R"sql(create table settings(
        id integer primary key,
        name text,
        value text
    ))sql");

    auto stmt = db.prepare("insert into settings (name, value) values (?, ?)");
    stmt.bind(0, "username");
    stmt.bind(1, "moisrex");
    stmt.execute();


    stmt = db.prepare("select * from settings;");
    while (stmt.step()) {
        stl::uint64_t id    = stmt.column(0);
        stl::string   name  = stmt.column(1);
        stl::string   value = stmt.column(2);
        EXPECT_EQ(name, "username");
        EXPECT_EQ(value, "moisrex");
    }

    stmt = db.prepare("select * from settings;");
    for (auto [id, name, value] : stmt) {
        EXPECT_EQ(name, "username");
        EXPECT_EQ(value, "moisrex");
    }
}

