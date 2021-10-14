// Created by moisrex on 7/3/21.

#ifndef WEBPP_RAPIDJSON_HPP
#define WEBPP_RAPIDJSON_HPP

#if __has_include(<rapidjson/document.h>)
#    define WEBPP_RAPIDJSON_READY
#    include "../std/string.hpp"
#    include "../std/string_view.hpp"
#    include "../traits/default_traits.hpp"
#    include "json_concepts.hpp"

#    include <filesystem>

#    define RAPIDJSON_HAS_STDSTRING 1 // enable std::string support for rapidjson (todo: do we need it?)
#    include <rapidjson/document.h>
#    include <rapidjson/prettywriter.h>

#    if (RAPIDJSON_MAJOR_VERSION == 1 && RAPIDJSON_MINOR_VERSION == 1 && RAPIDJSON_PATCH_VERSION == 0)
namespace rapidjson {
    // a patch for issue: https://github.com/Tencent/rapidjson/pull/1947
    template <bool Const, typename Encoding, typename Allocator>
    GenericMemberIterator<Const, Encoding, Allocator>
    operator+(typename GenericMemberIterator<Const, Encoding, Allocator>::DifferenceType n,
              GenericMemberIterator<Const, Encoding, Allocator> const&                   j) {
        return j + n;
    }

    // This part doesn't need to be sent to rapidjson, it's already there
    // (https://github.com/the-moisrex/rapidjson/blob/f14d5097e51fc19582884a517699adef09edbff7/include/rapidjson/document.h#L262)
    template <bool Const, typename Encoding, typename Allocator>
    bool operator==(GenericMemberIterator<Const, Encoding, Allocator> const& n,
                    GenericMemberIterator<Const, Encoding, Allocator> const& j) {
        return n.operator->() == j.operator->();
    }
} // namespace rapidjson
#    endif

namespace webpp::json::rapidjson {
    // using namespace ::rapidjson;

    /**
     * todo: add a choice to use rapidjson's allocator
     * todo: use traits_type's allocator correctly if possible
     */
    namespace details {

        template <typename IteratorType>
        using generic_iterator = IteratorType;

        template <Traits TraitsType, typename ValueType>
        struct generic_value;

        template <Traits TraitsType, typename ArrayType>
        struct generic_array {
            using traits_type          = TraitsType;
            using rapidjson_array_type = ArrayType;

            generic_array(rapidjson_array_type& arr) : arr_handle{arr} {}

            [[nodiscard]] stl::size_t size() const {
                return arr_handle.Size();
            }

            [[nodiscard]] stl::size_t capacity() const {
                return arr_handle.Capacity();
            }


            auto begin() const {
                return arr_handle.Begin();
            }
            auto end() const {
                return arr_handle.End();
            }

            auto cbegin() const {
                return arr_handle.Begin();
            }
            auto cend() const {
                return arr_handle.End();
            }



          protected:
            rapidjson_array_type& arr_handle;
        };

        /**
         * Generic number will hold
         *   - json numeric values
         *   - booleans
         * It hold booleans too because it's not JavaScript and bools are still numbers! :)
         */
        template <Traits TraitsType, typename ValueType>
        struct generic_number {
            using rapidjson_value_type = ValueType;
            using traits_type          = TraitsType;
            using value_type           = generic_value<traits_type, rapidjson_value_type>;
            using string_view_type     = traits::string_view<traits_type>;

            generic_number(rapidjson_value_type& val) : val_handle{val} {}

            /**
             * Get the number as the specified numeric type
             */
            template <JSONNumber T>
            [[nodiscard]] T as() const {
                return val_handle.template Get<T>();
            }


            /**
             * Checks whether a number can be losslessly converted to a float.
             */
            [[nodiscard]] bool is_lossless_float() const {
                return val_handle.IsLosslessFloat();
            }


#    define WEBPP_GETTER(name, type)           \
        [[nodiscard]] operator type() const {  \
            return as<type>();                 \
        }                                      \
                                               \
        [[nodiscard]] bool as_##name() const { \
            return as<type>();                 \
        }


            WEBPP_GETTER(bool, bool)
            // WEBPP_GETTER(char)
            // WEBPP_GETTER(short)
            // WEBPP_GETTER(int, int)
            // WEBPP_GETTER(long, long)
            // WEBPP_GETTER(long_long)
            WEBPP_GETTER(double, double)
            // WEBPP_GETTER(long_double)
            WEBPP_GETTER(float, float)
            // WEBPP_GETTER(int8)
            // WEBPP_GETTER(uint, unsigned)
            WEBPP_GETTER(uint8, stl::uint8_t)
            WEBPP_GETTER(uint16, stl::uint16_t)
            WEBPP_GETTER(uint32, stl::uint32_t)
            WEBPP_GETTER(uint64, stl::uint64_t)
            WEBPP_GETTER(int8, stl::int8_t)
            WEBPP_GETTER(int16, stl::int16_t)
            WEBPP_GETTER(int32, stl::int32_t)
            WEBPP_GETTER(int64, stl::int64_t)
            // WEBPP_GETTER(number)

#    undef WEBPP_GETTER

          protected:
            rapidjson_value_type val_handle;
        };

        /**
         * This is a json object which means it can hold a key/value pair of value objects.
         * The ValueType is a rapidjson value type not a generic value type.
         */
        template <Traits TraitsType, typename ObjectType>
        // requires(istl::is_specialization_of_v<ObjectType, ::rapidjson::GenericObject>)
        struct generic_object {
            using rapidjson_object_type = ObjectType;
            using traits_type           = TraitsType;
            using value_type            = generic_value<traits_type, rapidjson_object_type>;
            using string_view_type      = traits::string_view<traits_type>;

            generic_object(rapidjson_object_type& obj) : obj_handle{obj} {}

            template <JSONKey KeyType>
            [[nodiscard]] value_type operator[](KeyType&& key) {
                if constexpr (JSONNumber<KeyType>) {
                    // fixme: write tests for this, this will run the "index" version, right?
                    return value_type::operator[](key);
                } else if constexpr (JSONString<KeyType>) {
                    // The key is convertible to string_view
                    auto const key_view =
                      istl::string_viewify_of<string_view_type>(stl::forward<KeyType>(key));
                    return obj_handle[::rapidjson::StringRef(key_view.data(), key_view.size())];
                }
            }

            generic_object& clear() {
                obj_handle.Clear();
                return *this;
            }

            [[nodiscard]] stl::size_t size() const noexcept {
                return obj_handle.MemberCount();
            }

            template <JSONKey KeyT, PotentialJSONValue ValT>
            generic_object& insert(KeyT&& key, ValT&& val) {
                auto const key_view = istl::string_viewify_of<string_view_type>(stl::forward<KeyT>(key));
                obj_handle.AddMember(::rapidjson::StringRef(key_view.data(), key_view.size()),
                                     stl::forward<ValT>(val));
                return *this;
            }

            template <JSONKey KeyT, PotentialJSONValue ValT>
            generic_object& emplace(KeyT&& key, ValT&& val) {
                return insert<KeyT, ValT>(stl::forward<KeyT>(key), stl::forward<ValT>(val));
            }

            static_assert(stl::random_access_iterator<typename rapidjson_object_type::MemberIterator>);
            auto begin() const {
                return obj_handle.MemberBegin();
            }
            auto end() const {
                return obj_handle.MemberEnd();
            }

            auto cbegin() const {
                return obj_handle.MemberBegin();
            }
            auto cend() const {
                return obj_handle.MemberEnd();
            }

            template <JSONKey KeyT>
            [[nodiscard]] bool contains(KeyT&& key) const {
                const auto key_view = istl::string_viewify_of<string_view_type>(stl::forward<KeyT>(key));
                return obj_handle.HasMember(key_view.data()); // fixme: not passing the length
            }

          protected:
            rapidjson_object_type obj_handle;
        };

        template <Traits TraitsType, typename ValueType>
        struct generic_value {
          private:
            // DocType could be a document or an GenericObject actually
            template <typename DocType>
            struct value_type_finder {
                using type = typename DocType::ValueType;
            };

          public:
            // finding the rapidjson's ValueType even if the ValueType is a ::rapidjson::Document type or a
            // ::rapidjson::GenericObject
            static constexpr bool has_ref = !stl::same_as<ValueType, stl::remove_cvref_t<ValueType>>;
            using value_type              = istl::lazy_conditional_t<
              (requires { typename stl::remove_cvref_t<ValueType>::ValueType; }),
              istl::templated_lazy_type<value_type_finder, stl::remove_cvref_t<ValueType>>,
              istl::lazy_type<stl::remove_cvref_t<ValueType>>>;
            using traits_type           = TraitsType;
            using string_type           = traits::general_string<traits_type>;
            using string_view_type      = traits::string_view<traits_type>;
            using char_type             = traits::char_type<traits_type>;
            using generic_value_type    = generic_value<traits_type, value_type>;
            using value_ref             = stl::add_lvalue_reference_t<value_type>; // add & to obj
            using auto_ref_value_type   = stl::conditional_t<has_ref, value_ref, value_type>;
            using value_ref_holder      = generic_value<traits_type, value_ref>; // ref holder
            using rapidjson_object_type = typename value_type::Object;
            using object_type           = generic_object<traits_type, rapidjson_object_type>;
            using rapidjson_array_type  = typename value_type::Array;
            using array_type            = generic_array<traits_type, rapidjson_array_type>;
            using generic_iterator_type =
              generic_iterator<typename stl::remove_cvref_t<value_type>::ValueIterator>;

          protected:
            auto_ref_value_type val_handle{};

          public:
            generic_value() = default;
            generic_value(value_ref obj) : val_handle{obj} {}

            template <typename T>
            [[nodiscard]] bool is() const {
                return val_handle.template Is<T>();
            }

            template <typename T>
            [[nodiscard]] T as() const {
                return val_handle.template Get<T>();
            }

            /**
             * Check if it has a member
             */
            [[nodiscard]] bool contains(string_view_type key) const {
                return val_handle.HasMember(::rapidjson::StringRef(key.data(), key.size()));
            }


#    define IS_METHOD(real_type, type_name, is_func, get_func, set_func) \
        [[nodiscard]] bool is_##type_name() const {                      \
            return val_handle.is_func();                                 \
        }                                                                \
                                                                         \
        generic_value& set_##type_name(real_type const& val) {           \
            val_handle.set_func(val);                                    \
            return *this;                                                \
        }                                                                \
                                                                         \
        generic_value& set_##type_name(real_type&& val) {                \
            val_handle.set_func(stl::move(val));                         \
            return *this;                                                \
        }                                                                \
                                                                         \
        [[nodiscard]] real_type as_##type_name() const {                 \
            return val_handle.get_func();                                \
        }


#    define WEBPP_IS_OPERATOR(real_type, type_name) \
        [[nodiscard]] operator real_type() const {  \
            return as_##type_name();                \
        }

            // WEBPP_IS_METHOD(null, IsNull, SetNull)
            IS_METHOD(bool, bool, IsBool, GetBool, SetBool)
            IS_METHOD(stl::int8_t, int8, IsInt, GetInt, SetInt)
            IS_METHOD(stl::int16_t, int16, IsInt, GetInt, SetInt)
            IS_METHOD(stl::int32_t, int32, IsInt, GetInt, SetInt)
            IS_METHOD(stl::int64_t, int64, IsInt, GetInt, SetInt)
            IS_METHOD(double, double, IsDouble, GetDouble, SetDouble)
            IS_METHOD(float, float, IsFloat, GetFloat, SetFloat)
            IS_METHOD(stl::uint8_t, uint8, IsUint, GetUint, SetUint)
            IS_METHOD(stl::uint16_t, uint16, IsUint, GetUint, SetUint)
            IS_METHOD(stl::uint32_t, uint32, IsUint, GetUint, SetUint)
            IS_METHOD(stl::uint64_t, uint64, IsUint64, GetUint64, SetUint64)
            // WEBPP_IS_METHOD(stl::string, string, IsString, GetString, SetString)

            WEBPP_IS_OPERATOR(bool, bool)
            WEBPP_IS_OPERATOR(stl::int8_t, int8)
            WEBPP_IS_OPERATOR(stl::int16_t, int16)
            WEBPP_IS_OPERATOR(stl::int32_t, int32)
            WEBPP_IS_OPERATOR(stl::int64_t, int64)
            WEBPP_IS_OPERATOR(stl::uint8_t, uint8)
            WEBPP_IS_OPERATOR(stl::uint16_t, uint16)
            WEBPP_IS_OPERATOR(stl::uint32_t, uint32)
            WEBPP_IS_OPERATOR(stl::uint64_t, uint64)
            WEBPP_IS_OPERATOR(double, double)
            WEBPP_IS_OPERATOR(float, float)
            WEBPP_IS_OPERATOR(string_type, string)
            WEBPP_IS_OPERATOR(string_view_type, string_view)

            // set here has no values!
            // todo: make custom Array and Object structs and don't rely on rapidjson
            // WEBPP_IS_METHOD(rapidjson_value_type::Array, array, IsArray, GetArray, SetArray)
            // WEBPP_IS_METHOD(rapidjson_value_type::Object, object, IsObject, GetObject, SetObject)
            // WEBPP_IS_METHOD(number, IsNumber, GetNumber, SetNumber)
            // WEBPP_IS_METHOD(true, IsTrue)
            // WEBPP_IS_METHOD(false, IsFalse)

#    undef IS_METHOD
#    undef WEBPP_IS_OPERATOR



            template <istl::String StrT = string_type, typename... Args>
            StrT as_string(Args&&... string_args) const {
                if constexpr (sizeof...(Args) == 0) {
                    return StrT{val_handle.GetString(), val_handle.GetStringLength()};
                } else {
                    StrT output{stl::forward<Args>(string_args)...};
                    output.append(val_handle.GetString(), val_handle.GetStringLength());
                    return output;
                }
            }

            string_view_type as_string_view() const {
                return string_view_type{val_handle.GetString(), val_handle.GetStringLength()};
            }

            generic_value& set_string(string_view_type str) {
                // todo: use allocator if possible
                val_handle.SetString(str.data(), str.size());
                return *this;
            }

            template <istl::String StrT = string_type, typename... Args>
            StrT pretty(Args&&... string_args) const {
                StrT output{stl::forward<Args>(string_args)...};
                ::rapidjson::PrettyWriter<stl::istringstream>{output};
                return output;
            }

            template <istl::String StrT = string_type, typename... Args>
            StrT uglified(Args&&... string_args) const {
                return as_string();
            }

            template <typename T>
            [[nodiscard]] value_ref_holder operator[](T&& val) {
                return {val_handle[stl::forward<T>(val)]};
            }

            template <typename T>
            generic_value& operator=(T&& val) {
                val_handle = stl::forward<T>(val);
                return *this;
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
            RENAME(array_type, GetArray, as_array, );
            RENAME(bool, IsNull, is_null, const);
            RENAME(bool, IsString, is_string, const);
            RENAME(bool, IsObject, is_object, const);
            RENAME(bool, IsArray, is_array, const);

            RENAME(generic_iterator_type, Begin, begin, );
            RENAME(generic_iterator_type, Begin, begin, const);
            RENAME(generic_iterator_type, End, end, );
            RENAME(generic_iterator_type, End, end, const);
            RENAME(generic_iterator_type, Begin, cbegin, const);
            RENAME(generic_iterator_type, End, cend, const);

#    undef RENAME
        };



    } // namespace details

    template <Traits TraitsType = default_traits>
    using value = details::generic_value<TraitsType, ::rapidjson::Value>;

    template <Traits TraitsType = default_traits>
    struct document : public details::generic_value<TraitsType, ::rapidjson::Document> {
        using traits_type              = TraitsType;
        using string_view_type         = traits::string_view<traits_type>;
        using char_type                = traits::char_type<traits_type>;
        using general_allocator_type   = traits::general_allocator<traits_type, char_type>;
        using value_type               = value<traits_type>;
        using rapidjson_document_type  = ::rapidjson::Document;
        using rapidjson_value_type     = typename rapidjson_document_type::ValueType;
        using rapidjson_allocator_type = typename rapidjson_document_type::AllocatorType;
        using object_type   = details::generic_object<traits_type, typename rapidjson_value_type::Object>;
        using array_type    = details::generic_array<traits_type, typename rapidjson_value_type::Array>;
        using document_type = details::generic_value<traits_type, ::rapidjson::Document>;

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
        requires(!stl::same_as<stl::remove_cvref_t<StrT>, document>) // not a copy/move ctor
          document(StrT&& json_string) {
            parse(stl::forward<StrT>(json_string));
        }

        /**
         * A document containing the specified, already parsed, value
         */
        template <typename ConvertibleToValue>
        requires(stl::convertible_to<ConvertibleToValue, value_type> && // check if it's a value or an object
                 !stl::same_as<stl::remove_cvref_t<ConvertibleToValue>, document>)
          document(ConvertibleToValue&& val)
          : document_type{stl::forward<ConvertibleToValue>(val)} {}

        template <JSONObject ObjType>
        document(ObjType&& obj) : document_type{stl::forward<ObjType>(obj)} {}

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
