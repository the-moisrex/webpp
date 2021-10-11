// Created by moisrex on 9/9/20.

#ifndef WEBPP_JSON_CONCEPTS_HPP
#define WEBPP_JSON_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/iterator.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp::json {

    /**
     * Number includes:
     *   - integer types
     *   - float, double
     *   - bool (even though in JavaScript it's a different type)
     */
    template <typename T>
    concept JSONNumber = stl::is_arithmetic_v<T>;

    /**
     * Honestly I accept any type that can be converted into a string or a string_view
     */
    template <typename T>
    concept JSONString = istl::StringViewifiable<T>;


    /**
     * This is a json array type. It's not a C++ std::array; it's more of a std::vector but it's called
     * array because JavaScript likes to call list, array :)
     */
    template <typename T>
    concept JSONArray = requires {
        typename T::value_type;
    }
    and requires(T arr, typename T::value_type val) {
        { arr.begin() } -> stl::random_access_iterator;
        { arr.end() } -> stl::random_access_iterator;
        { arr.cbegin() } -> stl::random_access_iterator;
        { arr.cend() } -> stl::random_access_iterator;

        { arr.size() } -> stl::same_as<stl::size_t>;

        arr.push_back(val); // push_back is so vector like, so we use that instead of the other possible names
        arr.emplace_back(val);
        arr.clear();
    };

    /**
     * This is a JSON Object
     */
    template <typename T>
    concept JSONObject = requires(T obj) {
        // Object should be definable without the help of Value. So I'm not gonna do "-> JSONValue" here.
        obj[0];
        obj["key"];

        // Iterator for this type should be of type: pair<key, value>
        // This is because it would allow structured binding to work in a for loop:
        //   for (auto [key, value] : doc.as_object());
        { obj.begin() } -> stl::random_access_iterator;
        { obj.end() } -> stl::random_access_iterator;
        { obj.cbegin() } -> stl::random_access_iterator;
        { obj.cend() } -> stl::random_access_iterator;

        { obj.size() } -> stl::same_as<stl::size_t>;

        obj.insert("value"); // push_back is so vector like, Object is more like a std::map
        obj.emplace("value");
        obj.clear();

        { obj.has("member") } -> stl::same_as<bool>; // the input is of type JSONKey
    };

    /**
     * Check if it's a json key
     */
    template <typename T>
    concept JSONKey = JSONString<T> || JSONNumber<T>;

    /**
     * This is a JSON Value,
     * Object could be a json key or a json value.
     * Of course the implementation is different for keys and values.
     * For example, iterating over a json key which is an integer, is just doesn't have a meaning.
     */
    template <typename T>
    concept JSONValue = requires(T val) {
        requires JSONObject<T>;
        { val.is_null() } -> stl::same_as<bool>;

        // object related methods
        { val.is_object() } -> stl::same_as<bool>;
        val.as_object(); // -> JSONObject, but it'll be a circular dependency

        // array related methods
        { val.is_array() } -> stl::same_as<bool>;
        val.as_array(); // JSONArray

        // string related methods
        { val.is_string() } -> stl::same_as<bool>;
        { val.as_string() } -> istl::String;
        { val.as_string_view() } -> istl::StringView;
        val.pretty_string();
        val.uglified_string();
        val.template to_string<stl::string>(stl::allocator<char>());

        // todo: find, clear, ... methods

#define WEBPP_IS_METHOD(name, type)                    \
    { val.template as<type>() } -> stl::same_as<type>; \
    { val.is_##name() } -> stl::same_as<bool>;         \
    { val.as_##name() } -> stl::same_as<type>;

        // WEBPP_IS_METHOD(null)
        WEBPP_IS_METHOD(bool, bool)
        // WEBPP_IS_METHOD(char)
        // WEBPP_IS_METHOD(short)
        WEBPP_IS_METHOD(int, int)
        WEBPP_IS_METHOD(long, long)
        // WEBPP_IS_METHOD(long_long)
        WEBPP_IS_METHOD(double, double)
        // WEBPP_IS_METHOD(long_double)
        WEBPP_IS_METHOD(float, float)
        // WEBPP_IS_METHOD(int8)
        // WEBPP_IS_METHOD(int16)
        // WEBPP_IS_METHOD(int32)
        // WEBPP_IS_METHOD(int64)
        WEBPP_IS_METHOD(uint, unsigned)
        // WEBPP_IS_METHOD(uint8)
        // WEBPP_IS_METHOD(uint16)
        // WEBPP_IS_METHOD(uint32)
        // WEBPP_IS_METHOD(uint64)
        // WEBPP_IS_METHOD(number)

#undef WEBPP_IS_METHOD

        // todo: should we add stl::optional<...> ?
#define WEBPP_AS_METHOD(type)                          \
    { val.template as<type>() } -> stl::same_as<type>; \
    { val.as_##name() } -> stl::same_as<type>;

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

#undef WEBPP_AS_METHOD
    };


    /**
     * JSON Document will contain a JSON Document
     */
    template <typename T>
    concept JSONDocument = requires(T doc) {
        requires JSONObject<T>;
        doc.parse("{}");
        T{"{}"};                // parse
        T{};                    // will contain a null value
        T{T{"{}"}.as_object()}; // passing an object/value as input
        // T{stl::filesystem::path{"file.json"}}; // read from file.
    };

} // namespace webpp::json

#endif // WEBPP_JSON_CONCEPTS_HPP
