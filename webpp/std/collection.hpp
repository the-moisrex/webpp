#ifndef WEBPP_STD_COLLECTION_HPP
#define WEBPP_STD_COLLECTION_HPP

#include "../common/meta.hpp"
#include "utility.hpp"

namespace webpp::istl {

    namespace details {

// NOLINTBEGIN(*-macro-usage)
#define webpp_support_func(method, val_type) \
    template <typename T>                    \
    concept supports_##method = requires(val_type val, T obj) { obj.method(val); };

        webpp_support_func(emplace_back, typename T::value_type);
        webpp_support_func(emplace, typename T::value_type);
        webpp_support_func(push_back, typename T::value_type);
        webpp_support_func(add, typename T::value_type);
        webpp_support_func(push, typename T::value_type);
        webpp_support_func(append, typename T::value_type);
        webpp_support_func(insert, typename T::value_type);

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

    namespace collection {

        template <Collection T, typename... Args>
        void emplace(T& vec, Args&&... args) {
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
            } else {
                static_assert_false(T, "We don't know how to add things to this collection");
            }
        }


    } // namespace collection
} // namespace webpp::istl

#endif // COLLECTION_HPP
