// Created by moisrex on 7/3/21.

#ifndef WEBPP_QUERY_HPP
#define WEBPP_QUERY_HPP


namespace webpp::database {


    struct query {};

    template <Traits TraitsType>
    struct query_builder {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;

        // shortcuts:
        using strv = string_view_type;
        using str  = string_type;

        struct table_type {
            query_builder& builder;

            query_builder& operator[](strv table_name) {
                // todo
                return builder;
            }

        } tables{.builder = *this};
    };

} // namespace webpp::database


#endif // WEBPP_QUERY_HPP
