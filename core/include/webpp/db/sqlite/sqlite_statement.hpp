#ifndef WEBPP_DATABASE_SQLITE_STATEMENT_HPP
#define WEBPP_DATABASE_SQLITE_STATEMENT_HPP

#include "../../common/meta.hpp"
#include "../../libs/sqlite.hpp"
#include "../../std/concepts.hpp"
#include "../../std/span.hpp"
#include "../../std/string.hpp"
#include "../../std/string_concepts.hpp"
#include "../../std/string_view.hpp"

namespace webpp::sql {


    struct sqlite_statement {
        using size_type = int;
        enum struct supported_column_types {
            integer_col = SQLITE_INTEGER,
            float_col   = SQLITE_FLOAT,
            text_col    = SQLITE_TEXT,
            blob_col    = SQLITE_BLOB,
            null_col    = SQLITE_NULL
        };

      private:
        ::sqlite3_stmt* stmt = nullptr;

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
        constexpr sqlite_statement() noexcept = default;
        constexpr sqlite_statement(::sqlite3_stmt* in_stmt) noexcept : stmt{in_stmt} {}
        constexpr sqlite_statement(sqlite_statement const&)     = default;
        constexpr sqlite_statement(sqlite_statement&&) noexcept = default;

        sqlite_statement& operator=(const sqlite_statement&)     = default;
        sqlite_statement& operator=(sqlite_statement&&) noexcept = default;


        sqlite_statement& operator=(::sqlite3_stmt* in_stmt) noexcept {
            stmt = in_stmt;
            return *this;
        }


        ~sqlite_statement() noexcept {
            destroy();
        }

        inline void destroy() noexcept {
            if (stmt) {
                (void) sqlite3_finalize(stmt);
            }
        }


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

        void step(std::string& errmsg) {
            const int rc = sqlite3_step(stmt);
            switch (rc) {
                // this is an embarrassing situation for clang-format
                // clang-format off
                [[likely]]
                case SQLITE_OK:
                case SQLITE_ROW: // might occur if execute is called withe a select
                case SQLITE_DONE:
                    return;
                // clang-format on
                default: {
                    errmsg = "SQLite3 error, could not execute prepared statement: ";
                    errmsg += sqlite3_errmsg(sqlite3_db_handle(stmt));
                }
            }
        }

        /**
         * Get the column name based on its index
         */
        template <istl::String StrT>
        void column_name(int index, StrT& name_ref) const {
            using str_value_type = typename StrT::value_type;
            if constexpr (istl::UTF16<StrT>) {
                const void* name = sqlite3_column_name16(stmt, index);
                if (!name) [[unlikely]] {
                    // memory error happened
                    // todo: error handling
                    return;
                }
                name_ref = reintepret_cast<str_value_type const*>(name);
            } else {
                const char* name = sqlite3_column_name(stmt, index);
                if (!name) [[unlikely]] {
                    // memory error happened
                    // todo: error handling
                    return;
                }
                if constexpr (stl::same_as<str_value_type, char>) {
                    name_ref = name;
                } else {
                    name_ref = reintepret_cast<str_value_type const*>(name);
                }
            }
        }

        // how many columns do we have
        [[nodiscard]] inline int column_count() const noexcept {
            return sqlite3_column_count(stmt);
        }

        [[nodiscard]] inline supported_column_types column_type(int index) const noexcept {
            return static_cast<supported_column_types>(sqlite3_column_type(stmt, index));
        }

        [[nodiscard]] inline column_category column_category(int index) const noexcept {
            switch (sqlite3_column_type(stmt, index)) {
                default: return column_category::unknown;
            }
        }


        [[nodiscard]] inline bool is_column_null(int index) const noexcept {
            return sqlite3_column_type(stmt, index) == SQLITE_NULL;
        }


        [[nodiscard]] inline int as_int(int index) const noexcept {
            return sqlite3_column_int(stmt, index);
        }

        [[nodiscard]] inline stl::int64_t as_int64(int index) const noexcept {
            return static_cast<stl::int64_t>(sqlite3_column_int64(stmt, index));
        }

        [[nodiscard]] inline double as_double(int index) const noexcept {
            return sqlite3_column_double(stmt, index);
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_STATEMENT_HPP