// Created by moisrex on 7/3/21.

#ifndef WEBPP_HTTP_BODIES_JSON_HPP
#define WEBPP_HTTP_BODIES_JSON_HPP


#include "../../extensions/extension.hpp"
#include "../../json/defaultjson.hpp"
#include "../http_concepts.hpp"
#include "../response_body.hpp"
#include "../routes/router_concepts.hpp"
#include "string.hpp"

namespace webpp::http {


    namespace details {

        /**
         * This extension helps the user to create a response with the help of the context
         */
        template <Traits TraitsType, Context ContextType>
        struct json_context_extension : public ContextType {
            using context_type       = ContextType;
            using traits_type        = TraitsType;
            using json_response_type = typename context_type::response_type;
            using json_document_type = json::document<traits_type>;

            using context_type::context_type; // inherit the constructors

            template <istl::StringViewifiable StrT>
            constexpr HTTPResponse auto json_file(StrT&& file_path) const noexcept {
                return json(stl::filesystem::path{istl::string_viewify(stl::forward<StrT>(file_path))});
            }

            template <typename... Args>
            constexpr HTTPResponse auto json(Args&&... args) const noexcept {
                // check if there's an allocator in the args:
                constexpr bool has_allocator = (Allocator<Args> || ...);
                using value_type             = traits::char_type<traits_type>;
                if constexpr (!has_allocator &&
                              requires {
                                  json_response_type::with_body(
                                    stl::forward<Args>(args)...,
                                    this->alloc_pack.template general_allocator<value_type>());
                              }) {
                    return json_response_type::with_body(
                      stl::forward<Args>(args)...,
                      this->alloc_pack.template general_allocator<value_type>());
                } else {
                    return json_response_type::with_body(stl::forward<Args>(args)...);
                }
            }


            constexpr json_document_type json_doc() const {
                // fixme: should we use the default allocator?
                return json_document_type{};
            }
        };


    } // namespace details


    /**
     * String Response Extension Pack.
     */
    struct json_body {
        using context_extensions = extension_pack<as_extension<details::json_context_extension>>;
    };


    template <json::JSONDocument DocT, HTTPBody BodyType>
    constexpr void deserialize_body(BodyType const& body) {
        // todo
    }


    // Only sets the body
    template <json::JSONDocument DocT, HTTPBody BodyType>
    constexpr void serialize_body(DocT const& doc, BodyType& body) {
        using body_type   = stl::remove_cvref_t<BodyType>;
        using traits_type = typename body_type::traits_type;
        using string_type = traits::general_string<traits_type>;

        // delegate the string to the serialize_body for the strings:
        body = doc.template uglified<string_type>(alloc::general_alloc_for<string_type>(body));
    }

    // Set the header for json and pass it to the serialize_body to set the body as well
    template <json::JSONDocument DocT, HTTPResponse ResT>
    constexpr void serialize_response_body(DocT const& doc, ResT& res) {
        using response_type = stl::remove_cvref_t<ResT>;
        using body_type     = typename response_type::body_type;
        using traits_type   = typename body_type::traits_type;
        using string_type   = traits::general_string<traits_type>;

        // todo: encoding support
        // todo: don't insert into headers directly
        res.headers.emplace_back("Content-Type", "application/json; charset=utf-8");

        serialize_body<DocT>(stl::forward<DocT>(doc), res.body);
    }

} // namespace webpp::http


#endif // WEBPP_HTTP_BODIES_JSON_HPP
