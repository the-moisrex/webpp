// Created by moisrex on 11/12/20.

#ifndef WEBPP_COMMON_PCH_HPP
#define WEBPP_COMMON_PCH_HPP

#if __has_include(<gtest/gtest.h>)
#   include <gtest/gtest.h>
#else
#   error "We don't have access to google test library (gtest)"
#endif

#endif // WEBPP_COMMON_PCH_HPP
