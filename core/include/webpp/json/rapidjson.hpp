// Created by moisrex on 7/3/21.

#ifndef WEBPP_RAPIDJSON_HPP
#define WEBPP_RAPIDJSON_HPP

#if __has_include(<rapidjson/document.h>)
#    define WEBPP_RAPIDJSON_READY
#    include "../std/string.hpp"
#    include "../std/string_view.hpp"

#    include <rapidjson/document.h>


namespace webpp::json::rapidjson {
    using namespace ::rapidjson;

    template <Traits TraitsType>
    struct value : ::rapidjson::Value {
        using traits_type          = TraitsType;
        using rapidjson_value_type = ::rapidjson::Value;
        using string_type          = traits::general_string<traits_type>;
        using string_view_type     = traits::string_view<traits_type>;

        template <typename T>
        [[nodiscard]] bool is() const {
            return rapidjson_value_type::Is<T>();
        }

#    define WEBPP_IS_METHOD(real_type, type_name, is_func, get_func, set_func) \
        [[nodiscard]] bool is_##type_name() const {                            \
            return rapidjson_value_type::is_func();                            \
        }                                                                      \
                                                                               \
        value& set_##type_name(real_type const& val) {                         \
            rapidjson_value_type::set_func(val);                               \
            return *this;                                                      \
        }                                                                      \
                                                                               \
        value& set_##type_name(real_type&& val) {                              \
            rapidjson_value_type::set_func(stl::move(val));                    \
            return *this;                                                      \
        }                                                                      \
                                                                               \
        [[nodiscard]] real_type get_##type_name() const {                      \
            return rapidjson_value_type::get_func();                           \
        }

#    define WEBPP_IS_OPERATOR(real_type, type_name) \
        [[nodiscard]] operator real_type() const {  \
            return get_##type_name();               \
        }

        // WEBPP_IS_METHOD(null, IsNull, SetNull)
        WEBPP_IS_METHOD(bool, bool, IsBool, GetBool, SetBool)
        WEBPP_IS_METHOD(int, int, IsInt, GetInt, SetInt)
        WEBPP_IS_METHOD(long, long, IsInt64, GetInt64, SetInt64)
        WEBPP_IS_METHOD(long long, long_long, IsInt64, GetInt64, SetInt64)
        WEBPP_IS_METHOD(double, double, IsDouble, GetDouble, SetDouble)
        WEBPP_IS_METHOD(float, float, IsFloat, GetFloat, SetFloat)
        WEBPP_IS_METHOD(uint16_t, uint, IsUint, GetUint, SetUint)
        WEBPP_IS_METHOD(uint64_t, uint64, IsUint64, GetUint64, SetUint64)
        // WEBPP_IS_METHOD(stl::string, string, IsString, GetString, SetString)

        WEBPP_IS_OPERATOR(bool, bool)
        WEBPP_IS_OPERATOR(int, int)
        WEBPP_IS_OPERATOR(long, long)
        WEBPP_IS_OPERATOR(long long, long_long)
        WEBPP_IS_OPERATOR(double, double)
        WEBPP_IS_OPERATOR(float, float)
        WEBPP_IS_OPERATOR(uint16_t, uint)
        WEBPP_IS_OPERATOR(uint64_t, uint64)
        WEBPP_IS_OPERATOR(string_type, string)

        // set here has no values!
        // todo: make custom Array and Object structs and don't rely on rapidjson
        WEBPP_IS_METHOD(rapidjson_value_type::Array, array, IsArray, GetArray, SetArray)
        WEBPP_IS_METHOD(rapidjson_value_type::Object, object, IsObject, GetObject, SetObject)
        // WEBPP_IS_METHOD(number, IsNumber, GetNumber, SetNumber)
        // WEBPP_IS_METHOD(true, IsTrue)
        // WEBPP_IS_METHOD(false, IsFalse)

#    undef WEBPP_IS_METHOD
#    undef WEBPP_IS_OPERATOR


        string_type get_string() const {
            return string_type{rapidjson_value_type::GetString(), rapidjson_value_type::GetStringLength()};
        }

        value& set_string(string_view_type str) {
            // todo: use allocator if possible
            rapidjson_value_type::SetString(str.data(), str.size());
            return *this;
        }
    };

    template <Traits TraitsType>
    struct document : Document {
        using traits_type            = TraitsType;
        using string_view_type       = traits::string_view<traits_type>;
        using char_type              = traits::char_type<traits_type>;
        using general_allocator_type = traits::general_allocator<traits_type, char_type>;
        using value_type             = value<traits_type>;

        document() = default;

        // implement the parse method
        template <istl::StringViewifiable StrT>
        void parse(StrT&& json_string) {
            const auto json_str_view = istl::string_viewify(stl::forward<StrT>(json_string));
            this->Parse(json_str_view.data(), json_str_view.size());
        }
    };

} // namespace webpp::json::rapidjson

#endif

#endif // WEBPP_RAPIDJSON_HPP
