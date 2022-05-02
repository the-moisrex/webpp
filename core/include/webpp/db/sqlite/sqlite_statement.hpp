#ifndef WEBPP_DATABASE_SQLITE_STATEMENT_HPP
#define WEBPP_DATABASE_SQLITE_STATEMENT_HPP


namespace webpp::sql {


    struct sqlite_statement {

        template <typename T>
        void bind(int index, T&& val) noexcept {

        }

        void execute() noexcept {

        }

    };

}

#endif // WEBPP_DATABASE_SQLITE_STATEMENT_HPP
