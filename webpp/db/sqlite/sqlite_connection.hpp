#ifndef WEBPP_DATABASE_SQLITE_CONNECTION_HPP
#define WEBPP_DATABASE_SQLITE_CONNECTION_HPP

#include "../../common/os.hpp"
#include "../../libs/sqlite.hpp"
#include "sqlite_statement.hpp"

#include <cstdint>
#include <string>
#include <string_view>


// sorry that we're using std::string and other features that have nothing to do with the traits system that
// I'v built, the sqlite doesn't support those and it's pretty much useless to have them here so this way, we
// at least will gain some compile-time performance. Connection to the database should be performed when the
// application is started and will not affect the long running application's performance even though it might
// affect the performance of the protocols like CGI that the whole application is re-started for each request,
// but opening a connection is slow enough that a few allocations are dismissible.
//
// todo: Should we use a template instead of std::string directly (only for error handling)
namespace webpp::sql {

    struct sqlite_config {
        std::string_view filename = ":memory:";
        int              flags    = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        std::string_view password{};
#ifdef WINDOWS_SYSTEM
        std::string_view vfs = "win32";
#else
        std::string_view vfs = "unix";
#endif
    };


    struct sqlite_connection {
        using statement_type = sqlite_statement;

      private:
        ::sqlite3* handle{nullptr};


      public:
        // todo: add support for vfs

        sqlite_connection()                                    = default;
        sqlite_connection(sqlite_connection const&)            = delete;
        sqlite_connection(sqlite_connection&&) noexcept        = default;
        sqlite_connection& operator=(sqlite_connection const&) = delete;
        sqlite_connection& operator=(sqlite_connection&& other) noexcept {
            using std::swap;
            using stl::swap;
            if (handle != other.handle) {
                close();
                swap(handle, other.handle);
            } else {
                other.handle = nullptr;
            }
            return *this;
        }


        // in memory
        void open(istl::String auto& errmsg) noexcept {
            open(sqlite_config{}, errmsg);
        }

        void open(sqlite_config conf, istl::String auto& errmsg) noexcept {
            const int rc = sqlite3_open_v2(conf.filename.data(),
                                           &handle,
                                           conf.flags,
                                           conf.vfs.empty() ? nullptr : conf.vfs.data());
            if (rc != SQLITE_OK) {
                errmsg += sqlite3_errmsg(handle);
                (void) sqlite3_close_v2(handle);
                return;
            }
            assert(handle != nullptr);

            // todo: SQLCipher sqlite3_key password (https://github.com/rbock/sqlpp11/blob/1e7f4b98c727643513eb94100133c009906809d9/include/sqlpp11/sqlite3/connection.h#L95)
        }

        [[nodiscard]] bool is_open() const noexcept {
            return handle != nullptr;
        }

        bool close() noexcept {
            if (handle) {
                if (const int res = sqlite3_close_v2(handle); res == SQLITE_OK) {
                    handle = nullptr;
                    return true;
                }
                return false;
            } else {
                return true;
            }
        }

        void execute(std::string_view sql, istl::String auto& errmsg) noexcept {
            char*     err;
            const int rc = sqlite3_exec(handle, sql.data(), nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                errmsg += err;
                sqlite3_free(err); // we have copied it
            }
        }

        template <istl::String StrT>
        statement_type prepare(std::string_view stmt_str, StrT& errmsg) noexcept {
            statement_type stmt;
            prepare(stmt_str, stmt, errmsg);
            return stmt;
        }


        // prepare a statement for the parent sql connection to wrap it
        void prepare(std::string_view stmt_str, statement_type& stmt, istl::String auto& errmsg) noexcept {
            // todo: there's a performance gain if you know the string is null terminated. (more: https://www.sqlite.org/c3ref/prepare.html)

            const int rc = sqlite3_prepare_v2(handle,
                                              stmt_str.data(),
                                              static_cast<int>(stmt_str.size()),
                                              &stmt.sqlite3_stmt(),
                                              nullptr);
            if (rc != SQLITE_OK) [[unlikely]] {
                errmsg += "SQLite3 error, could not prepare statement: ";
                errmsg += sqlite3_errmsg(handle);
                errmsg += "; for statement: ";
                if (rc != SQLITE_TOOBIG) {
                    errmsg += stmt_str;
                } else {
                    errmsg += stmt_str.substr(0, 128);
                    errmsg += "...";
                }
            }
        }

        ~sqlite_connection() noexcept {
            static_cast<void>(close());
        }


        ::sqlite3* native_handle() noexcept {
            return handle;
        }


        std::uint64_t last_insert_id() noexcept {
            return static_cast<std::uint64_t>(sqlite3_last_insert_rowid(handle));
        }


        // escape string
        constexpr void escape(auto&& input, istl::String auto& output) const noexcept {
            // todo: optimize this using SIMD if the compiler doesn't already do it
            for (const auto c : input) {
                if (c == '\'')
                    output.push_back(c);
                output.push_back(c);
            }
        }

        constexpr void quoted_escape(auto&& input, istl::String auto& output) const noexcept {
            output.reserve(output.size() + input.size() + 2);
            output.push_back('\'');
            escape(input, output);
            output.push_back('\'');
        }


        template <istl::String StrT>
        void version(StrT& sqlite) const {
            sqlite += StrT(SQLITE_VERSION);
        }

        // source: https://raw.githubusercontent.com/sqlitebrowser/sqlitebrowser/master/src/sqlitedb.cpp
        template <istl::String StrT>
        void sqlite_sqlcipher_version(StrT& sqlcipher) const {

            // The SQLCipher version must be queried via a pragma and for a pragma we need a database
            // connection. Because we want to be able to query the SQLCipher version without opening a
            // database file first, we open a separate connection to an in-memory database here.
#ifdef ENABLE_SQLCIPHER
            sqlite3* dummy;
            if (sqlite3_open(":memory:", &dummy) == SQLITE_OK) {
                sqlite3_stmt* stmt;
                if (sqlite3_prepare_v2(dummy, "PRAGMA cipher_version", -1, &stmt, nullptr) == SQLITE_OK) {
                    // fixme: QByteArray? really?
                    if (sqlite3_step(stmt) == SQLITE_ROW)
                        sqlcipher += QByteArray(static_cast<const char*>(sqlite3_column_blob(stmt, 0)),
                                                sqlite3_column_bytes(stmt, 0));

                    sqlite3_finalize(stmt);
                }

                sqlite3_close(dummy);
            }
#else
            sqlcipher = "Unknown";
#endif
        }
    };





} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_CONNECTION_HPP
