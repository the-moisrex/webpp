#ifndef WEBPP_HTTP_FILE_H
#define WEBPP_HTTP_FILE_H

#include "../../extensions/extension.hpp"
#include "../../memory/object.hpp"
#include "../../traits/traits.hpp"
#include "string.hpp"


namespace webpp::http {

    namespace details {


        template <Traits TraitsType, typename Mother>
        struct file_context_extension : public Mother {
            using traits_type   = TraitsType;
            using response_type = typename Mother::response_type;
            using body_type     = typename response_type::body_type;
            using string_type   = typename body_type::string_type;
            using char_type     = traits::char_type<traits_type>;

            // ctor
            using Mother::Mother;


          public:
        };



        template <Traits TraitsType, typename Mother>
        struct response_body_file_extension : public Mother {
            using traits_type   = TraitsType;
            using body_type     = Mother;
            using string_type   = typename body_type::string_type;
            using char_type     = traits::char_type<traits_type>;
            using ifstream_type = typename stl::basic_ifstream<char_type, stl::char_traits<char_type>>;

            // ctor
            using Mother::Mother;


          public:
        };
    } // namespace details


    struct file_response {
        using response_body_extensions = extension_pack<as_extension<details::string_response_body_extension>,
                                                        as_extension<details::response_body_file_extension>>;
        using context_extensions       = extension_pack<as_extension<details::file_context_extension>>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_FILE_H
