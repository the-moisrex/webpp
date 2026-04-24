// Created by moisrex on 7/3/21.

#ifndef WEBPP_HTTP_BODIES_JSON_HPP
#define WEBPP_HTTP_BODIES_JSON_HPP


#include "../../json/defaultjson.hpp"
#include "../http_concepts.hpp"
#include "../response_body.hpp"
#include "../routes/router_concepts.hpp"
#include "./string.hpp"

namespace webpp::http {


    namespace details {

        /**
         * todo: remove this when you don't need to see the implementation
         */
        template <Context ContextType>
        struct json_context_extension : ContextType {
            using context_type       = ContextType;
            using json_response_type = typename context_type::response_type;
            using char_type          = typename context_type::char_type;
            using allocator_type     = typename context_type::allocator_type;
            using json_document_type = json::document<char_type, allocator_type>;

            using context_type::context_type; // inherit the constructors

            template <typename CharT>
            constexpr HTTPResponse auto json_file(stl::basic_string_view<CharT> const file_path) const noexcept {
                return json(stl::filesystem::path{file_path});
            }

            template <typename... Args>
            constexpr HTTPResponse auto json(Args&&... args) const noexcept {
                return this->response_body(stl::forward<Args>(args)...);
            }

            constexpr json_document_type json_doc() const {
                // fixme: should we use the default allocator?
                return json_document_type{};
            }
        };


    } // namespace details

    template <json::JSONDocument DocT, HTTPBody BodyType>
    constexpr DocT tag_invoke(deserialize_body_tag, stl::type_identity<DocT>, BodyType const& body) {
        DocT{body};
    }

    // Only sets the body
    template <json::JSONDocument DocT, HTTPBody BodyType>
    constexpr void tag_invoke(serialize_body_tag, DocT const& doc, BodyType& body) {
        using body_type   = stl::remove_cvref_t<BodyType>;
        using traits_type = typename body_type::traits_type;
        using string_type = traits::string<traits_type>;

        // delegate the string to the serialize_body for the strings:
        body = doc.template uglified<string_type>(get_alloc_for<string_type>(body));
    }

    // Set the header for json and pass it to the serialize_body to set the body as well
    template <json::JSONDocument DocT, HTTPResponse ResT>
    constexpr void tag_invoke(serialize_response_body_tag, DocT const& doc, ResT& res) {
        // todo: encoding support
        // todo: don't insert into headers directly
        res.headers.set("Content-Type", "application/json; charset=utf-8");

        serialize_body(stl::forward<DocT>(doc), res.body);
    }

} // namespace webpp::http


#endif // WEBPP_HTTP_BODIES_JSON_HPP
