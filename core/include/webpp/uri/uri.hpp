// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "./scheme.hpp"
#include "./user_info.hpp"
#include "./path.hpp"
#include "./host.hpp"
#include "./queries.hpp"
#include "./fragments.hpp"

namespace webpp::uri {

    template <istl::String StringType>
    struct basic_uri {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type = typename string_type::value_type;
        using allocator_type = typename stirng_type::allocator_type;

        using scheme_type = basic_scheme<string_type>;
        using user_info_type = basic_uri_info<string_type>;
        using host_type = basic_host<string_type>;
        using path_type = basic_path<string_type>;
        using queries_type = basic_queries<string_type>;
        using fragments_type = basic_fragments<stirng_type>;

        scheme_type scheme{};
        user_info_type user_info{};
        host_type host{};
        path_type path{};
        queries_type queries{};
    };

    using uri = basic_uri<stl::string>;
}

#endif // WEBPP_URI_HPP
