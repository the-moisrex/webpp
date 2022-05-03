#ifndef WEBPP_DATABASE_SQLITE_STATEMENT_HPP
#define WEBPP_DATABASE_SQLITE_STATEMENT_HPP

#include "../../common/meta.hpp"
#include "../../libs/sqlite.hpp"
#include "../../std/concepts.hpp"
#include "../../std/span.hpp"
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
        /**
         * Possilbe values:
         *   - nullptr_t
         *   - std::basic_string_view<...>
         *   - int/short/long/...
         *   - double/float/...
         *   - blob
         */
        template <typename T>
        void bind(int index, T&& val, istl::String auto& err_msg) {
            using type = stl::remove_cvref_t<T>;
            if constexpr (istl::Span<type>) {
                if (val.size() > stl::numeric_limits<int>::max()) {
                    check_bind_result(sqlite3_bind_blob64(stmt,
                                                          index,
                                                          static_cast<void const*>(val.data()),
                                                          static_cast<sqlite3_int64>(val.size()),
                                                          SQLITE_STATIC),
                                      err_msg);
                } else {
                    check_bind_result(sqlite3_bind_blob(stmt,
                                                        index,
                                                        static_cast<void const*>(val.data()),
                                                        static_cast<int>(val.size()),
                                                        SQLITE_STATIC),
                                      err_msg);
                }
            } else if constexpr (stl::is_floating_point_v<type>) {
                check_bind_result(sqlite3_bind_double(stmt, index, static_cast<double>(val)), err_msg);
            } else if constexpr (stl::integral<type>) {
                if constexpr (sizeof(type) >= 64) {
                    check_bind_result(sqlite3_bind_int64(stmt, index, static_cast<sqlite3_int64>(val)),
                                      err_msg);
                } else {
                    check_bind_result(sqlite3_bind_int(stmt, index, static_cast<int>(val)), err_msg);
                }
            } else if constexpr (istl::StringView<type>) {
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
            } else if constexpr (stl::is_null_pointer_v<type>) { // null
                check_bind_result(sqlite3_bind_null(stmt, index), err_msg);
                if (err_msg.empty()) {
                    err_msg = "bind value set to null because you passed an unknown type.";
                }
            } else {
                static_assert_false(T, "SQLite cannot handle the specified type.");
            }
        }

        void execute() noexcept {}
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_STATEMENT_HPP
