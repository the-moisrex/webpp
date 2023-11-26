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
     * Common JSON Value methods.
     */
    template <typename T>
    concept JSONCommon = requires(T val) {

#define AS_METHOD(name, type)   \
    {                           \
        val.template is<type>() \
    } -> stl::same_as<bool>;    \
    {                           \
        val.is_##name()         \
    } -> stl::same_as<bool>;
        AS_METHOD(bool, bool)
        // AS_METHOD(char, char)
        // todo: add char8_t
        AS_METHOD(double, double)
        // AS_METHOD(long_double, long double)
        AS_METHOD(float, float)
        AS_METHOD(int8, stl::int8_t)
        AS_METHOD(int16, stl::int16_t)
        AS_METHOD(int32, stl::int32_t)
        AS_METHOD(int64, stl::int64_t)
        AS_METHOD(uint8, stl::uint8_t)
        AS_METHOD(uint16, stl::uint16_t)
        AS_METHOD(uint32, stl::uint32_t)
        AS_METHOD(uint64, stl::uint64_t)

#undef AS_METHOD
    };


    /**
     * This is a json array type. It's not a C++ std::array; it's more of a std::vector but it's called
     * array because JavaScript likes to call list, array :)
     */
    template <typename T>
    concept JSONArray = requires { typename T::value_type; } and requires(T arr, typename T::value_type val) {
        {
            arr[0]
        } -> JSONCommon; // just because we can't use JSONValue here

        {
            arr.begin()
        } -> stl::random_access_iterator;
        {
            arr.end()
        } -> stl::random_access_iterator;
        {
            arr.cbegin()
        } -> stl::random_access_iterator;
        {
            arr.cend()
        } -> stl::random_access_iterator;

        {
            arr.size()
        } -> stl::same_as<stl::size_t>;
        {
            arr.capacity()
        } -> stl::same_as<stl::size_t>;

        arr.push_back(val); // push_back is so vector like, so we use that instead of the other possible names
        arr.emplace_back(val);
        arr.clear();

        // todo: add erase methods
    };

    /**
     * @brief this is a custom std::pair or a tuple essentially.
     * It's here to make structure binding work.
     * @param pair
     */
    template <typename T>
    concept JSONPair = requires(T pair) {
        {
            pair.key
        } -> JSONCommon; // JSONKey in fact
        {
            pair.value
        } -> JSONCommon; // JSONValue in fact, but we can't use it here
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
        {
            obj.begin()
        } /* -> stl::random_access_iterator */;
        {
            obj.end()
        } /* -> stl::random_access_iterator */;
        {
            obj.cbegin()
        } /* -> stl::random_access_iterator */;
        {
            obj.cend()
        } /* -> stl::random_access_iterator */;

        {
            obj.size()
        } -> stl::same_as<stl::size_t>;

        obj.insert("key", "value");  // push_back is so vector like, Object is more like a std::map
        obj.emplace("key", "value"); // todo: add allocator support maybe?
        obj["key"] = "value";
        obj.clear();

        {
            obj.contains("member")
        } -> stl::same_as<bool>; // the input is of type JSONKey
    };

    /**
     * Check if it's a json key
     */
    template <typename T>
    concept JSONKey = JSONString<stl::remove_cvref_t<T>> || JSONNumber<stl::remove_cvref_t<T>>;

    /**
     * This is a JSON Value,
     * Object could be a json key or a json value.
     * Of course the implementation is different for keys and values.
     * For example, iterating over a json key which is an integer, is just doesn't have a meaning.
     */
    template <typename T>
    concept JSONValue = requires(T val) {
        requires JSONCommon<T>;

        // requires JSONObject<T>; // using JSONObject as the default. If the users want,
        // they can use .as_array() function to get the same thing as an array
        {
            val.is_null()
        } -> stl::same_as<bool>;

        // object related methods
        {
            val.is_object()
        } -> stl::same_as<bool>;
        {
            val.as_object()
        } -> JSONObject;

        // array related methods
        {
            val.is_array()
        } -> stl::same_as<bool>;
        val.as_array(); // JSONArray

        // string related methods
        {
            val.is_string()
        } -> stl::same_as<bool>;
        {
            val.as_string_view()
        } -> istl::StringView;
        {
            val.template as_string<stl::string>(stl::allocator<char>())
        } -> istl::String;

        // todo: add default value as<...> functions
#define AS_METHOD(name, type)   \
    {                           \
        val.template as<type>() \
    } -> stl::same_as<type>;    \
    {                           \
        val.as_##name()         \
    } -> stl::same_as<type>;

        AS_METHOD(bool, bool)
        // AS_METHOD(char, char)
        // todo: add char8_t
        AS_METHOD(double, double)
        // AS_METHOD(long_double, long double)
        AS_METHOD(float, float)
        AS_METHOD(int8, stl::int8_t)
        AS_METHOD(int16, stl::int16_t)
        AS_METHOD(int32, stl::int32_t)
        AS_METHOD(int64, stl::int64_t)
        AS_METHOD(uint8, stl::uint8_t)
        AS_METHOD(uint16, stl::uint16_t)
        AS_METHOD(uint32, stl::uint32_t)
        AS_METHOD(uint64, stl::uint64_t)

#undef AS_METHOD
    };

    template <typename T>
    concept PotentialJSONValue =
      JSONValue<T> || JSONString<T> || JSONNumber<T> || JSONArray<T> || JSONObject<T>;

    /**
     * JSON Document will contain a JSON Document
     */
    template <typename T>
    concept JSONDocument = requires(T doc) {
        requires JSONValue<T>;
        doc.parse("{}");
        T{"{}"};                // parse
        T{};                    // will contain a null value
        T{T{"{}"}.as_object()}; // passing an object/value as input
        // T{stl::filesystem::path{"file.json"}}; // read from file.
        {
            doc.pretty()
        } -> istl::String;
        {
            doc.uglified()
        } -> istl::String;
    };

} // namespace webpp::json

#endif // WEBPP_JSON_CONCEPTS_HPP
