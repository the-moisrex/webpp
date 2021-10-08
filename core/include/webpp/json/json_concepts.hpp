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


    /**
     * This is a JSON Object,
     * Object could be a json key or a json value.
     * Of course the implementation is different for keys and values.
     * For example, iterating over a json key which is an integer, is just doesn't have a meaning.
     */
    template <typename T>
    concept JSONObject = requires(T obj) {
        { obj.size() } -> stl::same_as<stl::size_t>;
        { obj.begin() } -> JsonIterator;
        { obj.end() } -> JsonIterator;
        { obj.cbegin() } -> JsonIterator;
        { obj.cend() } -> JsonIterator;
        { obj.is_null() } -> stl::same_as<bool>;
        obj.clear();

        // Structured binding helper:
        //   for (auto [key, value] : doc);
        { obj.operator stl::pair<T, T>() } -> stl::same_as<stl::pair<T, T>>;
        { obj.key_value() } -> stl::same_as<stl::pair<T, T>>; // with explicit function name

            // todo: find, clear, ... methods

#define WEBPP_IS_METHOD(name) \
    { obj.is_##name() } -> stl::same_as<bool>;

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
#define WEBPP_AS_METHOD(type)                          \
    { obj.template as<type>() } -> stl::same_as<type>; \
    { obj.as_##type() } -> stl::same_as<type>;

        WEBPP_AS_METHOD(bool)
        WEBPP_AS_METHOD(char)
        WEBPP_AS_METHOD(short)
        WEBPP_AS_METHOD(int)
        WEBPP_AS_METHOD(long)
        // WEBPP_AS_METHOD(long long)
        WEBPP_AS_METHOD(double)
        // WEBPP_AS_METHOD(long double)
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
    concept JSONDocument = requires(T doc) {
        requires JSONObject<T>;
        doc.parse("{}");
        doc.pretty_string();
        doc.uglified_string();
        doc.template to_string<stl::string>(stl::allocator<char>());
        { doc[0] } -> JSONObject;
        { doc["key"] } -> JSONObject;
        { doc.key() } -> JSONObject;
    };

} // namespace webpp

#endif // WEBPP_JSON_CONCEPTS_HPP
