#ifndef WEBPP_SQL_SQLITE_SQLITE_HPP
#define WEBPP_SQL_SQLITE_SQLITE_HPP

#include "../../std/string_view.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/traits.hpp"
#include "sqlite_connection.hpp"
#include "sqlite_grammar.hpp"

namespace webpp::sql {


    template <Traits TraitsType>
    struct basic_sqlite : public sqlite_connection {
        using traits_type  = TraitsType;
        using grammar_type = sqlite_grammar;

        template <typename T>
        using supports_string_view = istl::StringView<T>&& stl::same_as<istl::CharType<T>, char>;

        using sqlite_connection::sqlite_connection;
    };

    using sqlite = basic_sqlite<default_traits>;
    // todo: add pmr::sqlite

} // namespace webpp::sql

#endif
