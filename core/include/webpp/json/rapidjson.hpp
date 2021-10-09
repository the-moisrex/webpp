// Created by moisrex on 7/3/21.

#ifndef WEBPP_RAPIDJSON_HPP
#define WEBPP_RAPIDJSON_HPP

#if __has_include(<rapidjson/document.h>)
#    define WEBPP_RAPIDJSON_READY
#    include "../std/string.hpp"
#    include "../std/string_view.hpp"
#    include "../traits/default_traits.hpp"

#    include <filesystem>

#    define RAPIDJSON_HAS_STDSTRING 1 // enable std::string support for rapidjson (todo: do we need it?)
#    include <rapidjson/document.h>


namespace webpp::json::rapidjson {
    // using namespace ::rapidjson;

    /**
     * todo: add a choise to use rapidjson's allocator
     * todo: use traits_type's allocator correctly if possible
     */
    namespace details {

        /**
         * This is a json object which means it can hold a key/value pair of value objects.
         */
        template <JSONValue ValueType>
        struct generic_object : public ValueType {
            using value_type          = ValueType;
            using traits_type         = typename value_type::traits_type;
            using key_type            = value_type; // both key and value types are the same
            using key_value_pair_type = stl::pair<key_type, value_type>;

            [[nodiscard]] key_type key() {}

            [[nodiscard]] value_type value() {}

            [[nodiscard]] key_value_pair_type key_value() {
                return {key(), value()};
            }

            [[nodiscard]] operator key_value_pair_type() {
                return key_value();
            }
        };

        template <Traits TraitsType, typename ValueType>
        struct generic_value {
            using traits_type          = TraitsType;
            using rapidjson_value_type = ::rapidjson::Value;
            using string_type          = traits::general_string<traits_type>;
            using string_view_type     = traits::string_view<traits_type>;
            using char_type            = traits::char_type<traits_type>;
            using value_type           = ValueType;
            using value_ref            = stl::add_lvalue_reference_t<value_type>; // add & to obj
            using value_ref_holder     = generic_value<traits_type, value_ref>;   // ref holder
            using object_type          = generic_object<value_type>;

          protected:
            value_type val_handle{};

          public:
            generic_value() = default;
            generic_value(value_ref obj) : val_handle{obj} {}

            template <typename T>
            [[nodiscard]] bool is() const {
                return val_handle.template Is<T>();
            }

#    define WEBPP_IS_METHOD(real_type, type_name, is_func, get_func, set_func) \
        [[nodiscard]] bool is_##type_name() const {                            \
            return val_handle.is_func();                                       \
        }                                                                      \
                                                                               \
        generic_value& set_##type_name(real_type const& val) {                 \
            val_handle.set_func(val);                                          \
            return *this;                                                      \
        }                                                                      \
                                                                               \
        generic_value& set_##type_name(real_type&& val) {                      \
            val_handle.set_func(stl::move(val));                               \
            return *this;                                                      \
        }                                                                      \
                                                                               \
        [[nodiscard]] real_type as_##type_name() const {                       \
            return val_handle.get_func();                                      \
        }


#    define WEBPP_IS_OPERATOR(real_type, type_name) \
        [[nodiscard]] operator real_type() const {  \
            return as_##type_name();                \
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
            WEBPP_IS_OPERATOR(string_view_type, string_view)

            // set here has no values!
            // todo: make custom Array and Object structs and don't rely on rapidjson
            // WEBPP_IS_METHOD(rapidjson_value_type::Array, array, IsArray, GetArray, SetArray)
            // WEBPP_IS_METHOD(rapidjson_value_type::Object, object, IsObject, GetObject, SetObject)
            // WEBPP_IS_METHOD(number, IsNumber, GetNumber, SetNumber)
            // WEBPP_IS_METHOD(true, IsTrue)
            // WEBPP_IS_METHOD(false, IsFalse)

#    undef WEBPP_IS_METHOD
#    undef WEBPP_IS_OPERATOR



            string_type as_string() const {
                return string_type{val_handle.GetString(), val_handle.GetStringLength()};
            }

            string_view_type as_string_view() const {
                return string_view_type{val_handle.GetString(), val_handle.GetStringLength()};
            }

            generic_value& set_string(string_view_type str) {
                // todo: use allocator if possible
                val_handle.SetString(str.data(), str.size());
                return *this;
            }


            template <typename T>
            [[nodiscard]] value_ref_holder operator[](T&& val) {
                return {val_handle[stl::forward<T>(val)]};
            }


            //            template <stl::size_t N>
            //            [[nodiscard]] auto operator[](char_type const child_name[N]) {
            //                return
            //                obj_handle.operator[](::rapidjson::GenericStringRef<char_type>(child_name,
            //                N));
            //            }

            //            template <stl::size_t N>
            //            [[nodiscard]] auto operator[](char_type const child_name[N]) const {
            //                using value_type =
            //                  stl::remove_cvref_t<decltype(rapidjson_value_type::operator[](child_name))>;
            //                using new_value_type = stl::add_cv_t<general_value<traits_type, value_type>>;
            //                return new_value_type{rapidjson_value_type::operator[](StringRef(child_name))};
            //            }

#    define RENAME(ret_type, orig_name, new_name, details) \
        ret_type new_name() details {                      \
            return val_handle.orig_name();                 \
        }

            RENAME(stl::size_t, Size, size, const);
            RENAME(bool, Empty, empty, const);
            RENAME(stl::size_t, Capacity, capacity, const);
            RENAME(void, Clear, clear, );
            RENAME(object_type, GetObject, as_object, );

#    undef RENAME
        };



    } // namespace details

    template <Traits TraitsType = default_traits>
    using value = details::generic_value<TraitsType, ::rapidjson::Value>;

    template <Traits TraitsType = default_traits>
    struct document : public details::generic_value<TraitsType, ::rapidjson::Document> {
        using traits_type             = TraitsType;
        using string_view_type        = traits::string_view<traits_type>;
        using char_type               = traits::char_type<traits_type>;
        using general_allocator_type  = traits::general_allocator<traits_type, char_type>;
        using value_type              = value<traits_type>;
        using rapidjson_document_type = details::generic_value<traits_type, ::rapidjson::Document>;

        /**
         * A document containing null
         */
        document() = default;

        /**
         * Get the file and parse it.
         */
        document(stl::filesystem::path file_path) {
            // todo
        }

        /**
         * Parse the json string specified here
         */
        template <istl::StringViewifiable StrT>
        document(StrT&& json_string) {
            parse(stl::forward<StrT>(json_string));
        }

        /**
         * A document containing the specified, already parsed, value
         */
        document(value_type&& val) : rapidjson_document_type{.obj_handle = val} {}

        // implement the parse method
        template <istl::StringViewifiable StrT>
        document& parse(StrT&& json_string) {
            const auto json_str_view = istl::string_viewify(stl::forward<StrT>(json_string));
            this->val_handle.Parse(json_str_view.data(), json_str_view.size());
            return *this;
        }
    };

} // namespace webpp::json::rapidjson

#endif

#endif // WEBPP_RAPIDJSON_HPP
