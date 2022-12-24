// Created by moisrex on 12/23/22.

#ifndef WEBPP_HEADERS_HPP
#define WEBPP_HEADERS_HPP

#include "../std/tuple.hpp"

#include <algorithm>

namespace webpp::http {

    template <typename Container>
    struct headers_container : public Container {
        using container_type = Container;
        using field_type     = typename container_type::value_type;

        using Container::Container;

        /**
         * Check if the specified names are in headers
         * returns stl::tuple<bool, bool, ...> if you give multiple names
         * or
         * returns bool if you give one name
         */
        template <typename... NameType>
        [[nodiscard]] constexpr auto has(NameType&&... name) const noexcept {
            if constexpr (sizeof...(NameType) == 1) {
                return stl::find(this->begin(), this->end(), name...) != this->end();
            } else if constexpr (sizeof...(NameType) > 1) {
                auto tup = stl::make_tuple(((stl::ignore = name, false), ...));
                auto fillter =
                  [&tup, names = stl::forward_as_tuple(stl::forward<NameType>(name)...)]<stl::size_t... I>(
                    field_type const& field,
                    stl::index_sequence<I...>) constexpr noexcept {
                    ((field == stl::get<I>(names) && (stl::get<I>(tup) = true)), ...);
                };
                for (const auto& field : *this) {
                    filler(field, stl::make_index_sequence<sizeof...(NameType)>{});
                }
                return tup;
            } else {
                return true;
            }
        }
    };

} // namespace webpp::http

#endif // WEBPP_HEADERS_HPP
