#ifndef WEBPP_STD_COLLECTION_HPP
#define WEBPP_STD_COLLECTION_HPP

#include "../common/meta.hpp"
#include "utility.hpp"

namespace webpp::istl {

    namespace details {
        // NOLINTBEGIN(*-macro-usage)
#define webpp_support_func(method)                                                              \
    template <typename T>                                                                       \
    concept supports_##method = requires {                                                      \
        typename T::value_type;                                                                 \
        requires requires(typename T::value_type&& val, T obj) { obj.method(std::move(val)); }; \
    };                                                                                          \
                                                                                                \
    template <typename T, typename... Args>                                                     \
    concept supports_##method##_of = requires {                                                 \
        typename T::value_type;                                                                 \
        requires requires(T obj, Args... args) { obj.method(std::forward<Args>(args)...); };    \
    };

        webpp_support_func(emplace_back);
        webpp_support_func(emplace);
        webpp_support_func(push_back);
        webpp_support_func(add);
        webpp_support_func(push);
        webpp_support_func(append);
        webpp_support_func(insert);

        template <typename T>
        concept supports_op_append = requires {
            typename T::value_type;
            requires requires(typename T::value_type&& val, T obj) {
                obj.operator+=(std::move(val));
                requires !requires { obj = std::move(val); };
            };
        };

#undef webpp_support_func
        // NOLINTEND(*-macro-usage)
    } // namespace details

    template <typename T>
    concept AppendableCollection =
      !std::is_const_v<T> &&
      (details::supports_emplace_back<T> || details::supports_emplace<T> || details::supports_push_back<T> ||
       details::supports_push<T> || details::supports_add<T> || details::supports_append<T> ||
       details::supports_insert<T>);

    template <typename T>
    concept Collection = AppendableCollection<stl::remove_cvref_t<T>>;

    template <typename T>
    concept ReadOnlyCollection = requires(T obj) {
        typename T::value_type;
        obj.begin();
        obj.end();
    };

    template <Collection T, typename... Args>
    constexpr decltype(auto) emplace_one(T& vec, Args&&... args) {
        using details::supports_emplace_back;
        using stl::constructible_from;
        using value_type = typename std::remove_cvref_t<T>::value_type;

        if constexpr (details::supports_emplace_back_of<T, Args...>) {
            return vec.emplace_back(stl::forward<Args>(args)...);
        } else if constexpr (supports_emplace_back<T> && constructible_from<value_type, Args...>) {
            return vec.emplace_back(value_type{stl::forward<Args>(args)...});
        } else if constexpr (details::supports_emplace_of<T, Args...>) {
            return vec.emplace(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_emplace<T> && constructible_from<value_type, Args...>) {
            return vec.emplace(value_type{stl::forward<Args>(args)...});
        } else if constexpr (details::supports_push_back_of<T, Args...>) {
            return vec.push_back(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_push_back<T> && constructible_from<value_type, Args...>) {
            return vec.push_back(value_type{stl::forward<Args>(args)...});
        } else if constexpr (details::supports_push_of<T, Args...>) {
            return vec.push(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_add_of<T, Args...>) {
            return vec.add(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_append_of<T, Args...>) {
            return vec.append(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_insert_of<T, Args...>) {
            return vec.insert(stl::forward<Args>(args)...);
        } else if constexpr (details::supports_op_append<T> && sizeof...(Args) == 1) {
            return vec.operator+=(stl::forward<Args>(args)...);
        } else {
            static_assert_false(T, "We don't know how to add things to this collection");
        }
    }

    template <Collection T, typename... Args>
    static constexpr void emplace(T& vec, Args&&... args) {
        static_cast<void>((emplace_one(vec, stl::forward<Args>(args)), ...));
    }

    template <Collection T>
        requires(!requires { T::npos; }) // exclude strings, there's an istl::clear for strings
    constexpr void clear(T& vec) noexcept {
        vec.clear();                     // doesn't deallocate actually, so it's nothrow
    }

    template <typename T>
    concept LinearContainer = ReadOnlyCollection<T> && Collection<T> && requires(T container) {
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
