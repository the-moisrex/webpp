// Created by moisrex on 5/3/20.

#ifndef WEBPP_CONTEXT_EXTENSION_MAP
#define WEBPP_CONTEXT_EXTENSION_MAP 1

#include "../../../std/map.hpp"

namespace webpp::extensions {

    /**
     * This context extension is designed for sharing arbitrary key/value pairs
     * of data between routes.
     *
     * With this extension the user can use "Strategy Pattern" in the routes;
     * Of course we don't like calling it "strategy pattern" since it doesn't
     * make any sense to do so. I like to call this just "Context Passing" or
     * even "Data Passing".
     *
     * @tparam Traits
     */


    template <typename Traits, typename KeyType = typename Traits::string_type,
              typename ValueType = typename Traits::string_type>
    struct map : public stl::map<Traits, KeyType, ValueType> {};


    template <typename Traits, typename KeyType = typename Traits::string_type,
              typename ValueType = typename Traits::string_type>
    struct multimap : public stl::multimap<Traits, KeyType, ValueType> {};


    /**
     * Extension As Field: as_data
     * Field name: data
     *
     * @tparam T
     */
    template <typename T>
    struct as_data {
        T data;
    };

} // namespace webpp::routes::extensions

#endif // WEBPP_CONTEXT_EXTENSION_MAP
