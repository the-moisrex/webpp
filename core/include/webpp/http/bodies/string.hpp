// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits_concepts.hpp"
#include "../../std/concepts.hpp"

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

          private:
            using alloc_type    = allocator_type const&;
            string_type content = "";

          public:
            constexpr type(string_view_type str, alloc_type alloc = allocator_type{}) noexcept
              : content{str, alloc} {
            }

            template <typename... Args>
            constexpr type(Args&&... args) noexcept : content{stl::forward<Args>(args)...} {
            }

            /**
             * @brief Get a reference to the body's string
             * @return string
             */
            [[nodiscard]] string_type const& str() const noexcept {
                return content;
            }

            constexpr operator string_type() const noexcept {
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
    [[nodiscard]] bool operator==(typename TraitsType::string_view_type                  str,
                                  typename string_body::template type<TraitsType> const& strbody) noexcept {
        return strbody.str() == str;
    }

    template <Traits TraitsType>
    [[nodiscard]] bool operator!=(typename TraitsType::string_view_type                  str,
                                  typename string_body::template type<TraitsType> const& strbody) noexcept {
        return strbody.str() != str;
    }


    struct string_response {

        /**
         * This extension helps the user to create a response with the help of the context
         *
         *   ctx.string_type{"this is a response"}
         *   ctx.str_t{"this is nice"}
         *   ctx.string("hello world")
         *
         * The reason for preferring "string" over "string_type" is that the allocator is handled correctly.
         */
        struct string_context_extension {
            template <Traits TraitsType, Context ContextType>
            struct type : public ContextType {
                using string_response =
                  typename ContextType::response_type::template apply_extension_type<string_body>;

                using ContextType::ContextType;

                template <typename... Args>
                constexpr auto string(Args&&... args) const noexcept {
                    // check if there's an allocator in the args:
                    constexpr bool has_allocator = (istl::Allocator<Args> || ...);
                    if constexpr (has_allocator) {
                        return string_response{stl::forward<Args>(args)...};
                    } else {
                        return string_response{stl::forward<Args>(args)..., ContextType::alloc};
                    }
                }
            };
        };

        using response_body_extensions = extension_pack<string_body>;
        using context_extensions       = extension_pack<string_context_extension>;
    };


} // namespace webpp

#endif // WEBPP_HTTP_BODIES_STRING_H
