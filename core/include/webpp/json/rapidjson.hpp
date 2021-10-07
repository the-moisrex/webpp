// Created by moisrex on 7/3/21.

#ifndef WEBPP_RAPIDJSON_HPP
#define WEBPP_RAPIDJSON_HPP

#if __has_include(<rapidjson/document.h>)
#    define WEBPP_RAPIDJSON_READY
#    include "../std/string_view.hpp"

#    include <rapidjson/document.h>


namespace webpp::json::rapidjson {
    using namespace ::rapidjson;

    template <Traits TraitsType>
    struct value : ::rapidjson::Value {
        using traits_type          = TraitsType;
        using rapidjson_value_type = ::rapidjson::Value;

        template <typename T>
        [[nodiscard]] bool is() const {
            return rapidjson_value_type::Is<T>();
        }

#    define WEBPP_IS_METHOD(type, is_func, get_func, set_func) \
        [[nodiscard]] bool is_##type() const {                 \
            return rapidjson_value_type::is_func();            \
        }                                                      \
                                                               \
        void set_##type(type const& val) {                     \
            rapidjson_value_type::set_func(val);               \
        }                                                      \
                                                               \
        void set_##type(type&& val) {                          \
            rapidjson_value_type::set_func(stl::move(val));    \
        }                                                      \
                                                               \
        [[nodiscard]] type get_##type() const {                \
            return rapidjson_value_type::get_func();           \
        }                                                      \
                                                               \
        [[nodiscard]] operator type() const {                  \
            return rapidjson_value_type::get_func();           \
        }

        // WEBPP_IS_METHOD(null, IsNull, SetNull)
        WEBPP_IS_METHOD(bool, IsBool, GetBool, SetBool)
        WEBPP_IS_METHOD(int, IsInt, GetInt, SetInt)
        WEBPP_IS_METHOD(long, IsInt64, GetInt64, SetInt64)
        WEBPP_IS_METHOD(long_long, IsInt64, GetInt64, SetInt64)
        WEBPP_IS_METHOD(double, IsDouble, GetDouble, SetDouble)
        WEBPP_IS_METHOD(float, IsFloat, GetFloat, SetFloat)
        WEBPP_IS_METHOD(uint, IsUint, GetUint, SetUint)
        WEBPP_IS_METHOD(uint64, IsUint64, GetUint64, SetUint64)
        WEBPP_IS_METHOD(string, IsString, GetString, SetString)

        // set here has no values!
        WEBPP_IS_METHOD(array, IsArray, GetArray, SetArray)
        WEBPP_IS_METHOD(object, IsObject, GetObject, SetObject)
        // WEBPP_IS_METHOD(number, IsNumber, GetNumber, SetNumber)
        // WEBPP_IS_METHOD(true, IsTrue)
        // WEBPP_IS_METHOD(false, IsFalse)

#    undef WEBPP_IS_METHOD
    };

    template <Traits TraitsType>
    struct document : Document {
        using traits_type            = TraitsType;
        using string_view_type       = traits::string_view<traits_type>;
        using char_type              = traits::char_type<traits_type>;
        using general_allocator_type = traits::general_allocator<traits_type, char_type>;
        using value_type             = value<traits_type>;

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
