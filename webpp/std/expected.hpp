// Created by moisrex on 2/26/23.

#ifndef WEBPP_STD_EXPECTED_HPP
#define WEBPP_STD_EXPECTED_HPP

#include "./std.hpp"

#if __has_include(<expected>) && defined(__cpp_lib_expected)
#    include <expected>
#else
#    include "./expected_impl.hpp"
#endif

#endif // WEBPP_STD_EXPECTED_HPP
