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

        void check_bind_result(int const result, istl::String auto& err_msg) {
            switch (result) {
                [[likely]] case SQLITE_OK:
                    return;
                case SQLITE_RANGE: err_msg = "bind value out of range"; break;
                case SQLITE_NOMEM: err_msg = "bind value out of memory"; break;
                case SQLITE_TOOBIG: err_msg = "bind value is too big"; break;
                case SQLITE_MISUSE: err_msg = "sqlite misuse"; break;
                default:
                    err_msg += "bind value returned unexpected value: ";
                    err_msg += stl::to_string(result); // todo: use our own version of this function call
                    err_msg +=
                      "; check https://www.sqlite.org/c3ref/bind_blob.html for more info about this error.";
                    break;
            }
        }

      public:
        constexpr sqlite_statement() noexcept = default;

        explicit constexpr sqlite_statement(::sqlite3_stmt* in_stmt) noexcept : stmt{in_stmt} {}

        constexpr sqlite_statement(sqlite_statement const&) = delete;

        constexpr sqlite_statement(sqlite_statement&& in_stmt) noexcept : stmt{in_stmt.stmt} {
            in_stmt.stmt = nullptr; // stop the pointer from becoming "destroyed".
        }

        sqlite_statement& operator=(sqlite_statement const&) = delete;

        sqlite_statement& operator=(sqlite_statement&& in_stmt) noexcept {
            if (stmt == in_stmt.stmt) {
                return *this;
            }

            destroy();
            stmt         = in_stmt.stmt;
            in_stmt.stmt = nullptr; // stop the pointer from becoming "destroyed".
            return *this;
        }

        sqlite_statement& operator=(::sqlite3_stmt* in_stmt) noexcept {
            destroy();
            stmt = in_stmt;
            return *this;
        }

        bool operator==(sqlite_statement const& rhs) const noexcept {
            return stmt == rhs.stmt;
        }

        auto operator<=>(sqlite_statement const& rhs) const noexcept {
            return stmt <=> rhs.stmt;
        }

        ~sqlite_statement() noexcept {
            destroy();
        }

        inline void destroy() noexcept {
            if (stmt != nullptr) {
                (void) sqlite3_finalize(stmt);
            }
        }

        inline void reset(istl::String auto& errmsg) noexcept {
            int const rc = sqlite3_reset(stmt);
            if (rc != SQLITE_OK) {
                errmsg += "Sqlite reset error with code: ";
                errmsg += stl::to_string(rc);
                // todo: a better error please that supports other string types as well.
            }
        }

        inline ::sqlite3_stmt*& sqlite3_stmt() noexcept {
            return stmt;
        }

        /**
         * Possible values:
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
                    check_bind_result(
                      sqlite3_bind_blob64(stmt,
                                          index,
                                          static_cast<void const*>(val.data()),
                                          static_cast<sqlite3_int64>(val.size()),
                                          SQLITE_STATIC),
                      err_msg);
                } else {
                    check_bind_result(
                      sqlite3_bind_blob(stmt,
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
                // this function will only receive string views.
                using value_type                        = typename T::value_type;
                static constexpr stl::size_t value_size = sizeof(value_type);

                if constexpr (value_size >= 64) { // 64bit string
                    // todo: it's possible for this string type to be utf-16 as well, write tests for this
                    check_bind_result(
                      sqlite3_bind_text64(
                        stmt,
                        index,
                        val.data(),
                        static_cast<int>(val.size()),
                        SQLITE_STATIC,
                        SQLITE_UTF8),
                      err_msg);
                } else if constexpr (value_size == 16) { // utf-16
                    check_bind_result(
                      sqlite3_bind_text16(stmt,
                                          index,
                                          static_cast<int>(val.size()),
                                          val.size(),
                                          SQLITE_STATIC),
                      err_msg);
                } else { // normal string
                    check_bind_result(
                      sqlite3_bind_text(stmt, index, val.data(), static_cast<int>(val.size()), SQLITE_STATIC),
                      err_msg);
                }
            } else if constexpr (stl::is_null_pointer_v<type>) { // null
                check_bind_result(sqlite3_bind_null(stmt, index), err_msg);
                if (err_msg.empty()) {
                    err_msg += "bind value set to null because you passed an unknown type.";
                }
            } else {
                static_assert_false(T, "SQLite cannot handle the specified type.");
            }
        }

        bool step(istl::String auto& errmsg) {
            assert(stmt != nullptr);
            int const rc = sqlite3_step(stmt);
            switch (rc) {
                // this is an embarrassing situation for clang-format
                // clang-format off
                [[likely]]
                case SQLITE_OK: return false;
                case SQLITE_ROW: return true; // might occur if execute is called with a select
                case SQLITE_DONE:
                    return false;
                // clang-format on
                default: {
                    ::sqlite3* handle  = sqlite3_db_handle(stmt);
                    errmsg            += "SQLite3 error, could not execute prepared statement: ";
                    errmsg            += sqlite3_errmsg(handle);
                    return false;
                }
            }
        }

        /**
         * Get the column name based on its index
         */
        template <istl::String StrT>
        void column_name(int const index, StrT& name_ref) const {
            using str_value_type = typename StrT::value_type;
            if constexpr (istl::UTF16<StrT>) {
                void const* name = sqlite3_column_name16(stmt, index);
                if (!name) [[unlikely]] {
                    // memory error happened
                    // todo: error handling
                    return;
                }
                name_ref = reinterpret_cast<str_value_type const*>(name);
            } else {
                char const* name = sqlite3_column_name(stmt, index);
                if (!name) [[unlikely]] {
                    // memory error happened
                    // todo: error handling
                    return;
                }
                if constexpr (stl::same_as<str_value_type, char>) {
                    name_ref = name;
                } else {
                    name_ref = reinterpret_cast<str_value_type const*>(name);
                }
            }
        }

        // how many columns do we have
        [[nodiscard]] inline int column_count() const noexcept {
            return sqlite3_column_count(stmt);
        }

        [[nodiscard]] inline supported_column_types column_type(int const index) const noexcept {
            return static_cast<supported_column_types>(sqlite3_column_type(stmt, index));
        }

        [[nodiscard]] inline column_category column_cat(int const index) const noexcept {
            switch (sqlite3_column_type(stmt, index)) {
                case SQLITE_FLOAT: [[fallthrough]];
                case SQLITE_INTEGER: return column_category::number;
                case SQLITE_TEXT: return column_category::string;
                case SQLITE_BLOB: return column_category::blob;
                default: return column_category::unknown;
            }
        }

        [[nodiscard]] inline bool is_column_null(int const index) const noexcept {
            return sqlite3_column_type(stmt, index) == SQLITE_NULL;
        }

        [[nodiscard]] inline bool is_column_float(int const index) const noexcept {
            return sqlite3_column_type(stmt, index) == SQLITE_FLOAT;
        }

        [[nodiscard]] inline bool is_column_integer(int const index) const noexcept {
            return sqlite3_column_type(stmt, index) == SQLITE_INTEGER;
        }

        [[nodiscard]] inline int as_int(int const index) const noexcept {
            return sqlite3_column_int(stmt, index);
        }

        [[nodiscard]] inline stl::int64_t as_int64(int const index) const noexcept {
            return static_cast<stl::int64_t>(sqlite3_column_int64(stmt, index));
        }

        [[nodiscard]] inline double as_double(int const index) const noexcept {
            return sqlite3_column_double(stmt, index);
        }

        template <istl::String StrT = stl::string>
        void as_string(int const index, StrT& out) const noexcept {
            // todo: handle text16
            char const* str     = reinterpret_cast<char const*>(sqlite3_column_text(stmt, index));
            auto const  str_len = static_cast<stl::size_t>(sqlite3_column_bytes(stmt, index));
            out.append(str, str_len);
        }
    };

} // namespace webpp::sql

#endif // WEBPP_DATABASE_SQLITE_STATEMENT_HPP
