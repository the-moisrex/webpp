// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/uri_components_encoding.hpp"

namespace webpp::uri {


    template <istl::String StringType = stl::string>
    struct basic_host
      : stl::vector<StringType, rebind_allocator<typename StringType::allocator_type, StringType>> {
        using string_type = StringType;
        using super =
          stl::vector<string_type, rebind_allocator<typename string_type::allocator_type, string_type>>;

        template <typename... T>
        explicit constexpr basic_host(T&&... args) : super{stl::forward<T>(args)...} {}


        template <istl::StringViewifiable StrT>
        constexpr basic_host& operator=(StrT&& inp_str) {
            const auto str =
              istl::stringify_of<string_type>(stl::forward<StrT>(inp_str), this->get_allocator());
            // todo: split it based on the domains
            this->push_back(str);
            return *this;
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        [[nodiscard]] constexpr auto tld() const {
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
