// Created by moisrex on 6/28/20.

#ifndef WEBPP_HTTP_BODIES_STRING_H
#define WEBPP_HTTP_BODIES_STRING_H

#include "../../extensions/extension.hpp"
#include "../../std/concepts.hpp"
#include "../../std/string_view.hpp"
#include "../../traits/traits_concepts.hpp"
#include "../routes/context_concepts.hpp"

#include <type_traits>
#include <utility>

namespace webpp {


    namespace details {

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
                  : content{str, alloc} {}

                template <typename... Args>
                constexpr type(Args&&... args) noexcept : content{stl::forward<Args>(args)...} {}

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

    } // namespace details

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
            template <typename TraitsType, typename ContextType>
            struct type : public stl::remove_cvref_t<ContextType> {
                using context_type = stl::remove_cvref_t<ContextType>;
                using traits_type  = TraitsType;
                using string_response_type =
                  typename context_type::response_type::template apply_extensions_type<details::string_body>;

                template <typename... Args>
                constexpr type(Args&&... args) noexcept(noexcept(context_type{stl::forward<Args>(args)...})) :
                    context_type{stl::forward<Args>(args)...} {}

                template <typename... Args>
                constexpr Response auto string(Args&&... args) const noexcept {
                    // check if there's an allocator in the args:
                    constexpr bool has_allocator = (istl::Allocator<Args> || ...);
                    using body_type              = typename string_response_type::body_type;
                    if constexpr (has_allocator) {
                        return string_response_type{body_type{stl::forward<Args>(args)...}};
                    } else if constexpr (
                      requires {
                          body_type{stl::forward<Args>(args)..., context_type::get_allocator()};
                      }) {
                        auto body = body_type{stl::forward<Args>(args)..., context_type::get_allocator()};
                        auto res  = string_response_type{body};
                        return res;
                    } else {
                        return string_response_type{body_type{stl::forward<Args>(args)...}};
                    }
                }
            };
        };

        struct string_response_extension {
            template <Traits TraitsType, typename ResType>
            struct type : public ResType {
                using ResType::ResType;
                using body_type   = typename ResType::body_type;
                using string_type = typename body_type::string_type;

                constexpr type(istl::ConvertibleToStringView auto&& str_view) noexcept
                  : ResType{body_type{stl::forward<decltype(str_view)>(str_view)}} {}

                constexpr type(string_type const& str) noexcept : ResType{body_type{str}} {}
                constexpr type(string_type&& str) noexcept : ResType{body_type{stl::move(str)}} {}
            };
        };

        using response_body_extensions = extension_pack<details::string_body>;
        using response_extensions      = extension_pack<string_response_extension>;
        using context_extensions       = extension_pack<string_context_extension>;
    };


} // namespace webpp

#endif // WEBPP_HTTP_BODIES_STRING_H
