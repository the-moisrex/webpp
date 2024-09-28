#ifndef WEBPP_STD_COLLECTION_HPP
#define WEBPP_STD_COLLECTION_HPP

#include "../common/meta.hpp"
#include "utility.hpp"

namespace webpp::istl {

    namespace details {
        // NOLINTBEGIN(*-macro-usage)
#define webpp_support_func(name, method)                                              \
    template <typename T>                                                             \
    concept supports_##name = !std::is_const_v<T> && requires {                       \
        typename std::remove_cvref_t<T>::value_type;                                  \
        requires requires(typename std::remove_cvref_t<T>::value_type&& val, T obj) { \
            obj.method(std::move(val));                                               \
            requires !requires { obj = std::move(val); };                             \
        };                                                                            \
    };

        webpp_support_func(emplace_back, emplace_back);
        webpp_support_func(emplace, emplace);
        webpp_support_func(push_back, push_back);
        webpp_support_func(add, add);
        webpp_support_func(push, push);
        webpp_support_func(append, append);
        webpp_support_func(insert, insert);
        webpp_support_func(op_append, operator+=);


#undef webpp_support_func
        // NOLINTEND(*-macro-usage)
    } // namespace details

    template <typename T>
    concept AppendableCollection =
      details::supports_emplace_back<T> || details::supports_emplace<T> || details::supports_push_back<T> ||
      details::supports_push<T> || details::supports_add<T> || details::supports_append<T> ||
      details::supports_insert<T>;

    template <typename T>
    concept Collection = AppendableCollection<T>;

    template <typename T>
    concept ReadOnlyCollection = requires(T obj) {
        typename T::value_type;
        obj.begin();
        obj.end();
    };

    template <Collection T, typename... Args>
    static constexpr void emplace(T& vec, Args&&... args) {
        if constexpr (details::supports_emplace_back<T>) {
            (vec.emplace_back(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_emplace<T>) {
            (vec.emplace(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_push_back<T>) {
            (vec.push_back(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_push<T>) {
            (vec.push(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_add<T>) {
            (vec.add(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_append<T>) {
            (vec.append(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_insert<T>) {
            (vec.insert(stl::forward<Args>(args)), ...);
        } else if constexpr (details::supports_op_append<T> && sizeof...(Args) == 1) {
            (vec.operator+=(stl::forward<Args>(args)), ...);
        } else {
            static_assert_false(T, "We don't know how to add things to this collection");
        }
    }

    template <Collection T, typename... Args>
    constexpr decltype(auto) emplace_one(T& vec, Args&&... args) {
        if constexpr (details::supports_emplace_back<T>) {
            return vec.emplace_back(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_emplace<T>) {
            return vec.emplace(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_push_back<T>) {
            return vec.push_back(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_push<T>) {
            return vec.push(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_add<T>) {
            return vec.add(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_append<T>) {
            return vec.append(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_insert<T>) {
            return vec.insert(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_op_append<T> && sizeof...(Args) == 1) {
            return vec.operator+=(std::forward<Args>(args)...);
        } else {
            static_assert_false(T, "We don't know how to add things to this collection");
        }
    }

    template <Collection T>
        requires(!requires { T::npos; }) // exclude strings, there's an istl::clear for strings
    constexpr void clear(T& vec) noexcept {
        vec.clear();                     // doesn't deallocate actually, so it's nothrow
    }

    template <typename T>
    concept LinearContainer = ReadOnlyCollection<T> && AppendableCollection<T> && requires(T container) {
        typename T::value_type;
        typename T::size_type;
        typename T::allocator_type;
        typename T::iterator;
        typename T::const_iterator;
        container.size();
    };

    template <typename T>
    concept MapContainer = LinearContainer<T> && requires(T container) {
        typename T::key_type;
        typename T::mapped_type;
        requires requires(typename T::key_type key, typename T::mapped_type value) {
            container[key];         // get the value
            container[key] = value; // set the value
        };
    };


} // namespace webpp::istl

#endif // COLLECTION_HPP
