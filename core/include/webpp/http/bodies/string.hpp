// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string_view.hpp"
#include "../../traits/traits.hpp"
#include "../response_concepts.hpp"
#include "../routes/context_concepts.hpp"

namespace webpp::http {


    namespace details {

        struct string_response_body_extension {

            template <Traits TraitsType>
            struct type {
                using traits_type      = TraitsType;
                using string_view_type = traits::string_view<traits_type>;
                using string_type      = traits::general_string<traits_type>;
                using allocator_type   = typename string_type::allocator_type;
                using char_type        = traits::char_type<traits_type>;

              protected: // the file extension will use the "content" directly
                using alloc_type    = allocator_type const&;
                string_type content = "";

              public:
                constexpr type(string_view_type str, alloc_type alloc = allocator_type{})
                  : content{str, alloc} {}

                template <typename... Args>
                constexpr type(Args&&... args) : content{stl::forward<Args>(args)...} {}

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

                [[nodiscard]] auto get_allocator() const noexcept {
                    return content.get_allocator();
                }
            };
        };

        template <Traits TraitsType>
        [[nodiscard]] bool operator==(
          typename TraitsType::string_view_type                                     str,
          typename string_response_body_extension::template type<TraitsType> const& strbody) noexcept {
            return strbody.str() == str;
        }

        template <Traits TraitsType>
        [[nodiscard]] bool operator!=(
          typename TraitsType::string_view_type                                     str,
          typename string_response_body_extension::template type<TraitsType> const& strbody) noexcept {
            return strbody.str() != str;
        }

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
            struct type : public stl::remove_cvref_t<ContextType> {
                using context_type         = stl::remove_cvref_t<ContextType>;
                using traits_type          = TraitsType;
                using string_response_type = typename context_type::response_type;
                // ::template apply_extensions_type<details::string_response_body_extension>;

                using context_type::context_type; // inherit the constructors

                template <typename... Args>
                constexpr HTTPResponse auto string(Args&&... args) const noexcept {
                    // check if there's an allocator in the args:
                    constexpr bool has_allocator = (istl::Allocator<Args> || ...);
                    using body_type              = typename string_response_type::body_type;
                    using value_type             = traits::char_type<traits_type>;
                    if constexpr (!has_allocator && requires {
                                      body_type{stl::forward<Args>(args)...,
                                                this->alloc_pack.template general_allocator<value_type>()};
                                  }) {
                        return string_response_type{
                          body_type{stl::forward<Args>(args)...,
                                    this->alloc_pack.template general_allocator<value_type>()}};
                    } else {
                        return string_response_type{body_type{stl::forward<Args>(args)...}};
                    }
                }
            };
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     *
     * This includes these extensions:
     *   - response body    : 1 extension (adds .str())
     *   - response         : 1 extension (adds string_view support to response)
     *   - context          : 1 extension (adds .string(...))
     */
    struct string_response {
        using response_body_extensions = extension_pack<details::string_response_body_extension>;
        using context_extensions       = extension_pack<details::string_context_extension>;
    };


} // namespace webpp::http

#endif // WEBPP_HTTP_BODIES_STRING_H
