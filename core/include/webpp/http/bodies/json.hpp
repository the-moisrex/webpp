// Created by moisrex on 7/3/21.

#ifndef WEBPP_HTTP_BODIES_JSON_HPP
#define WEBPP_HTTP_BODIES_JSON_HPP


#include "../../extensions/extension.hpp"
#include "../../json/defaultjson.hpp"
#include "../response_body.hpp"
#include "../response_concepts.hpp"
#include "../routes/context_concepts.hpp"
#include "string.hpp"

namespace webpp::http {


    namespace details {

        // todo: do this for every implementation

        template <Traits TraitsType, HTTPResponseBody BodyType>
        struct json_response_body_extension : BodyType {
          private:
            using super = BodyType;

          public:
            using traits_type    = TraitsType;
            using allocator_type = typename super::allocator_type;
            using string_type    = typename super::string_type;
            using char_type      = typename super::char_type;

          private:
            using alloc_type = allocator_type const&;

          public:
            using BodyType::BodyType;

            template <EnabledTraits ET, json::JSONDocument DocT>
                requires requires(ET et, DocT doc) {
                    super{et,
                          doc.template uglified<string_type>(
                            et.alloc_pack.template general_allocator<char_type>())};
                }
            constexpr json_response_body_extension(ET&& et, DocT const& doc)
              : super{et,
                      doc.template uglified<string_type>(
                        et.alloc_pack.template general_allocator<char_type>())} {}


            template <EnabledTraits ET, json::JSONDocument DocT>
                requires requires(ET et, DocT doc) {
                    super{doc.template uglified<string_type>(
                      et.alloc_pack.template general_allocator<char_type>())};
                }
            constexpr json_response_body_extension(ET&& et, DocT const& doc)
              : super{doc.template uglified<string_type>(
                  et.alloc_pack.template general_allocator<char_type>())} {}

            template <json::JSONDocument DocT>
            constexpr json_response_body_extension(DocT const& doc, alloc_type alloc = allocator_type{})
              : super{doc.template uglified<string_type>(alloc), alloc} {}
        };


        /**
         * This extension helps the user to create a response with the help of the context
         */

        template <Traits TraitsType, Context ContextType>
        struct json_context_extension : public ContextType {
            using context_type       = ContextType;
            using traits_type        = TraitsType;
            using json_response_type = typename context_type::response_type;
            using json_document_type = json::document<traits_type>;
            // ::template apply_extensions_type<details::json_response_body_extension>;

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
                if constexpr (!has_allocator && requires {
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

        template <Traits TraitsType, typename ResType>
        struct json_response_extension : public ResType {
            using body_type   = typename ResType::body_type;
            using traits_type = TraitsType;

            using ResType::ResType;

            // pass it to the body
            template <EnabledTraits ET, json::JSONDocument DocT>
                requires requires(ET et, DocT doc) {
                    ResType{et, body_type{et, doc}};
                }
            constexpr json_response_extension(ET&& et, DocT const& doc) noexcept
              : ResType{et, body_type{et, doc}} {
                this->add_headers();
            }



            template <EnabledTraits ET, json::JSONDocument DocT>
                requires requires(ET et, DocT doc) {
                    ResType{body_type{et, doc}};
                }
            constexpr json_response_extension(ET&& et, DocT const& doc) noexcept
              : ResType{body_type{et, doc}} {
                this->add_headers();
            }

          private:
            constexpr void add_headers() {
                // todo: encoding support
                this->headers.emplace_back("Content-Type", "application/json; charset=utf-8");
            }
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     */
    struct json_response {
        // we're going to use "string extension" as a place to store the data
        using dependencies = extension_pack<string_response>;

        using response_body_extensions = extension_pack<as_extension<details::json_response_body_extension>>;
        using response_extensions      = extension_pack<as_extension<details::json_response_extension>>;
        using context_extensions       = extension_pack<as_extension<details::json_context_extension>>;
    };


} // namespace webpp::http


#endif // WEBPP_HTTP_BODIES_JSON_HPP
