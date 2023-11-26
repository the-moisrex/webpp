// Created by moisrex on 9/30/22.

#ifndef WEBPP_COUNTER_MODEL_HPP
#define WEBPP_COUNTER_MODEL_HPP

#include <webpp/db/sql_database.hpp>
#include <webpp/db/sqlite/sqlite.hpp>

namespace website {
    using namespace webpp;
    using namespace webpp::sql;

    struct counter_model {
      private:
        sql_database<sqlite> db;
        stl::string          ip;

      public:
        counter_model() {
            auto counter = db.table("counter");
            counter["id"].primary().number().not_null();
            counter["ip"].string().unique().not_null();
            counter["val"].number().not_null().default_value(0);
            counter.create_if_not_exists();
        }

        bool increment() {
            auto counter = db.table("counter").where("ip", ip);
            counter["val"]++;
            return counter.update_or_set();
        }

        stl::size_t current() {
            return db.table("counter").where("ip", ip).select("value").first();
        }
    };

} // namespace website
#endif // WEBPP_COUNTER_MODEL_HPP
