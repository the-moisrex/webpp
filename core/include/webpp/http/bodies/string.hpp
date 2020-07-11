// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits_concepts.hpp"

#include <type_traits>
#include <utility>

namespace webpp {



    struct string_body {


        template <Traits TraitsType>
        struct type {
            using traits_type      = TraitsType;
            using string_type      = typename traits_type::string_type;
            using string_view_type = typename traits_type::string_view_type;
            using allocator_type   = typename string_type::allocator_type;
            using alloc_type       = allocator_type const&;

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

            [[nodiscard]] bool operator==(string_view_type str) const noexcept {
                return str == content;
            }

            [[nodiscard]] bool operator!=(string_view_type str) const noexcept {
                return str != content;
            }
        };
    };

    template <Traits TraitsType>
    [[nodiscard]] bool
    operator==(typename TraitsType::string_view_type                  str,
               typename string_body::template type<TraitsType> const& strbody) noexcept {
        return strbody.str() == str;
    }

    template <Traits TraitsType>
    [[nodiscard]] bool
    operator!=(typename TraitsType::string_view_type                  str,
               typename string_body::template type<TraitsType> const& strbody) noexcept {
        return strbody.str() != str;
    }


    struct string_response {
        template <Traits TraitsType>
        using response_body_extensions = extension_pack<string_body>;
    };


} // namespace webpp

#endif // WEBPP_HTTP_BODIES_STRING_H
