// Created by moisrex on 11/12/20.

#ifndef WEBPP_COMMON_PCH_HPP
#define WEBPP_COMMON_PCH_HPP

#if __has_include(<gtest/gtest.h>)
#    include <gtest/gtest.h>
#else
#    error "We don't have access to google test library (gtest)"
#endif

#ifndef WEBPP_PROJECT_ROOT
#    define WEBPP_PROJECT_ROOT "../../"
#endif

#define webpp_include(file_path) <WEBPP_PROJECT_ROOT/core/include/webpp/file_path.hpp>


#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>

#endif // WEBPP_COMMON_PCH_HPP
