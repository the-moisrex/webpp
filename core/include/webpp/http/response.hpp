#ifndef WEBPP_HTTP_RESPONSE_H
#define WEBPP_HTTP_RESPONSE_H

#include "../convert/casts.hpp"
#include "../traits/traits.hpp"
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
    template <Traits TraitsType, typename EList, typename ResponseHeaderType, ResponseBody BodyType>
    class basic_response : public extension_wrapper<EList> {

      public:
        using traits_type      = TraitsType;
        using body_type        = BodyType;
        using headers_type     = ResponseHeaderType;
        using string_view_type = traits::string_view<traits_type>;
        using string_type      = traits::general_string<traits_type>;
        using elist_type       = extension_wrapper<EList>;

        body_type    body{};
        headers_type headers{};

        basic_response(auto&& arg1, auto&&... args) noexcept
          requires(!one_of<decltype(arg1), headers_type, http::status_code_type, body_type>)
          : elist_type{stl::forward<decltype(args)>(args)...} {}

        basic_response() noexcept                          = default;
        basic_response(basic_response const& res) noexcept = default;
        basic_response(basic_response&& res) noexcept      = default;

        basic_response(http::status_code_type err_code) noexcept : elist_type{}, headers{err_code} {}
        basic_response(http::status_code_type err_code, string_type const& b) noexcept
          : elist_type{},
            headers{err_code},
            body{b} {}
        basic_response(http::status_code_type err_code, string_type&& b) noexcept
          : elist_type{},
            headers{err_code},
            body{stl::move(b)} {}
        explicit basic_response(body_type const& b) noexcept : elist_type{}, body(b) {}
        explicit basic_response(body_type&& b) noexcept : elist_type{}, body(stl::move(b)) {}

        explicit basic_response(headers_type&& e) noexcept : elist_type{}, headers(stl::move(e)) {}
        explicit basic_response(headers_type const& e) noexcept : elist_type{}, headers(e) {}

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
            using header_field_type = typename headers_type::field_type;
            using str_t             = typename header_field_type::string_type;
            if (stl::find(headers.cbegin(), headers.cend(), "Content-Type") != headers.cend())
                headers.push_back(
                  header_field_type{.name = "Content-Type", .value = "text/html; charset=utf-8"});

            if (stl::find(headers.cbegin(), headers.cend(), "Content-Length") != headers.cend()) {
                str_t value{headers.get_allocator()};
                append_to(value, body.str().size() * sizeof(char));
                headers.push_back(header_field_type{.name = "Content-Length", .value = stl::move(value)});
            }
        }


        // static methods:
        // static auto file(stl::filesystem::path const& file) noexcept;
        // static auto image(stl::string_view const& file) noexcept;
        // static auto json_file(stl::string_view const& file) noexcept;
    };




    template <Traits TraitsType, typename DescriptorType, typename OriginalExtensionList, typename EList>
    struct final_response final : public extension_wrapper<EList> {
        using traits_type                  = TraitsType;
        using elist_type                   = extension_wrapper<EList>;
        using response_descriptor_type     = DescriptorType;
        using original_extension_pack_type = OriginalExtensionList;

        using elist_type::elist_type;


        /**
         * Append some extensions to this context type and get the type back
         */
        template <typename... E>
        using apply_extensions_type =
          typename details::unique_extensions<typename original_extension_pack_type::template appended<
            E...>>::type::template extensie_type<traits_type, response_descriptor_type>;
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
          TraitsType,
          EList,
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


    // using fake_response_type = simple_response<fake_traits_type>;

} // namespace webpp
#endif // WEBPP_HTTP_RESPONSE_H
