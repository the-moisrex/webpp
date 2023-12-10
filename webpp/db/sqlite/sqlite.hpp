#ifndef WEBPP_SQL_SQLITE_SQLITE_HPP
#define WEBPP_SQL_SQLITE_SQLITE_HPP

#include "../../std/string_view.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/traits.hpp"
#include "sqlite_connection.hpp"
#include "sqlite_grammar.hpp"

#include <vector>

namespace webpp::sql {


    template <Traits TraitsType>
    struct basic_sqlite : public sqlite_connection {
        using traits_type     = TraitsType;
        using grammar_type    = sqlite_grammar;
        using connection_type = sqlite_connection;
        using size_type       = int;
        using float_type      = double;
        using integer_type    = int;
        using string_type     = traits::string<traits_type>;
        using blob_type       = stl::vector<char, traits::allocator_type_of<traits_type, char>>;

        template <typename T>
        static constexpr bool supports_string_view =
          istl::StringView<T> && stl::same_as<istl::char_type_of_t<T>, char>;

        static_assert(supports_string_view<stl::string_view>, "It should support standard string views");

        using sqlite_connection::sqlite_connection;
    };

    using sqlite = basic_sqlite<default_traits>;
    // todo: add pmr::sqlite

} // namespace webpp::sql

#endif
