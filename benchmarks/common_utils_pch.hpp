// Created by moisrex on 9/23/20.

#ifndef WEBPP_COMMON_URILS_PCH_H
#define WEBPP_COMMON_URILS_PCH_H

#include <algorithm>
#include <random>
#include <string>

template <typename StrType = std::string>
static StrType str_generator(
  std::size_t      size  = 10'000,
  std::string_view chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
    StrType str;
    str.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        str.append(chars);
    }
    std::shuffle(str.begin(), str.end(), std::mt19937(std::random_device()()));
    return str.substr(0, size);
}

template <std::size_t count>
static std::array<std::string, count> str_array_generator(
  std::size_t      size  = 10'000,
  std::string_view chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
    std::array<std::string, count> strs;
    for (auto& str : strs) {
        str = str_generator(size, chars);
    }
    return strs;
}

static std::vector<int> int_generator(std::size_t size = 10'000) {
    std::vector<int> ints;
    ints.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        ints.push_back(i);
    }
    std::shuffle(ints.begin(), ints.end(), std::mt19937(std::random_device()()));
    return ints;
}


#endif // WEBPP_COMMON_URILS_PCH_H
