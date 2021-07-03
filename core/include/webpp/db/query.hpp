// Created by moisrex on 7/3/21.

#ifndef WEBPP_QUERY_HPP
#define WEBPP_QUERY_HPP


namespace webpp::database {


    struct query {};


    /**
     * The holder of a single row from the results of a query in the database
     */
    struct row {

        /**
         * Sync
         */
        struct sync_type {
            query_builder& builder;

        } sync{.builder = *this};

        /**
         * Async
         */
        struct async_type {
            query_builder& builder;

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
