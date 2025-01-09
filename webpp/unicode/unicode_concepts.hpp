// Created by moisrex on 4/14/24.

#ifndef WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
#define WEBPP_UNICODE_UNICODE_CONCEPTS_HPP

namespace webpp::unicode {

    template <typename T>
    concept UTF8 = (sizeof(T) == sizeof(char8_t));

    template <typename T>
    concept UTF16 = (sizeof(T) == sizeof(char16_t));

    template <typename T>
    concept WChar = (sizeof(T) >= sizeof(wchar_t));

    template <typename T>
    concept UTF32 = (sizeof(T) >= sizeof(wchar_t));

    template <typename T>
    concept UTF = UTF8<T> || UTF16<T> || UTF32<T>;

    template <typename T>
    concept UTF8String = UTF8<typename T::value_type>;

    template <typename T>
    concept UTF16String = UTF16<typename T::value_type>;

    template <typename T>
    concept UTF32String = UTF32<typename T::value_type>;

} // namespace webpp::unicode

#endif // WEBPP_UNICODE_UNICODE_CONCEPTS_HPP
