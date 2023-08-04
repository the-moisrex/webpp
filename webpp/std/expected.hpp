// Created by moisrex on 2/26/23.

#ifndef WEBPP_STD_EXPECTED_HPP
#define WEBPP_STD_EXPECTED_HPP

#include "./std.hpp"

#if __has_include(<expected>) && __cplusplus > 202002L && __cpp_concepts >= 202002L
#    include <expected>
#else
#    include "./expected_impl.hpp"
#endif

#endif // WEBPP_STD_EXPECTED_HPP
