// Created by moisrex on 7/3/21.

#ifndef WEBPP_JSON_HPP
#define WEBPP_JSON_HPP


#include "../../extensions/extension.hpp"
#include "../../json/defaultjson.hpp"
#include "../response_concepts.hpp"
#include "../routes/context_concepts.hpp"
#include "file.hpp"

namespace webpp::http {


    namespace details {

        struct json_response_body_extension {

            // we're going to use "string extension" as a place to store the data
            using dependencies = extension_pack<string_response_body_extension>;

            template <Traits TraitsType, ResponseBody BodyType>
            struct type : BodyType {
              private:
                using super = BodyType;

              public:
                using traits_type        = TraitsType;
                using allocator_type     = typename super::allocator_type;
                using string_type        = typename super::string_type;
                using json_document_type = json::document<traits_type>;

              private:
                using alloc_type = allocator_type const&;

              public:
                constexpr type(json_document_type const& doc, alloc_type alloc = allocator_type{})
                  : super{doc.template to_string<string_type>(alloc), alloc} {}
            };
        };


        /**
         * This extension helps the user to create a response with the help of the context
         */
        struct json_context_extension {

            template <Traits TraitsType, Context ContextType>
            struct type : public stl::remove_cvref_t<ContextType> {
                using context_type       = stl::remove_cvref_t<ContextType>;
                using traits_type        = TraitsType;
                using json_response_type = typename context_type::response_type;
                // ::template apply_extensions_type<details::json_response_body_extension>;

                using context_type::context_type; // inherit the constructors

                template <istl::StringViewifiable StrT>
                constexpr HTTPResponse auto json_file(StrT&& file_path) const noexcept {
                    // todo
                }

                template <typename... Args>
                constexpr HTTPResponse auto json(Args&&... args) const noexcept {
                    // check if there's an allocator in the args:
                    constexpr bool has_allocator = (istl::Allocator<Args> || ...);
                    using body_type              = typename json_response_type::body_type;
                    using value_type             = traits::char_type<traits_type>;
                    if constexpr (!has_allocator && requires {
                                      body_type{stl::forward<Args>(args)...,
                                                this->alloc_pack.template general_allocator<value_type>()};
                                  }) {
                        return json_response_type{
                          body_type{stl::forward<Args>(args)...,
                                    this->alloc_pack.template general_allocator<value_type>()}};
                    } else {
                        return json_response_type{body_type{stl::forward<Args>(args)...}};
                    }
                }
            };
        };

        struct json_response_extension {
            template <Traits TraitsType, typename ResType>
            struct type : public ResType {
                using ResType::ResType;
                using body_type          = typename ResType::body_type;
                using traits_type        = TraitsType;
                using json_document_type = json::document<traits_type>;

                // pass it to the body
                constexpr type(json_document_type const& doc) noexcept : ResType{body_type{doc}} {}
            };
        };

    } // namespace details


    /**
     * String Response Extension Pack.
     */
    struct json_response {
        using response_body_extensions = extension_pack<details::json_response_body_extension>;
        using response_extensions      = extension_pack<details::json_response_extension>;
        using context_extensions       = extension_pack<details::json_context_extension>;
    };


} // namespace webpp::http


#endif // WEBPP_JSON_HPP
