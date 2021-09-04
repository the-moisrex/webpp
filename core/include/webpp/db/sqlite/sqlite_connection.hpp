#ifndef WEBPP_DATABASE_SQLITE_CONNECTION_HPP
#define WEBPP_DATABASE_SQLITE_CONNECTION_HPP

#include "../../libs/sqlite.hpp"
#include "../../traits/traits.hpp"

#include <utility>

namespace webpp::database {

    template <Traits TraitsType>
    struct sqlite_connection {
        using traits_type = TraitsType;
        using string_type = traits::string<traits_type>;

      private:
        sqlite3* handle{nullptr};


      public:
        bool open() {}

        bool close() {
            if (handle) {
                sqlite3_close(handle);
                return true;
            } else {
                return true;
            }
        }

        ~sqlite_connection() {
            close();
        }
    };




    // source: https://raw.githubusercontent.com/sqlitebrowser/sqlitebrowser/master/src/sqlitedb.cpp
    template <istl::String StrT>
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



} // namespace webpp::database

#endif // WEBPP_DATABASE_SQLITE_CONNECTION_HPP
