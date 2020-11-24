// Created by moisrex on 11/12/20.

#ifndef WEBPP_COMMON_PCH_HPP
#define WEBPP_COMMON_PCH_HPP

#if __has_include(<gtest/gtest.h>)
#   include <gtest/gtest.h>
#else
#   error "We don't have access to google test library (gtest)"
#endif

#ifndef WEBPP_PROJECT_ROOT
#define WEBPP_PROJECT_ROOT "../../"
#endif

#define webpp_include(file_path) <WEBPP_PROJECT_ROOT/core/include/webpp/file_path.hpp>


#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <cctype>
#include <array>
#include <vector>
#include <string_view>
#include <array>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory_resource>
#include <limits>
#include <map>

#endif // WEBPP_COMMON_PCH_HPP
