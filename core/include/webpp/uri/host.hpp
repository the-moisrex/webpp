// Created by moisrex on 11/4/20.

#ifndef WEBPP_HOST_HPP
#define WEBPP_HOST_HPP

#include "../std/string.hpp"
#include "../std/vector.hpp"
#include "../utils/allocators.hpp"

namespace webpp::uri {

    template <istl::String StringType = stl::string>
    struct basic_host : stl::vector<stl::remove_cvref_t<StringType>,
      rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type, stl::remove_cvref_t<StringType>>> {
        using super = stl::vector<stl::remove_cvref_t<StringType>,
          rebind_allocator<typename stl::remove_cvref_t<StringType>::allocator_type, stl::remove_cvref_t<StringType>>>;
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename ...T>
        constexpr basic_host(T&&...args) : super{stl::forward<T>(args)...} {}


        /**
         * Top Level Domain; sometimes called the extension
         */
        auto tld() const {
            return this->back();
        }

        void append_to(istl::String auto&str) const {
            for (auto const& subdomain : *this) {
                str.append(subdomain);
            }
        }

    };

}

#endif // WEBPP_HOST_HPP
