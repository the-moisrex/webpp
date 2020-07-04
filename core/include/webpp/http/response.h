#ifndef WEBPP_HTTP_RESPONSE_H
#define WEBPP_HTTP_RESPONSE_H

#include "../traits/traits_concepts.h"
#include "./response_concepts.h"
#include "body.h"
#include "header.h"

#include <filesystem>
#include <fstream>
#include <memory>

namespace webpp {

    /**
     * This class owns its data.
     */
    template <Traits TraitsType, typename REL = empty_extension_pack,
              typename ResponseHeaderType = response_headers<TraitsType, REL>,
              typename BodyType           = response_body<TraitsType>>
    class basic_response : public REL {

      public:
        using traits_type  = TraitsType;
        using body_type    = BodyType;
        using headers_type = ResponseHeaderType;
        using str_view_t   = typename traits_type::string_view_type;
        using str_t        = typename traits_type::string_type;

        body_type    body;
        headers_type header;

        basic_response() noexcept                          = default;
        basic_response(basic_response const& res) noexcept = default;
        basic_response(basic_response&& res) noexcept      = default;

        basic_response(status_code_type err_code) noexcept : header{err_code} {
        }
        basic_response(status_code_type err_code, str_t const& b) noexcept : header{err_code}, body{b} {
        }
        basic_response(status_code_type err_code, str_t&& b) noexcept : header{err_code}, body{stl::move(b)} {
        }
        basic_response(str_t const& b) noexcept : body(b) {
        }
        basic_response(str_t&& b) noexcept : body(stl::move(b)) {
        }


        basic_response& operator=(basic_response const&) = default;
        basic_response& operator=(basic_response&& res) noexcept = default;
        basic_response& operator                                 =(str_t const& str) noexcept {
            body.replace_string(str);
            return *this;
        }
        basic_response& operator=(str_t&& str) noexcept {
            body.replace_string(stl::move(str));
            return *this;
        }

        [[nodiscard]] bool operator==(basic_response const& res) const noexcept {
            return body == res.body && header == res.header;
        }
        [[nodiscard]] bool operator!=(basic_response const& res) const noexcept {
            return body != res.body || header != res.header;
        }

        void calculate_default_headers() noexcept {
            if (!header.contains("Content-Type"))
                header.emplace("Content-Type", "text/html; charset=utf-8");

            if (!header.contains("Content-Length"))
                header.emplace("Content-Length", stl::to_string(body.str().size() * sizeof(char)));
        }


        // static methods:
        /*
        static response_t file(stl::filesystem::path const& file) noexcept;
        static response_t image(stl::string_view const& file) noexcept;
        static response_t json_file(stl::string_view const& file) noexcept;
         */
    };




    template <Traits TraitsType, typename DescriptorType, typename OriginalExtensionList, typename EList>
    struct final_response final : public EList {
        using traits_type                  = TraitsType;
        using elist_type                   = EList;
        using response_descriptor_type     = DescriptorType;
        using original_extension_pack_type = OriginalExtensionList;

        /**
         * Append some extensions to this context type and get the type back
         */
        template <typename... E>
        using apply_extensions_type = typename original_extension_pack_type::template appended<
          E...>::template extensie_type<traits_type, response_descriptor_type>;
    };



    struct basic_response_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::response_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::response_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = basic_response<
          TraitsType, EList,
          typename ExtensionListType::template extensie_type<TraitsType, response_header_descriptor>,

          typename ExtensionListType::template extensie_type<TraitsType, response_body_descriptor>>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type =
          final_response<TraitsType, basic_response_descriptor, ExtensionListType, EList>;
    };


} // namespace webpp
#endif // WEBPP_HTTP_RESPONSE_H
