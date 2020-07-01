// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../traits/traits_concepts.h"
#include "../../extensions/extension.h"

#include <type_traits>
#include <utility>

namespace webpp {



    struct string_body {


        template <Traits TraitsType>
        struct type {
            using traits_type      = TraitsType;
            using string_type      = typename traits_type::string_type;
            using string_view_type = typename traits_type::string_view_type;

          private:
            string_type content = "";

          public:
            template <typename... Args>
            type(Args&&... args) noexcept : content{stl::forward<Args>(args)...} {
            }

            /**
             * @brief Get a reference to the body's string
             * @return string
             */
            [[nodiscard]] string_type const& str() const noexcept {
                return content;
            }
        };
    };


    struct string_response {
        template <Traits TraitsType>
        using response_body_extensions = extension_pack<string_body>;
    };


} // namespace webpp

#endif // WEBPP_HTTP_BODIES_STRING_H
