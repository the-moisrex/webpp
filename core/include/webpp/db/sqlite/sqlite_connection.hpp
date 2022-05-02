#ifndef WEBPP_DATABASE_SQLITE_CONNECTION_HPP
#define WEBPP_DATABASE_SQLITE_CONNECTION_HPP

#include "../../libs/sqlite.hpp"

#include <cstdint>
#include <string>
#include <string_view>


// sorry that we're using std::string and other features that have nothing to do with the traits system that
// I'v built, the sqlite doesn't support those and it's pretty much useless to have them here so this way, we
// at least will gain some compile-time performance. Connection to the database should be performed when the
// applicaiton is started and will not affect the long running application's performane even though it might
// affect the performance of the protocols like CGI that the whole application is re-started for each request,
// but openning a connection is slow enough that a few allocations are dissmissable.
//
// todo: Should we use a template instead of std::string directly (only for error handlings)
namespace webpp::sql {

    struct sqlite_config {
        std::string_view filename = ":memory:";
        int              flags    = 0;
        std::string_view password{};
        std::string_view vfs{};
    };


    struct sqlite_connection {
      private:
        ::sqlite3* handle{nullptr};


      public:
        void open(sqlite_config conf, std::string& errmsg) noexcept {
            const int rc = sqlite3_open_v2(conf.filename.data(),
                                           &handle,
                                           conf.flags,
                                           conf.vfs.empty() ? nullptr : conf.vfs.data());
            if (rc != SQLITE_OK) {
                errmsg = sqlite3_errmsg(handle);
                (void) sqlite3_close_v2(handle);
                return;
            }

            // todo: SQLCipher sqlite3_key password (https://github.com/rbock/sqlpp11/blob/1e7f4b98c727643513eb94100133c009906809d9/include/sqlpp11/sqlite3/connection.h#L95)

            errmsg.clear(); // no errors
        }

        bool close() noexcept {
            if (handle) {
                if (int res = sqlite3_close_v2(handle); res == SQLITE_OK) {
                    return true;
                }
                return false;
            } else {
                return true;
            }
        }

        void execute(std::string_view sql, std::string& errmsg) noexcept {
            char*     err;
            const int rc = sqlite3_exec(handle, sql.data(), 0, 0, &err);
            if (rc != SQLITE_OK) {
                errmsg = err;
                sqlite3_free(err); // we have copied it
            }
        }

        ~sqlite_connection() noexcept {
            close();
        }


        ::sqlite3* native_handle() noexcept {
            return handle;
        }


        std::uint64_t last_insert_id() noexcept {
            return static_cast<std::uint64_t>(sqlite3_last_insert_rowid(handle));
        }

        // source: https://raw.githubusercontent.com/sqlitebrowser/sqlitebrowser/master/src/sqlitedb.cpp
        template <typename StrT>
        void sqlite_version(StrT& sqlite, StrT& sqlcipher) {
            sqlite = StrT(SQLITE_VERSION);

            // The SQLCipher version must be queried via a pragma and for a pragma we need a database
            // connection. Because we want to be able to query the SQLCipher version without opening a
            // database file first, we open a separate connection to an in-memory database here.
            sqlcipher = StrT();
#ifdef ENABLE_SQLCIPHER
            sqlite3* dummy;
            if (sqlite3_open(":memory:", &dummy) == SQLITE_OK) {
                sqlite3_stmt* stmt;
                if (sqlite3_prepare_v2(dummy, "PRAGMA cipher_version", -1, &stmt, nullptr) == SQLITE_OK) {
                    if (sqlite3_step(stmt) == SQLITE_ROW)
                        sqlcipher = QByteArray(static_cast<const char*>(sqlite3_column_blob(stmt, 0)),
                                               sqlite3_column_bytes(stmt, 0));

                    sqlite3_finalize(stmt);
                }

                sqlite3_close(dummy);
            }
#endif
        }
    };





} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_CONNECTION_HPP
