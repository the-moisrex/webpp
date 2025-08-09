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
        str.push_back(chars[i % chars.size()]);
    }
    std::shuffle(str.begin(), str.end(), std::mt19937(std::random_device()()));
    return str.substr(0, size);
}

static std::u8string str8_generator(std::size_t size = 10'000) {
    std::u8string str;
    str.reserve(size);

    std::mt19937 generator(std::random_device{}());

    for (std::size_t i = 0; i < size; ++i) {
        // Generate a random code point
        std::uniform_int_distribution<uint32_t> distribution(0, 0x10'FFFF); // Unicode range

        char32_t const code_point = distribution(generator);

        // Skip invalid or non-characters
        if ((code_point >= 0xD800 && code_point <= 0xDFFF) || (code_point == 0xFFFE) || (code_point == 0xFFFF)) {
            --i; // Decrement i to try again
            continue;
        }

        // Convert code point to UTF-8
        if (code_point <= 0x7F) {          // 1-byte sequence
            str += static_cast<char8_t>(code_point);
        } else if (code_point <= 0x7FF) {  // 2-byte sequence
            str += static_cast<char8_t>(0xC0 | (code_point >> 6));
            str += static_cast<char8_t>(0x80 | (code_point & 0x3F));
        } else if (code_point <= 0xFFFF) { // 3-byte sequence
            str += static_cast<char8_t>(0xE0 | (code_point >> 12));
            str += static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
            str += static_cast<char8_t>(0x80 | (code_point & 0x3F));
        } else { // 4-byte sequence
            str += static_cast<char8_t>(0xF0 | (code_point >> 18));
            str += static_cast<char8_t>(0x80 | ((code_point >> 12) & 0x3F));
            str += static_cast<char8_t>(0x80 | ((code_point >> 6) & 0x3F));
            str += static_cast<char8_t>(0x80 | (code_point & 0x3F));
        }
    }

    return str;
}

static std::u32string str32_generator(std::size_t size = 10'000) {
    std::u32string str;
    str.reserve(size);

    std::mt19937 generator(std::random_device{}());

    for (std::size_t i = 0; i < size; ++i) {
        // Generate a random code point
        std::uniform_int_distribution<char32_t> distribution(0, 0x10'FFFF); // Unicode range

        char32_t const code_point = distribution(generator);

        // Skip invalid or non-characters
        if ((code_point >= 0xD800 && code_point <= 0xDFFF) || (code_point == 0xFFFE) || (code_point == 0xFFFF)) {
            --i; // Decrement i to try again
            continue;
        }

        str.push_back(code_point);
    }

    return str;
}

template <std::size_t count>
static std::array<std::string, count> str8_array_generator(std::size_t const size = 10'000) {
    std::array<std::string, count> strs;
    for (auto& str : strs) {
        str = str8_generator(size);
    }
    return strs;
}

template <std::size_t count>
static std::array<std::u32string, count> str32_array_generator(std::size_t const size = 10'000) {
    std::array<std::u32string, count> strs;
    for (auto& str : strs) {
        str = str32_generator(size);
    }
    return strs;
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
