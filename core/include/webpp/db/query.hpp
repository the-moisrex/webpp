// Created by moisrex on 7/3/21.

#ifndef WEBPP_QUERY_HPP
#define WEBPP_QUERY_HPP

#include "../std/string.hpp"
#include "../traits/traits.hpp"

namespace webpp::database {


    template <istl::String StrT>
    struct query {
        using str_t = StrT;

      private:
        str_t tables_value;
        str_t select_value;

      public:
        template <istl::Stringifiable T>
        constexpr query& select(T&& val) noexcept {
            select_value = istl::stringify<str_t>(stl::forward<T>(val));
        }
    };

    template <Traits TraitsType>
    struct query_builder;

    /**
     * The holder of a single row from the results of a query in the database
     */
    template <Traits TraitsType>
    struct row {
        using traits_type        = TraitsType;
        using query_builder_type = query_builder<traits_type>;

        /**
         * Sync
         */
        struct sync_type {
            query_builder_type& builder;

        } sync{.builder = *this};

        /**
         * Async
         */
        struct async_type {
            query_builder_type& builder;

        } async{.builder = *this};
    };

    template <Traits TraitsType>
    struct query_builder {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using collection_type  = void;
        using row_type         = void; // collection_type should know the row type

        // shortcuts:
        using strv = string_view_type;
        using str  = string_type;

        struct table_type {
            query_builder& builder;

            query_builder& operator[](strv table_name) {
                // todo
                return builder;
            }

            table_type& join(strv table_name) {

                return *this;
            }

        } tables{.builder = *this};



        query_builder& commit() {
            // todo
            return *this;
        }


        collection_type get() {
            // todo
        }

        row_type row() {
            // todo
        }
    };

} // namespace webpp::database


#endif // WEBPP_QUERY_HPP
