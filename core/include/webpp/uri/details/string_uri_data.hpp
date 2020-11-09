// Created by moisrex on 10/30/20.

#ifndef WEBPP_STRING_URI_DATA_HPP
#define WEBPP_STRING_URI_DATA_HPP

#include "../../strings/charset.hpp"
#include "../../utils/allocators.hpp"

namespace webpp::uri::details {

    template <Traits TraitsType, bool Mutable>
    struct string_uri_data
      : public allocator_holder<typename TraitsType::allocator<typename TraitsType::char_type>> {
        using traits_type       = TraitsType;
        using string_type       = typename traits_type::string_type;
        using string_view_type  = typename traits_type::string_view_type;
        using char_type         = typename traits_type::char_type;
        using allocator_type    = typename traits_type::allocator<char_type>;
        using alloc_holder_type = allocator_holder<allocator_type>;
        using storred_str_t     = auto_string_type<traits_type, Mutable>;

        explicit string_uri_data(auto&& alloc) : alloc_holder_type{alloc} {}
        constexpr string_uri_data(const char_type* u) noexcept : data(u) {}


        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr string_uri_data(storred_str_t const& u) noexcept : data(u) {}

        /**
         * If the user uses this
         */
        constexpr string_uri_data(storred_str_t&& u) noexcept : data(stl::move(u)) {}

        constexpr string_uri_data(string_uri_data const& bu) noexcept
          : data{bu.data},
            scheme_end{bu.scheme_end},
            authority_start{bu.authority_start},
            user_info_end{bu.user_info_end},
            port_start{bu.port_start},
            authority_end{bu.authority_end},
            query_start{bu.query_start},
            fragment_start{bu.fragment_start} {}

        constexpr string_uri_data(string_uri_data&& bu) noexcept
          : data{stl::move(bu.data)},
            scheme_end{stl::move(bu.scheme_end)},
            authority_start{stl::move(bu.authority_start)},
            user_info_end{stl::move(bu.user_info_end)},
            port_start{stl::move(bu.port_start)},
            authority_end{stl::move(bu.authority_end)},
            query_start{stl::move(bu.query_start)},
            fragment_start{stl::move(bu.fragment_start)} {}



        // assignment operators
        string_uri_data& operator=(string_uri_data const& u) noexcept {
            data            = u.data;
            scheme_end      = u.scheme_end;
            authority_start = u.authority_start;
            user_info_end   = u.user_info_end;
            port_start      = u.port_start;
            authority_end   = u.authority_end;
            query_start     = u.query_start;
            fragment_start  = u.fragment_start;
        }

        string_uri_data& operator=(string_uri_data&& u) noexcept {
            data            = stl::move(u.data);
            scheme_end      = stl::move(u.scheme_end);
            authority_start = stl::move(u.authority_start);
            user_info_end   = stl::move(u.user_info_end);
            port_start      = stl::move(u.port_start);
            authority_end   = stl::move(u.authority_end);
            query_start     = stl::move(u.query_start);
            fragment_start  = stl::move(u.fragment_start);
        }

        string_uri_data& operator=(storred_str_t const& u) noexcept {
            data = u;
            unparse();
            return *this;
        }

        string_uri_data& operator=(storred_str_t&& u) noexcept {
            data = stl::move(u);
            unparse();
            return *this;
        }

      protected:
        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        storred_str_t data{};

        stl::size_t scheme_end      = string_view_type::npos;
        stl::size_t authority_start = string_view_type::npos;
        stl::size_t user_info_end   = string_view_type::npos;
        stl::size_t port_start      = string_view_type::npos;
        stl::size_t authority_end   = string_view_type::npos;
        stl::size_t query_start     = string_view_type::npos;
        stl::size_t fragment_start  = string_view_type::npos;
    };

} // namespace webpp::uri

#endif // WEBPP_STRING_URI_DATA_HPP
