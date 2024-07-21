// Created by moisrex on 7/3/21.

#ifndef WEBPP_RAPIDJSON_HPP
#define WEBPP_RAPIDJSON_HPP

#if __has_include(<rapidjson/document.h> )
#    define WEBPP_RAPIDJSON_READY
#    include "../common/meta.hpp"
#    include "../memory/allocators.hpp"
#    include "../memory/stack.hpp"
#    include "../std/array.hpp"
#    include "../std/collection.hpp"
#    include "../std/string.hpp"
#    include "../std/string_view.hpp"
#    include "../traits/default_traits.hpp"
#    include "json_common.hpp"
#    include "json_concepts.hpp"

#    include <compare>
#    include <cstdio>
#    include <filesystem>

// NOLINTNEXTLINE(*-macro-usage)
#    define RAPIDJSON_HAS_STDSTRING 1 // enable std::string support for rapidjson (todo: do we need it?)
#    include <rapidjson/document.h>
#    include <rapidjson/filereadstream.h>
#    include <rapidjson/prettywriter.h>

#    if (RAPIDJSON_MAJOR_VERSION == 1 && RAPIDJSON_MINOR_VERSION == 1 && RAPIDJSON_PATCH_VERSION == 0)
namespace rapidjson {
    // a patch for issue: https://github.com/Tencent/rapidjson/pull/1947
    template <bool Const, typename Encoding, typename Allocator>
    GenericMemberIterator<Const, Encoding, Allocator> operator+(
      typename GenericMemberIterator<Const, Encoding, Allocator>::DifferenceType count,
      GenericMemberIterator<Const, Encoding, Allocator> const&                   pos) {
        return pos + count;
    }

    // This part doesn't need to be sent to rapidjson, it's already there
    // (https://github.com/the-moisrex/rapidjson/blob/f14d5097e51fc19582884a517699adef09edbff7/include/rapidjson/document.h#L262)
    // template <bool Const, typename Encoding, typename Allocator>
    // bool operator==(GenericMemberIterator<Const, Encoding, Allocator> const& pos1,
    //                 GenericMemberIterator<Const, Encoding, Allocator> const& pos2) {
    //     return pos1.operator->() == pos2.operator->();
    // }
} // namespace rapidjson
#    endif

namespace webpp::json::rapidjson {
    // using namespace ::rapidjson;

    // A wrapper for MemoryPoolAllocator and others to match std::allocator and a-like
    template <typename T>
    struct rapidjson_allocator_wrapper {
        using value_type                             = char;
        using size_type                              = stl::size_t;
        using difference_type                        = stl::ptrdiff_t;
        using propagate_on_container_move_assignment = stl::true_type;


      private:
        T* alloc = nullptr;


      public:
        constexpr rapidjson_allocator_wrapper() {
            // default init is not allowed, but we don't want to disappoint allocator_holder
            assert(alloc);
        }

        constexpr rapidjson_allocator_wrapper(rapidjson_allocator_wrapper const&)     = default;
        constexpr rapidjson_allocator_wrapper(rapidjson_allocator_wrapper&&) noexcept = default;
        constexpr ~rapidjson_allocator_wrapper()                                      = default;

        explicit(false) constexpr rapidjson_allocator_wrapper(T& the_alloc) : alloc{&the_alloc} {}

        explicit(false) constexpr rapidjson_allocator_wrapper(T const& the_alloc) : alloc{&the_alloc} {}

        template <typename U>
        explicit(false) constexpr rapidjson_allocator_wrapper(rapidjson_allocator_wrapper<U> const& wrapper)
          : alloc{wrapper.alloc} {}

        constexpr rapidjson_allocator_wrapper& operator=(rapidjson_allocator_wrapper const&)     = default;
        constexpr rapidjson_allocator_wrapper& operator=(rapidjson_allocator_wrapper&&) noexcept = default;

// NOLINTNEXTLINE(*-macro-usage)
#    define RENAME(old_name, new_sig) \
        new_sig {                     \
            return alloc->old_name(); \
        }

        RENAME(Capacity, auto capacity() const)
        RENAME(Size, stl::size_t size() const)
        RENAME(Clear, void clear())

#    undef RENAME

        void* malloc(stl::size_t size) {
            return alloc->Malloc(size);
        }

        void* realloc(void* original_ptr, size_t const original_size, size_t const new_size) {
            return alloc->Realloc(original_ptr, original_size, new_size);
        }

        static void free(void* ptr) {
            T::Free(ptr);
        }

        constexpr auto& native_alloc() const {
            return *alloc;
        }

        // C++ allocator compatibility

        [[nodiscard]] constexpr value_type* allocate(size_type const count) {
            return static_cast<value_type*>(malloc(count));
        }

        constexpr void deallocate(value_type* ptr, [[maybe_unused]] size_type const count) {
            free(static_cast<void*>(ptr));
        }
    };

    // if they pass a GenericObject or GenericValue itself.
    template <typename T>
        requires requires { typename stl::remove_cvref_t<T>::AllocatorType; }
    struct rapidjson_allocator_wrapper<T>
      : rapidjson_allocator_wrapper<typename stl::remove_cvref_t<T>::AllocatorType> {};

    /**
     * @brief The goal of this struct is to make this code happen:
     * @code
     *   for (auto [key, value] : doc.as_object()) {
     *       cout << key << ": " << value << endl;
     *   }
     * @endcode
     */
    template <typename MemberValType>
    struct key_value_pair {
        using key_type   = MemberValType;
        using value_type = MemberValType;

        // constexpr key_value_pair(key_value_pair const& inp_pair) : key{inp_pair.key}, value{inp_pair.value}
        // {}
        //
        // constexpr key_value_pair(key_value_pair&&) noexcept = default;
        //
        // constexpr key_value_pair(key_type const& inp_key, value_type const& inp_val)
        //   : key{inp_key},
        //     value{inp_val} {}
        //
        // constexpr key_value_pair(key_type& inp_key, value_type& inp_val) : key{inp_key}, value{inp_val} {}
        //
        // constexpr key_value_pair(key_type&& inp_key, value_type&& inp_val)
        //   : key{stl::move(inp_key)},
        //     value{stl::move(inp_val)} {}
        //
        // constexpr key_value_pair(key_type const& inp_key, value_type& inp_val)
        //   : key{inp_key},
        //     value{inp_val} {}
        //
        // constexpr key_value_pair(key_type const& inp_key, value_type&& inp_val)
        //   : key{inp_key},
        //     value{stl::move(inp_val)} {}
        //
        // constexpr key_value_pair(key_type& inp_key, value_type const& inp_val)
        //   : key{inp_key},
        //     value{inp_val} {}
        //
        // constexpr key_value_pair(key_type&& inp_key, value_type const& inp_val)
        //   : key{stl::move(inp_key)},
        //     value{inp_val} {}
        //
        // key_value_pair& operator=(key_value_pair const&)     = default;
        // key_value_pair& operator=(key_value_pair&&) noexcept = default;
        // ~               key_value_pair()                     = default;
        //
        // auto operator<=>(key_value_pair const&) const = default;
        //
        // auto operator<=>(MemberValType const& val) const {
        //     return val <=> value;
        // }

        key_type   key;
        value_type value;
    };

    /**
     * todo: add a choice to use rapidjson's allocator
     * todo: use traits_type's allocator correctly if possible
     */
    namespace details {

        template <Traits TraitsType, typename ValueType>
            requires requires { typename stl::remove_cvref_t<ValueType>::AllocatorType; } // has an allocator
        struct generic_value;

        template <Traits TraitsType, typename ArrayType>
        struct generic_array;

        template <Traits TraitsType, typename ObjectType>
            requires requires { typename stl::remove_cvref_t<ObjectType>::AllocatorType; } // has an allocator
        struct generic_object;

        /**
         * Generic Member Iterator
         */
        template <Traits TraitsType, typename RapidJSONIterator>
        struct generic_member_iterator
          : stl::remove_pointer_t<RapidJSONIterator>,
            allocator_holder<rapidjson_allocator_wrapper<typename stl::remove_cvref_t<
              decltype(stl::declval<typename stl::remove_cvref_t<
                         typename stl::remove_cvref_t<RapidJSONIterator>::reference>>()
                         .name)>::AllocatorType>> {
            using traits_type                 = TraitsType;
            using base_type                   = stl::remove_pointer_t<RapidJSONIterator>;
            using rapidjson_reference         = typename base_type::reference;
            using rapidjson_pointer           = typename base_type::pointer;
            using rapidjson_difference_type   = typename base_type::difference_type;
            using rapidjson_value_type        = typename base_type::value_type;
            using rapidjson_iterator_category = typename base_type::iterator_category;
            using rapidjson_const_iterator    = typename base_type::ConstIterator;
            using rapidjson_iterator          = typename base_type::NonConstIterator;
            using rapidjson_member_value_type =
              stl::remove_cvref_t<decltype(stl::declval<stl::remove_cvref_t<rapidjson_value_type>>().name)>;
            using rapidjson_member_value_type_auto =
              stl::conditional_t<stl::is_const_v<rapidjson_value_type>,
                                 stl::add_cv_t<rapidjson_member_value_type>,
                                 stl::add_lvalue_reference_t<rapidjson_member_value_type>>;
            using allocator_holder_type =
              allocator_holder<rapidjson_allocator_wrapper<typename stl::remove_cvref_t<
                decltype(stl::declval<typename stl::remove_cvref_t<
                           typename stl::remove_cvref_t<RapidJSONIterator>::reference>>()
                           .name)>::AllocatorType>>;
            using allocator_type = typename allocator_holder_type::allocator_type;

            using item_type = generic_value<traits_type, rapidjson_member_value_type_auto>;

            using iterator          = generic_member_iterator;
            using const_iterator    = generic_member_iterator<traits_type, rapidjson_const_iterator> const;
            using iterator_category = rapidjson_iterator_category;
            using value_type        = key_value_pair<item_type>;
            using pointer           = value_type;
            using reference         = value_type;
            using difference_type   = rapidjson_difference_type;

            generic_member_iterator(base_type const& iter, allocator_type const& the_alloc)
              : base_type{iter},
                allocator_holder_type{the_alloc} {}

            generic_member_iterator(base_type&& iter, allocator_type const& the_alloc)
              : base_type{stl::move(iter)},
                allocator_holder_type{the_alloc} {}

            generic_member_iterator(generic_member_iterator const& iter)     = default;
            generic_member_iterator(generic_member_iterator&& iter) noexcept = default;
            ~generic_member_iterator() noexcept                              = default;

            iterator& operator=(iterator&& iter) noexcept      = default;
            iterator& operator=(iterator const& iter) noexcept = default;

            //            iterator& operator=(const iterator& iter) {
            //                base_type::opreator = (iter);
            //                return *this;
            //            }



            iterator& operator++() {
                base_type::operator++();
                return *this;
            }

            iterator& operator--() {
                base_type::operator--();
                return *this;
            }

            iterator operator++(int) { // NOLINT(cert-dcl21-cpp)
                return iterator(*this).operator++();
            }

            iterator operator--(int) { // NOLINT(cert-dcl21-cpp)
                return iterator(*this).operator--();
            }

            iterator operator+(difference_type n) const {
                return base_type::operator+(n);
            }

            iterator operator-(difference_type n) const {
                return base_type::operator-(n);
            }

            iterator& operator+=(difference_type n) {
                base_type::operator+=(n);
                return *this;
            }

            iterator& operator-=(difference_type n) {
                base_type::operator-=(n);
                return *this;
            }

            bool operator==(const_iterator that) const {
                return base_type::operator==(that);
            }

            bool operator!=(const_iterator that) const {
                return base_type::operator!=(that);
            }

            bool operator<=(const_iterator that) const {
                return base_type::operator<=(that);
            }

            bool operator>=(const_iterator that) const {
                return base_type::operator>=(that);
            }

            bool operator<(const_iterator that) const {
                return base_type::operator<(that);
            }

            bool operator>(const_iterator that) const {
                return base_type::operator>(that);
            }

            bool operator==(iterator const& that) const {
                return base_type::operator==(that);
            }

            bool operator!=(iterator const& that) const {
                return base_type::operator!=(that);
            }

            bool operator<=(iterator const& that) const {
                return base_type::operator<=(that);
            }

            bool operator>=(iterator const& that) const {
                return base_type::operator>=(that);
            }

            bool operator<(iterator const& that) const {
                return base_type::operator<(that);
            }

            bool operator>(iterator const& that) const {
                return base_type::operator>(that);
            }

            reference operator*() const {
                auto& res = base_type::operator*();
                return {
                  { res.name.Move(), this->get_allocator()},
                  {res.value.Move(), this->get_allocator()}
                };
            }

            pointer operator->() const {
                return base_type::operator->();
            }

            reference operator[](difference_type n) const {
                return base_type::operator[](n);
            }

            difference_type operator-(const_iterator that) const {
                return base_type::operator-(that);
            }
        };

        template <Traits TraitsType, typename RapidJSONIterator>
        generic_member_iterator<TraitsType, RapidJSONIterator> operator+(
          typename generic_member_iterator<TraitsType, RapidJSONIterator>::diff_t count,
          generic_member_iterator<TraitsType, RapidJSONIterator> const&           pos) {
            return pos + count;
        }

        /**
         * @brief This is the common things between values' types.
         * @tparam TraitsType
         * @tparam ValueContainer
         */
        template <Traits TraitsType, typename ValueContainer>
        struct json_common {
          private:
            // DocType could be a document or an GenericObject actually
            template <typename DocType>
            struct value_type_finder {
                using type = typename DocType::ValueType;
            };

          public:
            // finding the rapidjson's ValueType even if the ValueType is a ::rapidjson::Document type or a
            // ::rapidjson::GenericObject
            static constexpr bool has_ref =
              !stl::same_as<ValueContainer, stl::remove_cvref_t<ValueContainer>>;
            using container_type = ValueContainer;
            using value_type     = istl::lazy_conditional_t<
                  (requires { typename stl::remove_cvref_t<ValueContainer>::ValueType; }),
                  istl::templated_lazy_type<value_type_finder, stl::remove_cvref_t<ValueContainer>>,
                  istl::lazy_type<stl::remove_cvref_t<ValueContainer>>>;

            static_assert(
              requires { typename value_type::Object; },
              "The specified ValueType doesn't seem to be a valid rapidjson value");

            using traits_type            = TraitsType;
            using string_type            = traits::string<traits_type>;
            using string_view_type       = traits::string_view<traits_type>;
            using char_type              = traits::char_type<traits_type>;
            using generic_value_type     = generic_value<traits_type, value_type>;
            using value_ref              = stl::add_lvalue_reference_t<value_type>;     // add & to obj
            using value_const_ref        = stl::add_const_t<value_ref>;                 // add const
            using auto_ref_value_type    = stl::conditional_t<has_ref, value_ref, value_type>;
            using value_ref_holder       = generic_value<traits_type, value_ref>;       // ref holder
            using value_const_ref_holder = generic_value<traits_type, value_const_ref>; // ref holder
            using rapidjson_object_type  = typename value_type::Object;
            using object_type            = generic_object<traits_type, rapidjson_object_type>;
            using rapidjson_array_type   = typename value_type::Array;
            using array_type             = generic_array<traits_type, rapidjson_array_type>;
            using rapidjson_value_type   = value_type;
            using allocator_type =
              rapidjson_allocator_wrapper<typename stl::remove_cvref_t<ValueContainer>::AllocatorType>;

            constexpr json_common() : alloc{val_handle.GetAllocator()} {}

            constexpr json_common(json_common const&)     = default;
            constexpr json_common(json_common&&) noexcept = default;
            constexpr ~json_common()                      = default;

            json_common& operator=(json_common&&) noexcept = default;
            json_common& operator=(json_common const&)     = default;

            template <typename ValT>
            json_common(ValT&& obj, allocator_type const& inp_alloc)
              : val_handle{stl::forward<ValT>(obj)},
                alloc{inp_alloc} {}

            template <typename ValT>
                requires requires(ValT val) { val.GetAllocator(); }
            explicit(false) json_common(ValT&& obj)
              : val_handle{stl::forward<ValT>(obj)},
                alloc{obj.GetAllocator()} {}

            template <typename T>
            auto& operator=(T&& val) {
                using type = stl::remove_cvref_t<T>;
                if constexpr (istl::StringViewifiable<T>) {
                    auto const val_view = istl::string_viewify_of<string_view_type>(stl::forward<T>(val));
                    val_handle          = ::rapidjson::StringRef(val_view.data(), val_view.size());
                } else if constexpr (istl::is_specialization_of_v<type, json::field>) {
                    this->as_object()[val.key] = val.value();
                } else if constexpr (istl::is_specialization_of_v<type, json::field_pack>) {
                    val.apply([this](auto&&... field) {
                        auto obj = this->as_object();
                        ((obj[field.key] = field.value()), ...);
                    });
                } else if constexpr (JSONArray<T> || stl::is_array_v<T> ||
                                     istl::is_specialization_of_array_v<T> || istl::Collection<T>)
                {
                    rapidjson_value_type data{::rapidjson::kArrayType};
                    for (auto&& item : val) {
                        using item_type = stl::remove_cvref_t<decltype(item)>;
                        if constexpr (requires { rapidjson_value_type{item}; }) {
                            data.PushBack(rapidjson_value_type{item}.Move(),
                                          this->get_allocator().native_alloc());
                        } else if constexpr (istl::StringViewifiable<item_type>) {
                            auto const item_view = istl::string_viewify(stl::move(item));
                            auto const item_ref  = ::rapidjson::StringRef(item_view.data(), item_view.size());
                            data.PushBack(rapidjson_value_type{item_ref}.Move(),
                                          this->get_allocator().native_alloc());
                        }
                    }
                    val_handle = data.Move();
                } else {
                    val_handle = stl::forward<T>(val);
                }
                return *this;
            }

            template <typename T>
            [[nodiscard]] bool is() const {
                return val_handle.template Is<T>();
            }

            template <typename T>
            [[nodiscard]] T as() const {
                return val_handle.template Get<T>();
            }

            // NOLINTNEXTLINE(*-macro-usage)
#    define IS_METHOD(real_type, type_name, is_func, get_func, set_func) \
        [[nodiscard]] bool is_##type_name() const {                      \
            return val_handle.is_func();                                 \
        }                                                                \
                                                                         \
        value_ref_holder set_##type_name(real_type const& val) {         \
            val_handle.set_func(val);                                    \
            return *this;                                                \
        }                                                                \
                                                                         \
        value_ref_holder set_##type_name(real_type&& val) {              \
            val_handle.set_func(stl::move(val));                         \
            return *this;                                                \
        }                                                                \
                                                                         \
        [[nodiscard]] real_type as_##type_name() const {                 \
            return val_handle.get_func();                                \
        }


            // NOLINTNEXTLINE(*-macro-usage)
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

            // NOLINTNEXTLINE(*-macro-usage)
#    define RENAME(ret_type, orig_name, new_name, details) \
        ret_type new_name() details {                      \
            return val_handle.orig_name();                 \
        }

            RENAME(bool, IsNull, is_null, const);
            RENAME(bool, IsString, is_string, const);
            RENAME(bool, IsObject, is_object, const);
            RENAME(bool, IsArray, is_array, const);

#    undef RENAME

            [[nodiscard]] constexpr object_type as_object() {
                return object_type{val_handle.GetObject(), this->get_allocator()};
            }

            [[nodiscard]] constexpr array_type as_array() {
                return array_type{val_handle.GetArray()};
            }

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

            value_ref_holder set_string(string_view_type str) {
                // todo: use allocator if possible
                val_handle.SetString(str.data(), str.size());
                return *this;
            }

            auto& set_object() {
                val_handle.SetObject();
                return *this;
            }

            [[nodiscard]] constexpr decltype(auto) get_allocator() const {
                return alloc;
            }

          protected:
            container_type val_handle{};
            allocator_type alloc;
        };

        /**
         * This is a generic array holder
         */
        template <Traits TraitsType, typename ArrayType>
        struct generic_array {
            using traits_type          = TraitsType;
            using rapidjson_array_type = ArrayType;
            using rapidjson_value_type = typename rapidjson_array_type::ValueType;
            using value_type           = generic_value<traits_type, rapidjson_value_type>;

            explicit(false) constexpr generic_array(rapidjson_array_type& arr) : arr_handle{arr} {}

            explicit(false) constexpr generic_array(rapidjson_array_type const& arr) : arr_handle{arr} {}

            explicit(false) constexpr generic_array(rapidjson_value_type& arr) : arr_handle{arr.GetArray()} {}

            explicit(false) constexpr generic_array(rapidjson_value_type const& arr)
              : arr_handle{arr.GetArray()} {}

            [[nodiscard]] constexpr stl::size_t size() const {
                return arr_handle.Size();
            }

            [[nodiscard]] constexpr stl::size_t capacity() const {
                return arr_handle.Capacity();
            }

            template <JSONNumber T>
            [[nodiscard]] constexpr auto& operator[](T index) {
                return *stl::find(begin(), end(), static_cast<::rapidjson::SizeType>(index));
            }

            constexpr auto begin() const {
                return arr_handle.Begin();
            }

            constexpr auto end() const {
                return arr_handle.End();
            }

            constexpr auto cbegin() const {
                return arr_handle.Begin();
            }

            constexpr auto cend() const {
                return arr_handle.End();
            }



          private:
            rapidjson_array_type arr_handle;
        };

        /**
         * Generic number will hold
         *   - json numeric values
         *   - booleans
         * It hold booleans too because it's not JavaScript and bools are still numbers! :)
         */
        template <Traits TraitsType, typename ValueType>
        struct generic_number : json_common<TraitsType, ValueType> {
            using rapidjson_value_type = ValueType;
            using traits_type          = TraitsType;
            using value_type           = generic_value<traits_type, rapidjson_value_type>;
            using string_view_type     = traits::string_view<traits_type>;
            using json_common_type     = json_common<TraitsType, ValueType>;

            using json_common_type::json_common;  // common ctors

            template <typename T>
                requires(stl::is_arithmetic_v<T>) // only numbers
            explicit(false) generic_number(T val) : json_common_type{rapidjson_value_type{val}} {}

            /**
             * Checks whether a number can be losslessly converted to a float.
             */
            [[nodiscard]] bool is_lossless_float() const {
                return this->val_handle.IsLosslessFloat();
            }
        };

        namespace details {
            define_is_specialization_of(
              is_generic_object,
              auto             WEBPP_COMMA typename...,
              auto Item        WEBPP_COMMA typename... Args,
              Item WEBPP_COMMA Args...);
        } // namespace details

        /**
         * This is a json object which means it can hold a key/value pair of value objects.
         * The ValueType is a rapidjson value type not a generic value type.
         */
        template <Traits TraitsType, typename ObjectType>
            requires requires {
                typename stl::remove_cvref_t<ObjectType>::AllocatorType;
            } // GenericAllocator has an Allocator itself.
        struct generic_object
          : public allocator_holder<
              rapidjson_allocator_wrapper<typename stl::remove_cvref_t<ObjectType>::AllocatorType>> {
            static_assert(details::is_generic_object_v<ObjectType, ::rapidjson::GenericObject>,
                          "it's an object not a value");

            using rapidjson_object_type      = ObjectType;
            using rapidjson_plain_value_type = typename rapidjson_object_type::PlainType;
            using traits_type                = TraitsType;
            using value_type =
              generic_value<traits_type, stl::add_lvalue_reference_t<rapidjson_plain_value_type>>;
            using string_view_type                = traits::string_view<traits_type>;
            using rapidjson_member_iterator       = typename rapidjson_object_type::MemberIterator;
            using rapidjson_const_member_iterator = typename rapidjson_object_type::ConstMemberIterator;
            using iterator_type       = generic_member_iterator<traits_type, rapidjson_member_iterator>;
            using const_iterator_type = generic_member_iterator<traits_type, rapidjson_const_member_iterator>;
            using allocator_holder_type = allocator_holder<
              rapidjson_allocator_wrapper<typename stl::remove_cvref_t<ObjectType>::AllocatorType>>;
            using allocator_type = typename allocator_holder_type::allocator_type;

            // todo: add more optimization for reference and const reference and move
            // rapidjson_object_type might be a reference itself.
            constexpr generic_object(rapidjson_object_type obj, allocator_type const& the_alloc)
              : allocator_holder_type(the_alloc),
                obj_handle{obj} {}

            template <JSONKey KeyType>
            [[nodiscard]] value_type operator[](KeyType&& key) {
                if constexpr (JSONNumber<KeyType>) {
                    // fixme: write tests for this, this will run the "index" version, right?
                    auto const key_value = rapidjson_plain_value_type{stl::forward<KeyType>(key)};
                    return value_type{obj_handle[key_value], this->get_allocator()};
                } else if constexpr (JSONString<KeyType>) {
                    // The key is convertible to string_view
                    auto const key_view =
                      istl::string_viewify_of<string_view_type>(stl::forward<KeyType>(key));
                    auto const key_ref = ::rapidjson::StringRef(key_view.data(), key_view.size());
                    if (!contains(key_ref)) {
                        obj_handle.AddMember(rapidjson_plain_value_type{key_ref},
                                             rapidjson_plain_value_type{},
                                             this->get_allocator().native_alloc()); // null value
                    }
                    return value_type{obj_handle[rapidjson_plain_value_type{key_ref}], this->get_allocator()};
                } else {
                    return value_type{obj_handle[stl::forward<KeyType>(key)], this->get_allocator()};
                }
            }

            generic_object& clear() {
                obj_handle.Clear();
                return *this;
            }

            [[nodiscard]] stl::size_t size() const noexcept {
                return obj_handle.MemberCount();
            }

            template <JSONKey KeyT, typename ValT>
            generic_object& insert(KeyT&& key, ValT&& val) {
                auto const key_view = istl::string_viewify_of<string_view_type>(stl::forward<KeyT>(key));
                obj_handle.AddMember(::rapidjson::StringRef(key_view.data(), key_view.size()),
                                     stl::forward<ValT>(val),
                                     this->get_allocator().native_alloc());
                return *this;
            }

            template <JSONKey KeyT, typename ValT>
            generic_object& emplace(KeyT&& key, ValT&& val) {
                return insert<KeyT, ValT>(stl::forward<KeyT>(key), stl::forward<ValT>(val));
            }

            // static_assert(stl::random_access_iterator<typename rapidjson_object_type::MemberIterator>);
            iterator_type begin() const {
                return {obj_handle.MemberBegin(), this->get_allocator()};
            }

            iterator_type end() const {
                return {obj_handle.MemberEnd(), this->get_allocator()};
            }

            iterator_type cbegin() const {
                return {obj_handle.MemberBegin(), this->get_allocator()};
            }

            iterator_type cend() const {
                return {obj_handle.MemberEnd(), this->get_allocator()};
            }

            template <typename KeyT>
            [[nodiscard]] bool contains(KeyT&& key) const {
                if constexpr (JSONKey<KeyT>) {
                    auto const key_view = istl::string_viewify_of<string_view_type>(stl::forward<KeyT>(key));
                    return obj_handle.HasMember(key_view.data()); // fixme: not passing the length
                } else if constexpr (requires { obj_handle.HasMember(stl::forward<KeyT>(key)); }) {
                    return obj_handle.HasMember(stl::forward<KeyT>(key));
                } else {
                    static_assert_false(KeyT, "KeyT is not a valid json key.");
                    return false; // just to get rid of a warning
                }
            }


          private:
            rapidjson_object_type obj_handle;
        };

        /**
         * Rapidjson GenericValue wrapper
         * @tparam TraitsType
         * @tparam ValueType
         */
        template <Traits TraitsType, typename ValueType>
            requires requires { typename stl::remove_cvref_t<ValueType>::AllocatorType; } // has an allocator
        struct generic_value : json_common<TraitsType, ValueType> {
            using traits_type            = TraitsType;
            using common_type            = json_common<traits_type, ValueType>;
            using string_type            = typename common_type::string_type;
            using string_view_type       = typename common_type::string_view_type;
            using char_type              = typename common_type::char_type;
            using generic_value_type     = typename common_type::generic_value_type;
            using value_ref              = typename common_type::value_ref;
            using value_ref_holder       = typename common_type::value_ref_holder;
            using value_const_ref_holder = typename common_type::value_const_ref_holder;
            using rapidjson_object_type  = typename common_type::rapidjson_object_type;
            using object_type            = typename common_type::object_type;
            using rapidjson_array_type   = typename common_type::rapidjson_array_type;
            using array_type             = typename common_type::array_type;
            using value_type             = typename common_type::value_type;
            using allocator_holder_type  = allocator_holder<rapidjson_allocator_wrapper<ValueType>>;
            using allocator_type         = typename allocator_holder_type::allocator_type;
            using rapidjson_value_type   = typename common_type::rapidjson_value_type;


            using json_common<TraitsType, ValueType>::json_common;
            using json_common<TraitsType, ValueType>::operator=;

            constexpr generic_value(generic_value const&)     = default;
            constexpr generic_value(generic_value&&) noexcept = default;
            constexpr ~generic_value()                        = default;

            template <typename V>
                requires(!stl::is_same_v<stl::remove_cvref_t<V>, generic_value>) // no ctor
            constexpr generic_value( // NOLINT(*-forwarding-reference-overload)
              V&&                   val,
              allocator_type const& inp_alloc)
              : json_common<TraitsType, ValueType>(stl::forward<V>(val), inp_alloc) {}

            template <typename V>
                requires(!stl::is_same_v<stl::remove_cvref_t<V>, generic_value>) // no ctor
            explicit(false) constexpr generic_value( // NOLINT(*-forwarding-reference-overload)
              V&& val)
              : json_common<TraitsType, ValueType>(stl::forward<V>(val)) {}

            /**
             * Check if it has a member
             */
            [[nodiscard]] bool contains(string_view_type key) const {
                return this->val_handle.HasMember(::rapidjson::StringRef(key.data(), key.size()));
            }

            // this method will assume that the value is an object
            template <typename T>
            [[nodiscard]] decltype(auto) operator[](T&& val) {
                if constexpr (JSONNumber<T>) {
                    return this->as_array()[stl::forward<T>(val)];
                } else if (this->is_object()) {
                    return this->as_object()[stl::forward<T>(val)];
                } else {
                    this->set_object();
                    return this->as_object().emplace(val, rapidjson_value_type{})[val];
                }
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

            // NOLINTNEXTLINE(*-macro-usage)
#    define RENAME(ret_type, orig_name, new_name, details) \
        ret_type new_name() details {                      \
            return this->val_handle.orig_name();           \
        }

            RENAME(stl::size_t, Size, size, const);
            RENAME(bool, Empty, empty, const);
            RENAME(stl::size_t, Capacity, capacity, const);
            RENAME(void, Clear, clear, );

            //            RENAME(generic_iterator_type, Begin, begin, );
            //            RENAME(generic_iterator_type, Begin, begin, const);
            //            RENAME(generic_iterator_type, End, end, );
            //            RENAME(generic_iterator_type, End, end, const);
            //            RENAME(generic_iterator_type, Begin, cbegin, const);
            //            RENAME(generic_iterator_type, End, cend, const);

#    undef RENAME

            // this is a nice idea, but we have to have common iterator wrapper for this to work
            /*
#    define webpp_iterator_method(iter_name, constness)                                               \
                                                                                                      \
        auto iter_name() constness {                                                                  \
            if (this->is_array()) {                                                                   \
                return this->as_array().iter_name();                                                  \
            } else if (this->is_object()) {                                                           \
                return this->as_object().iter_name();                                                 \
            } else if (this->is_string()) {                                                           \
                return this->as_string_view().iter_name();                                            \
            } else {                                                                                  \
                throw stl::invalid_argument("This json value is not an array, object,"                \
                                            " or a string so you cannot get an iterator out of it."); \
            }                                                                                         \
        }

            webpp_iterator_method(begin, )         // begin
              webpp_iterator_method(end, )         // end
              webpp_iterator_method(cbegin, const) // cbegin
              webpp_iterator_method(cend, const)   // cend

#    undef webpp_iterator_method
            */
        };



    } // namespace details

    template <Traits TraitsType = default_traits>
    using value = details::generic_value<TraitsType, ::rapidjson::Value>;

    /**
     * Rapidjson's GenericDocument wrapper
     * @tparam TraitsType
     */
    template <Traits TraitsType = default_traits>
    struct document : details::generic_value<TraitsType, ::rapidjson::Document> {
        using traits_type              = TraitsType;
        using string_view_type         = traits::string_view<traits_type>;
        using string_type              = traits::string<traits_type>;
        using char_type                = traits::char_type<traits_type>;
        using general_allocator_type   = traits::allocator_type_of<traits_type, char_type>;
        using value_type               = value<traits_type>;
        using rapidjson_document_type  = ::rapidjson::Document;
        using rapidjson_value_type     = typename rapidjson_document_type::ValueType;
        using rapidjson_allocator_type = typename rapidjson_document_type::AllocatorType;
        using object_type = details::generic_object<traits_type, typename rapidjson_value_type::Object>;
        using array_type  = details::generic_array<traits_type, typename rapidjson_value_type::Array>;
        using generic_value_type = details::generic_value<traits_type, rapidjson_document_type>;
        using allocator_type     = typename rapidjson_document_type::AllocatorType;

        using generic_value_type::operator=;

        /**
         * A document containing null
         */
        document() = default;

        /**
         * Get the file and parse it.
         */
        explicit(false) document(stl::filesystem::path const& file_path) : generic_value_type{} {
            stl::FILE* fp = stl::fopen(file_path.c_str(), "rbe");

            stack<65'536>               read_buffer;
            ::rapidjson::FileReadStream inp_stream(fp, read_buffer.data(), read_buffer.size());
            this->ParseStream(inp_stream);

            stl::fclose(fp);
        }

        /**
         * Parse the json string specified here
         */
        template <istl::StringViewifiable StrT>
            requires(!istl::cvref_as<StrT, document>) // not a copy/move ctor
        explicit(false) document(StrT&& json_string)  // NOLINT(bugprone-forwarding-reference-overload)
          : generic_value_type{} {
            parse(stl::forward<StrT>(json_string));
        }

        /**
         * A document containing the specified, already parsed, value
         */
        template <typename ConvertibleToValue>
            requires(!istl::StringViewifiable<ConvertibleToValue> &&
                     (stl::convertible_to<stl::remove_cvref_t<ConvertibleToValue>,
                                          value_type> &&   // check if it's a value or an object
                      !istl::cvref_as<ConvertibleToValue, document>) )
        explicit(false) document(ConvertibleToValue&& val) // NOLINT(bugprone-forwarding-reference-overload)
          : generic_value_type{stl::forward<ConvertibleToValue>(val)} {}

        // implement the parse method
        template <istl::StringViewifiable StrT>
        document& parse(StrT&& json_string) {
            auto const json_str_view = istl::string_viewify(stl::forward<StrT>(json_string));
            this->val_handle.Parse(json_str_view.data(), json_str_view.size());
            return *this;
        }

        template <istl::String StrT = string_type, typename... Args>
        [[nodiscard]] StrT pretty(Args&&... string_args) const {
            StrT output{stl::forward<Args>(string_args)...};
            pretty(output);
            return output;
        }

        // todo: UTF8 support
        template <istl::String StrT = string_type>
        void pretty(StrT& output) const {
            ::rapidjson::StringBuffer                            buf;
            ::rapidjson::PrettyWriter<::rapidjson::StringBuffer> writer{buf};
            this->value_handle.Accept(writer);
            output.append(buf.GetString(), buf.GetSize());
        }

        // todo: add ascii and stream support
        template <istl::String StrT = string_type>
        void uglified(StrT& output) const {
            ::rapidjson::StringBuffer                      buf;
            ::rapidjson::Writer<::rapidjson::StringBuffer> writer(buf);
            this->val_handle.Accept(writer);
            output.append(buf.GetString(), buf.GetSize());
        }

        template <istl::String StrT = string_type, typename... Args>
        [[nodiscard]] StrT uglified(Args&&... args) const {
            StrT output{stl::forward<Args>(args)...};
            uglified(output);
            return output;
        }
    };

} // namespace webpp::json::rapidjson

#endif

#endif // WEBPP_RAPIDJSON_HPP
