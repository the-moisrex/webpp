#ifndef WEBPP_HTTP_RESPONSE_H
#define WEBPP_HTTP_RESPONSE_H

#include "../convert/casts.hpp"
#include "../traits/traits_concepts.hpp"
#include "./response_concepts.hpp"
#include "./response_headers.hpp"
#include "headers.hpp"
#include "response_body.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

namespace webpp {

    template <typename T, typename... E>
    concept one_of = (stl::same_as<stl::remove_cvref_t<T>, E> || ...);

    /**
     * This class owns its data.
     */
    template <Traits TraitsType, typename EList, typename ResponseHeaderType, typename BodyType>
    class basic_response : public EList {

      public:
        using traits_type      = TraitsType;
        using body_type        = BodyType;
        using headers_type     = ResponseHeaderType;
        using string_view_type = typename traits_type::string_view_type;
        using string_type      = typename traits_type::string_type;

        body_type    body{};
        headers_type headers{};

        basic_response(auto&& arg1, auto&&... args) noexcept
          requires(!one_of<decltype(arg1), headers_type, status_code_type, body_type>)
          : EList{stl::forward<decltype(args)>(args)...} {}

        basic_response() noexcept                          = default;
        basic_response(basic_response const& res) noexcept = default;
        basic_response(basic_response&& res) noexcept      = default;

        basic_response(status_code_type err_code) noexcept : EList{}, headers{err_code} {}
        basic_response(status_code_type err_code, string_type const& b) noexcept
          : EList{},
            headers{err_code},
            body{b} {}
        basic_response(status_code_type err_code, string_type&& b) noexcept
          : EList{},
            headers{err_code},
            body{stl::move(b)} {}
        explicit basic_response(body_type const& b) noexcept : EList{}, body(b) {}
        explicit basic_response(body_type&& b) noexcept : EList{}, body(stl::move(b)) {}

        explicit basic_response(headers_type&& e) noexcept : EList{}, headers(stl::move(e)) {}
        explicit basic_response(headers_type const& e) noexcept : EList{}, headers(e) {}

        basic_response& operator=(basic_response const&) = default;
        basic_response& operator=(basic_response&& res) noexcept = default;

        basic_response& operator=(string_type const& str) noexcept {
            body.replace_string(str);
            return *this;
        }

        basic_response& operator=(string_type&& str) noexcept {
            body.replace_string(stl::move(str));
            return *this;
        }

        [[nodiscard]] bool operator==(basic_response const& res) const noexcept {
            return headers == res.headers && body == res.body;
        }

        [[nodiscard]] bool operator!=(basic_response const& res) const noexcept {
            return headers != res.headers || body != res.body;
        }

        void calculate_default_headers() noexcept {
            using header_field_type = typename decltype(headers)::field_type;
            if (stl::find(headers.cbegin(), headers.cend(), "Content-Type") != headers.cend())
                headers.emplace_back(
                  header_field_type{.name = "Content-Type", .value = "text/html; charset=utf-8"});

            if (stl::find(headers.cbegin(), headers.cend(), "Content-Length") != headers.cend())
                headers.emplace_back(
                  header_field_type{.name  = "Content-Length",
                                    .value = to_str_copy<traits_type>(body.str().size() * sizeof(char))});
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

        using EList::EList;


        /**
         * Append some extensions to this context type and get the type back
         */
        template <typename... E>
        using apply_extensions_type = typename original_extension_pack_type::template appended<
          E...>::unique::template extensie_type<traits_type, response_descriptor_type>;
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
          typename ExtensionListType::template extensie_type<TraitsType, response_headers_descriptor>,

          typename ExtensionListType::template extensie_type<TraitsType, response_body_descriptor>>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type =
          final_response<TraitsType, basic_response_descriptor, ExtensionListType, EList>;
    };


    template <Traits TraitsType, Extension... E>
    using simple_response =
      typename extension_pack<E...>::template extensie_type<TraitsType, basic_response_descriptor>;


    using fake_response_type = simple_response<fake_traits_type>;

} // namespace webpp
#endif // WEBPP_HTTP_RESPONSE_H
