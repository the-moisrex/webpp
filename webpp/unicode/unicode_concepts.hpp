// Created by moisrex on 4/14/24.

#ifndef WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
#define WEBPP_UNICODE_UNICODE_CONCEPTS_HPP

namespace webpp::unicode {

    template <typename T>
    concept UTF8 = (sizeof(T) == sizeof(char8_t));

    template <typename T>
    concept UTF16 = (sizeof(T) == sizeof(char16_t));

    template <typename T>
    concept WChar = (sizeof(T) >= sizeof(char32_t));

    template <typename T>
    concept UTF32 = WChar<T>;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
