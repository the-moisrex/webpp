#ifndef WEBPP_DDATABASE_SQL_QUERY_BUILDER_HPP
#define WEBPP_DDATABASE_SQL_QUERY_BUILDER_HPP

#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../traits/traits.hpp"
#include "sql_concepts.hpp"

namespace webpp::sql {

    template <typename DBType>
    struct query_builder;


    namespace details {
        template <typename DBType>
        struct query_builder_subclasses {
            using driver_type        = DBType;
            using traits_type        = typename driver_type::traits_type;
            using string_type        = traits::general_string<traits_type>;
            using query_builder_type = query_builder<driver_type>;

            // get query_builder reference
#define define_enclosing(obj)                                                                   \
    constexpr inline query_builder_type& enclosing() noexcept {                                 \
        return *reinterpret_cast<query_builder_type*>(reinterpret_cast<char*>(this) -           \
                                                      offsetof(query_builder_subclasses, obj)); \
    }

            [[no_unique_address]] struct table_type {

                define_enclosing(table);

                // set the name
                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& name(StrvT&& in_table_name) noexcept {
                    enclosing().table_name =
                      istl::stringify_of<string_type>(stl::forward<StrvT>(in_table_name),
                                                      enclosing().db.alloc_pack);
                    return enclosing();
                }

                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& operator()(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

                template <istl::StringViewifiable StrvT>
                constexpr query_builder_type& operator=(StrvT&& in_table_name) noexcept {
                    return name(stl::forward<StrvT>(in_table_name));
                }

            } table;
        };
    } // namespace details

    template <typename DBType>
    struct query_builder : public details::query_builder_subclasses<DBType> {
        using database_type     = DBType;
        using traits_type       = typename database_type::traits_type;
        using string_type       = traits::general_string<traits_type>;
        using local_string_type = traits::local_string<traits_type>;
        using vector_of_strings = traits::localify_allocators<traits_type, stl::vector<local_string_type>>;
        using database_ref      = stl::add_lvalue_reference_t<database_type>;

        using driver_type     = typename database_type::driver_type;
        using grammar_type    = typename database_type::grammar_type;
        using connection_type = typename database_type::connection_type;


      private:
        database_ref      db;
        string_type       table_name;
        vector_of_strings columns;

        template <typename T>
        static constexpr bool is_stringify = istl::StringifiableOf<local_string_type, T>;

        // helper to convert the input to acceptable string type
        template <typename T>
            requires(is_stringify<T>)
        constexpr auto stringify(T&& str) const noexcept {
            return istl::stringify_of<local_string_type>(stl::forward<T>(str));
        }

      public:
        template <EnabledTraits ET>
        constexpr query_builder(ET&& et) noexcept
          : columns{et.alloc_pack.template local_allocator<local_string_type>()} {}

        /**
         * Set columns to be selected in the sql query.
         */
        template <typename T>
        constexpr query_builder& select(T&& col) noexcept {
            if constexpr (istl::Stringifiable<T>) {
                col.push_back(stringify(stl::forward<T>(col)));
            } else if constexpr (istl::ReadOnlyCollection<T>) {
            }
            return *this;
        }
    };



} // namespace webpp::sql


#endif // WEBPP_DDATABASE_SQL_QUERY_BUILDER_HPP
