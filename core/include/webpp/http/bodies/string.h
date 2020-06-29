// Created by moisrex on 6/28/20.

#ifndef WEBPP_STRING_H
#define WEBPP_STRING_H

#include "../../traits/traits_concepts.h"

#include <type_traits>
#include <utility>

namespace webpp {

    template <Traits TraitsType>
    struct string_body_extension {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;

      private:
        string_type content = "";

      public:
        template <typename... Args>
        string_body_extension(Args&&... args) noexcept : content{stl::forward<Args>(args)...} {
        }

        /**
         * @brief Get a reference to the body's string
         * @return string
         */
        [[nodiscard]] string_type const& str() const noexcept {
            return content;
        }
    };



    struct string_body {
        template <Traits TraitsType>
        using type = string_body_extension<TraitsType>;
    };




} // namespace webpp

#endif // WEBPP_STRING_H
