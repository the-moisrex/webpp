// Created by moisrex on 2/26/23.

#ifndef WEBPP_STD_EXPECTED_HPP
#define WEBPP_STD_EXPECTED_HPP

#include "./std.hpp"

#if __has_include(<expected>) && __cplusplus > 202002L && __cpp_concepts >= 202002L
#    include <expected>
namespace webpp::istl {

    template <typename E>
    using bad_expected_access = std::bad_expected_access;

    using unexpect_t = std::unexpect_t;

    inline constexpr unexpect_t unexpect = std::unexpect;

    template <typename E>
    using unexpected = std::unexpected;

    template <typename E>
    unexpected(E) -> unexpected<E>;

    template <typename T, typename E>
    using expected = std::expected;

} // namespace webpp::istl
#else
#    include "./expected_impl.hpp"
#endif

#endif // WEBPP_STD_EXPECTED_HPP
