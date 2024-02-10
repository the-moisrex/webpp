// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../memory/allocators.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "./details/uri_components_encoding.hpp"

namespace webpp::uri {


    /**
     * @brief Basic Host Name
     * @tparam StringType
     *
     * todo: hide vector
     */
    template <istl::String StringType = stl::string>
    struct basic_host
      : stl::vector<StringType, rebind_allocator<typename StringType::allocator_type, StringType>> {
        using string_type = StringType;
        using container_type =
          stl::vector<string_type, rebind_allocator<typename string_type::allocator_type, string_type>>;
        using vector_type = container_type;

        using iterator       = typename container_type::iterator;
        using const_iterator = typename container_type::const_iterator;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        template <typename... T>
        explicit constexpr basic_host(T&&... args) : container_type{stl::forward<T>(args)...} {}

        template <istl::StringViewifiable StrT>
        constexpr basic_host& operator=(StrT&& inp_str) {
            auto const str =
              istl::stringify_of<string_type>(stl::forward<StrT>(inp_str), this->get_allocator());
            // todo: split it based on the domains
            this->push_back(str);
            return *this;
        }

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void set_raw_value(iterator beg, iterator end) {
            this->clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(*this, beg, end, this->get_allocator());
            } else {
                istl::emplace_one(*this, beg, end);
            }
        }

        /**
         * @brief check if we have value
         * @return true if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !this->empty() && !(this->size() == 1 && this->front().empty());
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
