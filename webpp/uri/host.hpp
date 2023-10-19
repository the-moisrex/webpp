// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/uri_components.hpp"

namespace webpp::uri {


    template <typename... T>
    static constexpr void
    parse_host(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // todo
    }


    template <istl::String StringType = stl::string>
    struct basic_host : stl::vector<stl::remove_cvref_t<StringType>,
                                    rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type,
                                                     stl::remove_cvref_t<StringType>>> {
        using super       = stl::vector<stl::remove_cvref_t<StringType>,
                                  rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type,
                                                   stl::remove_cvref_t<StringType>>>;
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename... T>
        constexpr basic_host(T&&... args) : super{stl::forward<T>(args)...} {}


        template <istl::StringViewifiable StrT>
        constexpr basic_host& operator=(StrT&& str) {
            const auto s = istl::stringify_of<string_type>(stl::forward<StrT>(str), this->get_allocator());
            // todo: split it based on the domains
            this->push_back(s);
            return *this;
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        auto tld() const {
            return this->back();
        }

        void append_to(istl::String auto& str) const {
            for (auto const& subdomain : *this) {
                str.append(subdomain);
            }
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_HOST_HPP
