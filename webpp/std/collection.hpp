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

        template <Collection T, typename... Args>
        void emplace_one(T& vec, Args&&... args) {
            if constexpr (details::supports_emplace_back<T>) {
                vec.emplace_back(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_emplace<T>) {
                vec.emplace(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_push_back<T>) {
                vec.push_back(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_push<T>) {
                vec.push(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_add<T>) {
                vec.add(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_append<T>) {
                vec.append(stl::forward<Args>(args)...);
            } else if constexpr (details::supports_insert<T>) {
                vec.insert(stl::forward<Args>(args)...);
            } else {
                static_assert_false(T, "We don't know how to add things to this collection");
            }
        }

        template <Collection T>
        constexpr void clear(T& vec) noexcept {
            vec.clear(); // doesn't deallocate actually, so it's nothrow
        }


    } // namespace collection

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
