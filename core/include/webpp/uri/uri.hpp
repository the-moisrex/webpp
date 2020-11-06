// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "./scheme.hpp"
#include "./user_info.hpp"
#include "./path.hpp"
#include "./host.hpp"
#include "./queries.hpp"
#include "./fragments.hpp"
#include "./port.hpp"

namespace webpp::uri {

    template <istl::String StringType>
    struct basic_uri {
        using string_type = stl::remove_cvref_t<StringType>;
        using char_type = typename string_type::value_type;
        using allocator_type = typename stirng_type::allocator_type;

        using scheme_type = basic_scheme<string_type>;
        using user_info_type = basic_uri_info<string_type>;
        using host_type = basic_host<string_type>;
        using port_type = basic_port<string_type>;
        using path_type = basic_path<string_type>;
        using queries_type = basic_queries<string_type>;
        using fragments_type = basic_fragments<stirng_type>;

        scheme_type scheme{};
        user_info_type user_info{};
        host_type host{};
        port_type port{};
        path_type path{};
        queries_type queries{};
        fragments_type fragments{};


        void append_to(istl::String auto& out) {
            // estimate the size
            // todo: check if it has a good impact on performance or it's just in the way
            out.reserve(
              out.size() + // the size of out itself
              scheme.size() + // the scheme size
              user_info.username.size() + // the username size
              user_info.password.size() + 1 + // the password size + 1 character for @
              host.size() + // host size
              port.size() + 1 // port size + 1 character for :
              path.raw_string_size() + // path size
              queries.raw_string_size() + // queries size
              fragments.size() // fragments size
              );
            scheme.append_to(out);
            user_info.append_to(out);
            host.append_to(out);
            port.append_to(out);
            path.append_to(out);
            queries.append_to(out);
            fragments.append_to(out);
        }
    };

    using uri = basic_uri<stl::string>;
}

#endif // WEBPP_URI_HPP
