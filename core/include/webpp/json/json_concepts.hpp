// Created by moisrex on 9/9/20.

#ifndef WEBPP_JSON_CONCEPTS_HPP
#define WEBPP_JSON_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/iterator.hpp"

namespace webpp {

    template <typename T>
    concept JsonIterator = requires(T it) {
        requires stl::random_access_iterator<T>;
    };


    template <typename T>
    concept JsonValue = requires(T val) {
        { val.size() } -> stl::same_as<stl::size_t>;
        { val.begin() } -> JsonIterator;
        { val.end() } -> JsonIterator;
        { val.cbegin() } -> JsonIterator;
        { val.cend() } -> JsonIterator;
        { val.is_empty() } -> stl::same_as<bool>;
        val.clear();
        // todo: find, clear, ... methods

#define WEBPP_IS_METHOD(name) \
    { val.is_##name() } -> stl::same_as<bool>;

        WEBPP_IS_METHOD(null)
        WEBPP_IS_METHOD(bool)
        // WEBPP_IS_METHOD(char)
        // WEBPP_IS_METHOD(short)
        WEBPP_IS_METHOD(int)
        WEBPP_IS_METHOD(long)
        // WEBPP_IS_METHOD(long_long)
        WEBPP_IS_METHOD(double)
        // WEBPP_IS_METHOD(long_double)
        WEBPP_IS_METHOD(float)
        // WEBPP_IS_METHOD(int8)
        // WEBPP_IS_METHOD(int16)
        // WEBPP_IS_METHOD(int32)
        // WEBPP_IS_METHOD(int64)
        WEBPP_IS_METHOD(uint)
        // WEBPP_IS_METHOD(uint8)
        // WEBPP_IS_METHOD(uint16)
        // WEBPP_IS_METHOD(uint32)
        // WEBPP_IS_METHOD(uint64)
        WEBPP_IS_METHOD(string)
        WEBPP_IS_METHOD(array)
        WEBPP_IS_METHOD(object)
        WEBPP_IS_METHOD(number)

#undef WEBPP_IS_METHOD

        // todo: should we add stl::optional<...> ?
#define WEBPP_AS_METHOD(type) \
    { val.template as<type>() } -> stl::same_as<type>;

        WEBPP_AS_METHOD(bool)
        WEBPP_AS_METHOD(char)
        WEBPP_AS_METHOD(short)
        WEBPP_AS_METHOD(int)
        WEBPP_AS_METHOD(long)
        WEBPP_AS_METHOD(long long)
        WEBPP_AS_METHOD(double)
        WEBPP_AS_METHOD(long double)
        WEBPP_AS_METHOD(float)
        WEBPP_AS_METHOD(int8_t)
        WEBPP_AS_METHOD(int16_t)
        WEBPP_AS_METHOD(int32_t)
        WEBPP_AS_METHOD(int64_t)
        WEBPP_AS_METHOD(unsigned)
        WEBPP_AS_METHOD(uint8_t)
        WEBPP_AS_METHOD(uint16_t)
        WEBPP_AS_METHOD(uint32_t)
        WEBPP_AS_METHOD(uint64_t)
        // todo: add string, vector, list, ...

#undef WEBPP_AS_METHOD
    };

    template <typename T>
    concept JsonDocument = requires(T doc) {
        requires JsonObject<T>;
        doc.parse("{}");
        doc.pretty_string();
        doc.uglified_string();
        doc.template to_string<stl::string>(stl::allocator<char>());
        { doc[0] } -> JsonValue;
        { doc["key"] } -> JsonValue;
        { doc.key() } -> JsonValue;
    };

} // namespace webpp

#endif // WEBPP_JSON_CONCEPTS_HPP
