#ifndef WEBPP_DATABASE_SQLITE_STATEMENT_HPP
#define WEBPP_DATABASE_SQLITE_STATEMENT_HPP

#include "../../libs/sqlite.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"

namespace webpp::sql {


    struct sqlite_statement {
      private:
        ::sqlite3_stmt* stmt;

        void check_bind_result(int result, istl::String auto& err_msg) {
            switch (result) {
                [[likely]] case SQLITE_OK : return;
                case SQLITE_RANGE: err_msg = "bind value out of range"; break;
                case SQLITE_NOMEM: err_msg = "bind value out of memory"; break;
                case SQLITE_TOOBIG: err_msg = "bind value is too big"; break;
                default:
                    err_msg = "bind value returned unexpected value: ";
                    err_msg += stl::to_string(result); // todo: use our own version of this function call
                    err_msg +=
                      "; check https://www.sqlite.org/c3ref/bind_blob.html for more info about this error.";
                    break;
            }
        }

      public:
        template <typename T>
        void bind(int index, T&& val, istl::String auto& err_msg) noexcept {
            if constexpr (stl::integral<T>) {
                check_bind_result(sqlite3_bind_int(stmt, index, static_cast<int>(val)), err_msg);
            } else if constexpr (istl::StringView<T>) {
                // sql_statement functions will take care of strings and conversions to string,
                // this function will only recieve string views.
                using value_type                        = typename T::value_type;
                static constexpr stl::size_t value_size = sizeof(value_type);

                if constexpr (value_size >= 64) { // 64bit string
                    // todo: it's possible for this string type to be utf-16 as well, write tests for this
                    check_bind_result(
                      sqlite3_bind_text64(stmt, index, val.data(), val.size(), SQLITE_STATIC, SQLITE_UTF8),
                      err_msg);
                } else if constexpr (value_size == 16) { // utf-16
                    check_bind_result(sqlite3_bind_text16(stmt, index, val.data(), val.size(), SQLITE_STATIC),
                                      err_msg);
                } else { // normal string
                    check_bind_result(sqlite3_bind_text(stmt, index, val.data(), val.size(), SQLITE_STATIC),
                                      err_msg);
                }
            } else {
                check_bind_result(sqlite3_bind_null(stmt, index), err_msg);
                if (err_msg.empty()) {
                    err_msg = "bind value set to null because you passed an unknown type.";
                }
            }
        }

        void execute() noexcept {}
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_STATEMENT_HPP
