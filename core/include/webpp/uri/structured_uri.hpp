// Created by moisrex on 10/30/20.

#ifndef WEBPP_STRUCTURED_URI_HPP
#define WEBPP_STRUCTURED_URI_HPP

namespace webpp {

    template <typename StringType>
    struct structured_uri {
        using string_type = StringType;

        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        string_type data{};

        stl::size_t scheme_end = string_type::npos;
        stl::size_t authority_start = string_type::npos;
        stl::size_t user_info_end = string_type::npos;
        stl::size_t port_start = string_type::npos;
        stl::size_t authority_end = string_type::npos;
        stl::size_t query_start = string_type::npos;
        stl::size_t fragment_start = string_type::npos;

    };

}

#endif // WEBPP_STRUCTURED_URI_HPP
